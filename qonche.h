// Copyright (c) 2020 Stoiko Todorov
// This work is licensed under the terms of the MIT license.  
// For a copy, see https://opensource.org/licenses/MIT.


// A tiny quake-like console in a single C header with no dependencies.
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
// * utf-8 / multibyte support
// * advanced cursor movement i.e. between words
// * commands parsing, autocompletion, history, etc.
// * printf-like formatting
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

// TODO: expose the command buffer so it could be processed
// TODO: support for <tag> </tag> tags maybe supply a mechanism to callback on any char sequence
// TODO: API to access a sequence of characters on screen i.e. by mouse selection


#ifndef QONCHE_H
#define QONCHE_H

#ifdef __cplusplus
extern "C" {
#endif

// main
#ifndef QON_DrawChar
#define QON_DrawChar QON_DrawChar_impl
#endif
void QON_DrawChar( int c, int x, int y, int isUnderCursor, void *data );
void QON_Draw( int conWidth, int conHeight, void *drawCharParam );

// input 
void QON_MoveRight( int numChars );
void QON_MoveLeft( int numChars );
void QON_DelFront( int numChars );
void QON_DelBack( int numChars );
void QON_Insert( const char *str );
void QON_Print( const char *str );
void QON_PageUp( void );
void QON_PageDown( void );
void QON_EmitCommand( void );




#ifdef QONCHE_IMPLEMENTATION



// There are two distinct text fields on the console: the 'PAGER' and the 
// 'COMMAND'. 
// The 'PAGER' is where the Print-s are logged.
// The 'COMMAND' is where the command editing by moving the 'CURSOR' 
// around is done. 



// You can redefine the following macros in order to change buffer sizes, prompt 
// string, etc. Keep them power of two.

#ifndef QON_MAX_CMD
#define QON_MAX_CMD 512
#endif

#ifndef QON_MAX_PAGER
#ifdef QON_DEBUG
#define QON_MAX_PAGER (1024)
#else
#define QON_MAX_PAGER (16*1024)
#endif
#endif

#ifndef QON_PROMPT
#define QON_PROMPT "] "
#endif
static const int qon_promptLen = sizeof( QON_PROMPT ) - 1;

#ifdef QON_DEBUG
#define QON_TRAILING_SPACE "<"
#else
#define QON_TRAILING_SPACE " "
#endif

static int qon_prevPage = QON_MAX_PAGER;
static int qon_nextPage = QON_MAX_PAGER;
static int qon_currPage = QON_MAX_PAGER;
static int qon_pagerHead = QON_MAX_PAGER;
static char qon_pager[QON_MAX_PAGER];

static int qon_cursor = qon_promptLen;
static char qon_cmdBuf[QON_MAX_CMD] = (QON_PROMPT QON_TRAILING_SPACE);

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

static inline void QON_Cpy( char *dst, const char *src, int sz ) {
    for ( int i = 0; i < sz; i++ ) {
        dst[i] = src[i];
    }
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
    int max = qon_cursor - qon_promptLen;
    numChars = QON_Min( numChars, max );
    qon_cursor -= numChars;
}

void QON_DelFront( int numChars ) {
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

void QON_DelBack( int numChars ) {
    // always keep the prompt
    int max = qon_cursor - qon_promptLen;
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
        QON_Cpy( &qon_cmdBuf[qon_cursor], str, shift );
        // deletion always fills zeros, no need to zero terminate here
        qon_cursor += shift;
    }
}

static void QON_Printn( const char *str, int n ) {
    for ( int i = 0; i < n && str[i]; i++, qon_pagerHead++ ) {
        qon_pager[qon_pagerHead & ( QON_MAX_PAGER - 1 )] = str[i];
    }
    qon_pager[qon_pagerHead & ( QON_MAX_PAGER - 1 )] = 0;
    qon_currPage = qon_pagerHead;
}

void QON_Print( const char *str ) {
    QON_Printn( str, QON_MAX_PAGER );
}

void QON_EmitCommand( void ) {
    // skip the trailing space
    QON_Printn( qon_cmdBuf, QON_Len( qon_cmdBuf ) - 1 );
    QON_Print( "\n" );
    QON_MoveLeft( QON_MAX_CMD );
    QON_DelFront( QON_MAX_CMD );
}

void QON_PageUp( void ) {
    qon_currPage = qon_prevPage;
}

void QON_PageDown( void ) {
    qon_currPage = qon_nextPage;
}

static inline int QON_GetPagerChar( int i ) {
    int mask = QON_MAX_PAGER - 1;
    return qon_pagerHead - i >= QON_MAX_PAGER ? 0 : qon_pager[i & mask];
}

void QON_Draw( int conWidth, int conHeight, void *drawCharParam ) {
    int numCmdLines;

    // == command field ==

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

    // == pager ==

    int last = QON_Min( qon_pagerHead, qon_currPage );
    int max = conWidth * ( conHeight - numCmdLines );

    // scan for page-down
    qon_nextPage = qon_pagerHead;
    for ( int i = last, caret = 0, n = 0; i < qon_pagerHead; i++ ) {
        int c = QON_GetPagerChar( i );

        if ( ! c || c == '\n' ) {
            int r = caret % conWidth;
            if ( n ) {
                caret += r ? conWidth - r : 0;
            } else {
                caret += r ? conWidth - r : conWidth;
            }
            n = 0;
        } else {
            caret++;
            n++;
        } 
        
        if ( caret > max ) {
            qon_nextPage = i;
            break;
        } 
    }

    // both page index and pager head are one char off
    last--;

    // mandatory new line at buf last, skip it if in buffer
    if ( QON_GetPagerChar( last ) == '\n' ) {
        last--;
    } 

    // scan for prev page as we go
    qon_prevPage = 0;

    int pgrCaret = max - 1;

    for ( int i = last; ; ) {

        // find a string enclosed by new line/term zero

        int n;
        for ( n = 0; n < max * 2; n++ ) {
            int c = QON_GetPagerChar( i - n );

            // handle new lines specially
            // i.e. new line at the end of a full line is ignored

            if ( ! c || c == '\n' ) {
                if ( ! n ) {
                    pgrCaret -= conWidth;
                } else {
                    int r = n % conWidth;
                    pgrCaret -= r ? conWidth - r : 0;
                }
                break;
            }
        }

        // go up along the string print characters and scan for prev page

        for ( int j = 0; j <= n; j++ ) {
            if ( pgrCaret < 0 ) {
                // prev page right above screen
                qon_prevPage = QON_Max( i + 1, qon_prevPage );
                return;
            }

            int c = QON_GetPagerChar( i );
            int x = pgrCaret % conWidth;
            int y = pgrCaret / conWidth;

            if ( ! c ) {
                // prev page at the buffer start
                qon_prevPage = QON_Max( i + 1, qon_prevPage );
                QON_DrawChar( '~', 0, y, 0, drawCharParam );
            } else if ( c == '\n' ) {
#ifdef QON_DEBUG
                // debug draw the new lines
                QON_DrawChar( '0' + ( i % 10 ), x, y, 0, drawCharParam );
#endif
            } else {
                QON_DrawChar( c, x, y, 0, drawCharParam );
                pgrCaret--;
            } 

            i--;
        }
    }
}

#endif // QONCHE_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // QONCHE_H
