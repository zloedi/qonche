// Copyright (c) 2020 Stoiko Todorov
// This work is licensed under the terms of the MIT license.  
// For a copy, see https://opensource.org/licenses/MIT.


// A tiny quake style debug console in a single C header with no dependencies.
//
// This lib PROVIDES: 
// * editing:
//    - cursor movement
//    - erase characters at the cursor
//    - insert string at the cursor
//
// * printing:
//    - add string to the pager 
//    - paging 
//
// This lib DOES NOT PROVIDE:
// * (font) rendering of any kind
// * utf-8 / multibyte support
// * advanced cursor movement i.e. between words
// * commands parsing, autocompletion, history, etc.
// * printf-like formatting
// * multithreading support
//
//
// Usage
//
// 1. Define QONCHE_IMPLEMENTATION above the include in the c/c++ file where you 
// want the implementation to reside.
// 2. Define the QON_DrawChar macro with your own implementation of 
// QON_DrawChar().
// 3. Invoke QON_Draw() which will eventually call back QON_DrawChar().
// 4. Wire up the input routines i.e. to the matching key events in your code.

#ifndef QONCHE_H
#define QONCHE_H

#ifdef __cplusplus
extern "C" {
#endif

// == core ==

// Redefine this macro with your own implementation of DrawChar
#ifndef QON_DrawChar
#define QON_DrawChar QON_DrawChar_impl
#endif
// Draws character 'c' at (x,y) coordinates, where coordinates are in characters count.
// 'isUnderCursor' is set to true when the cursor is on this character.
// 'drawCharParam' is an optional pointer parameter.
void QON_DrawChar( int c, int x, int y, int isUnderCursor, void *drawCharParam );

// Draws the entire console. 'conWidth' and 'conHeight' are in characters count.
void QON_Draw( int conWidth, int conHeight, void *drawCharParam );

// == input ==

void QON_MoveRight( int numChars );
void QON_MoveLeft( int numChars );
// Deletes 'numChars' starting at the cursor
void QON_Delete( int numChars );
// Deletes 'numChars' to the left of the cursor, excluding the cursor
void QON_Backspace( int numChars );
void QON_Insert( const char *str );
int QON_Print( const char *str );
void QON_PageUp( void );
void QON_PageDown( void );
// Prints the contents of the command field and clears the command
void QON_EmitCommand( int bufSize, char *outBuf );
// Available only if QON_CUSTOM_DRAW_CALLBACKS is enabled.
// Same as QON_Print but registers an optional 'draw' callback on the first 
// character of 'str'
int QON_PrintWithCallback( void ( *cb )( int x, int y, void *param ), 
                                                            const char *str );




#ifdef QONCHE_IMPLEMENTATION



// There are two distinct text fields on the console: the 'PAGER' and the 
// 'COMMAND': 
// The 'PAGER' is where the Print-s are logged.
// The 'COMMAND' is where the command editing by moving the 'CURSOR' 
// around is done. 


// TODO: Clear-pager API


// Redefine these macros for custom sizes (keep them power of two)
#ifndef QON_MAX_CMD
#define QON_MAX_CMD 512
#endif

#ifndef QON_MAX_PAGER
#ifdef QON_DEBUG
#define QON_MAX_PAGER 1024
#else
#define QON_MAX_PAGER (16*1024)
#endif
#endif

#define QON_MAX_PAGER_MASK (QON_MAX_PAGER - 1)

// Redefine this macro for custom prompt
#ifndef QON_PROMPT
#define QON_PROMPT "] "
#endif
#define QON_PROMPT_LEN (sizeof(QON_PROMPT)-1)

#ifdef QON_DEBUG
#define QON_TRAILING_SPACE "<"
#else
#define QON_TRAILING_SPACE " "
#endif

#ifdef QON_CUSTOM_DRAW_CALLBACKS
static void ( *qon_drawCallbacks[QON_MAX_PAGER] )( int x, int y, void *param );
#define QON_DrawCallback( i, x, y, param ) \
                (qon_drawCallbacks[(i)&QON_MAX_PAGER_MASK]((x),(y),(param)))
#define QON_Printn( str, n ) \
                QON_PrintWithCallbackn((str),(n),QON_Dummy_f)
#define QON_PrintWithCallback QON_PrintWithCallback_impl
#else
#define QON_DrawCallback(...)
#define QON_Printn QON_PrintClamp
#define QON_PrintWithCallback(...)
#endif

static int qon_prevPage = QON_MAX_PAGER;
static int qon_nextPage = QON_MAX_PAGER;
static int qon_currPage = QON_MAX_PAGER;
static int qon_pagerHead = QON_MAX_PAGER;
static char qon_pager[QON_MAX_PAGER];

static int qon_cursor = QON_PROMPT_LEN;
static char qon_cmdBuf[QON_MAX_CMD] = QON_PROMPT QON_TRAILING_SPACE;

static inline int QON_Min( int a, int b ) {
    return a < b ? a : b;
}

static inline int QON_Max( int a, int b ) {
    return a > b ? a : b;
}

static inline int QON_Len( const char *p ) {
    int n;
    for ( n = 0; p[n]; n++ ) {}
    return n;
}

static inline void QON_Zero( char *p, int n ) {
    for ( int i = 0; i < n; i++ ) {
        p[i] = 0;
    }
}

void QON_MoveRight( int numChars ) {
    int max = QON_Len( qon_cmdBuf ) - 1 - qon_cursor;
    numChars = QON_Min( numChars, max );
    qon_cursor += numChars;
}

void QON_MoveLeft( int numChars ) {
    int max = qon_cursor - QON_PROMPT_LEN;
    numChars = QON_Min( numChars, max );
    qon_cursor -= numChars;
}

void QON_Delete( int numChars ) {
    int bufLen = QON_Len( qon_cmdBuf );
    // keep the trailing space
    int max = ( bufLen - 1 ) - qon_cursor;
    int shift = QON_Min( numChars, max );
    if ( shift ) {
        int n = bufLen - shift;
        for ( int i = qon_cursor; i < n; i++ ) {
            qon_cmdBuf[i] = qon_cmdBuf[i + shift];
        }
        QON_Zero( &qon_cmdBuf[n], shift );
    }
}

void QON_Backspace( int numChars ) {
    // always keep the prompt
    int max = qon_cursor - QON_PROMPT_LEN;
    int shift = QON_Min( numChars, max );
    if ( shift ) {
        int bufLen = QON_Len( qon_cmdBuf );
        for ( int i = qon_cursor; i < bufLen; i++ ) {
            qon_cmdBuf[i - shift] = qon_cmdBuf[i];
        }
        QON_Zero( &qon_cmdBuf[bufLen - shift], shift );
        qon_cursor -= shift;
    }
}

void QON_Insert( const char *str ) {
    int bufLen = QON_Len( qon_cmdBuf );
    // always leave a trailing space along with the term zero
    int max = ( QON_MAX_CMD - 2 ) - bufLen;
    int shift = QON_Min( QON_Len( str ), max );
    if ( shift ) {
        for ( int i = bufLen - 1; i >= qon_cursor; i-- ) {
            qon_cmdBuf[i + shift] = qon_cmdBuf[i];
        }
        for ( int i = 0; i < shift; i++ ) {
            qon_cmdBuf[qon_cursor + i] = str[i];
        }
        // deletion always fills zeros, no need to zero terminate here
        qon_cursor += shift;
    }
}

static int QON_PrintClamp( const char *str, int n ) {
    int i;
    for ( i = 0; i < n && str[i]; i++, qon_pagerHead++ ) {
        qon_pager[qon_pagerHead & QON_MAX_PAGER_MASK] = str[i];
    }
    qon_pager[qon_pagerHead & QON_MAX_PAGER_MASK] = 0;
    qon_currPage = qon_pagerHead;
    return i;
}

#ifdef QON_CUSTOM_DRAW_CALLBACKS

static void QON_Dummy_f( int x, int y, void *param ) {(void)x,(void)y,(void)param;}

static int QON_PrintWithCallbackn( const char *str, int n,
                                    void ( *cb )( int x, int y, void *param ) ) {
    // replace callbacks in the pager string
    qon_drawCallbacks[qon_pagerHead & QON_MAX_PAGER_MASK] = cb;
    for ( int i = 1; i < n && str[i]; i++ ) {
        int idx = qon_pagerHead + i;
        qon_drawCallbacks[idx & QON_MAX_PAGER_MASK] = QON_Dummy_f;
    }

    // actual printing to pager
    return QON_PrintClamp( str, n );
}

int QON_PrintWithCallback_impl( void ( *cb )( int x, int y, void *param ), 
                                                            const char *str ) {
    return QON_PrintWithCallbackn( str, QON_MAX_PAGER, cb );
}

#endif // QON_CUSTOM_DRAW_CALLBACKS

int QON_Print( const char *str ) {
    return QON_Printn( str, QON_MAX_PAGER );
}

void QON_EmitCommand( int bufSize, char *outBuf ) {
    // skip the trailing space
    int len = QON_Len( qon_cmdBuf ) - 1;

    // leave out terminating zero and prompt
    int n = QON_Min( bufSize - 1, len - QON_PROMPT_LEN );
    for ( int i = 0; i < n; i++ ) {
        outBuf[i] = qon_cmdBuf[QON_PROMPT_LEN + i];
    }
    outBuf[n] = 0;

    QON_Printn( qon_cmdBuf, len );
    QON_Print( "\n" );
    QON_MoveLeft( QON_MAX_CMD );
    QON_Delete( QON_MAX_CMD );
}

void QON_PageUp( void ) {
    qon_currPage = qon_prevPage;
}

void QON_PageDown( void ) {
    qon_currPage = qon_nextPage;
}

static inline int QON_GetPagerChar( int i ) {
    return qon_pagerHead - i >= QON_MAX_PAGER ? 0 : qon_pager[i & QON_MAX_PAGER_MASK];
}

static inline int QON_IsLineInc( int idx, int x, int conWidth, int c ) {
    // handle the case where the last character in the buffer is not a new line
    return ( idx == qon_pagerHead - 1 && c != '\n' ) 
            || x == conWidth - 1 
            || ! c 
            || c == '\n';
}

void QON_Draw( int conWidth, int conHeight, void *drawCharParam ) {
    int numCmdLines = 0;

    // == command field ==

    {
        // ignore the trailing space when counting lines
        int cmdLen = QON_Len( qon_cmdBuf ) - 1;

        // command may take more than one screen of lines, clamp it to screen
        // always atleast one line
        numCmdLines = QON_Min( conHeight, cmdLen / conWidth + 1 );

        // cursor is always inside the window
        int numLinesToCursor = qon_cursor / conWidth + 1;

        int start = QON_Max( numLinesToCursor - conHeight, 0 ) * conWidth;
        int numChars = QON_Min( conHeight * conWidth, QON_MAX_CMD - start );
        int cmdCaret = ( conHeight - numCmdLines ) * conWidth;
        int caretCursor = cmdCaret + qon_cursor - start;
        for ( int i = 0; i < numChars; i++, cmdCaret++ ) {
            int c = qon_cmdBuf[i + start]; 

            if ( ! c ) {
                break;
            }

            int x = cmdCaret % conWidth;
            int y = cmdCaret / conWidth;
            QON_DrawChar( c, x, y, cmdCaret == caretCursor, drawCharParam );
        }

        // out of lines for the pager
        if ( numCmdLines == conHeight ) {
            return;
        }
    }

    // == pager ==

    {
        int maxY = conHeight - numCmdLines;
        int start = QON_Min( qon_pagerHead, qon_currPage ) - 1;
        int x = 0;
        int y = maxY;

        while ( 1 ) {
            int c = QON_GetPagerChar( start );

            if ( y < 0 && ( ! c || c == '\n' ) ) {
                x = 0;
                break;
            }
            
            if ( QON_IsLineInc( start, x, conWidth, c ) ) {
                x = 0;
                y--;
            } else {
                x++;
            }

            start--;
        }

        qon_prevPage = qon_currPage;

        int i;
        for ( i = start + 1; y < 2 * maxY - 1; i++ ) {
            int c = QON_GetPagerChar( i );

            if ( y >= 0 && y < maxY ) {

                // previous page ends with the first line of the current one
                // don't bother going above any zeros/out of buffer
                if ( y == 1 && c ) {
                    qon_prevPage = i;
                }

                if ( ! c ) {
                    QON_DrawChar( '~', 0, y, 0, drawCharParam );
                } else {
                    QON_DrawCallback( i, x, y, drawCharParam );
                    if ( c == '\n' ) {
#ifdef QON_DEBUG
                        // debug draw the new lines
                        QON_DrawChar( '0' + ( i % 10 ), x, y, 0, drawCharParam );
#endif
                    } else {
                        QON_DrawChar( c, x, y, 0, drawCharParam );
                    }
                }
            }

            if ( QON_IsLineInc( i, x, conWidth, c ) ) {
                x = 0;
                y++;
            } else {
                x++;
            } 
        }

        qon_nextPage = i;
    }
}

#endif // QONCHE_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // QONCHE_H
