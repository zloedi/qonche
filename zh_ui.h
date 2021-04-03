// Copyright (c) 2021 Stoiko Todorov
// This work is licensed under the terms of the MIT license.  
// For a copy, see https://opensource.org/licenses/MIT.

#ifndef ZH_UI_H
#define ZH_UI_H

typedef enum {
    UIBR_IDLE,
    UIBR_HOT,
    UIBR_ACTIVE,
    UIBR_RELEASED,
    UIBR_NUM_RESULTS,
} uiWidgetResult_t;

// == user drawing routines == 

// TODO: redefine these with user implementations

// void UI_DrawFilledRect( int x, int y, int w, int h );
// void UI_DrawString( int x, int y, const char *str );
// void UI_MeasureString( const char *str, int *outW, int *outH );

// == core == 

int ZH_UI_OnMouseButton( int down );
void ZH_UI_Begin( int cursorX, int cursorY );
void ZH_UI_End( void );

#ifndef __COUNTER__ 
#error "Requires a compiler with a __COUNTER__ built-in macro"
#endif

#ifndef FILE_UID
#warning "A unique identifier per include should be defined. Defaulting to 0"
#define FILE_UID 0
#endif

// zero is reserved handle, thus COUNTER + 1
#define WG_GEN_ID(handle) (((handle & 0xffff)<<16)|(((__COUNTER__+1)&0xff)<<8)|(FILE_UID&0xff))
#define WG_DEFAULT_ID WG_GEN_ID(0xffff)

// == widgets == 

// use the corresponding macros for 'auto-generated' handles

uiWidgetResult_t ZH_UI_ClickRect_wg( int x, int y, int w, int h, int handle );

#define ZH_UI_ClickRect(x,y,w,h) ZH_UI_ClickRect_wg(x,y,w,h,WG_DEFAULT_ID )

#ifdef ZH_UI_IMLEMENTATION

#ifndef ZH_UI_MAX_RECTS
#define ZH_UI_MAX_RECTS 256
#endif

typedef enum {
    UIMBS_NONE,
    UIMBS_DOWN = 1 << 0,
    UIMBS_UP   = 1 << 1,
} uiMouseButtonState;

typedef struct {
    int x, y, w, h;
    int handle;
} uiRect_t;

static int ui_numRects;
static uiRect_t ui_rects[ZH_UI_MAX_RECTS];

static uiMouseButtonState ui_mouseButton;

// this widget is hovered over
static int ui_hotWidget;
// this widget is usually pressed but not released yet
static int ui_activeWidget;

static int ui_oldCursorX, ui_cursorX;
static int ui_oldCursorY, ui_cursorY;

static inline int UI_CursorInRect( int x, int y, int w, int h ) {
    return ui_cursorX >= x && ui_cursorY >= y 
        && ui_cursorX < x + w && ui_cursorY < y + h;
}

static void UI_DragPosition( uiWidgetResult_t res, int *ioX, int *ioY ) {
    if ( res == UIBR_ACTIVE ) {
        int dx = ui_cursorX - ui_oldCursorX;
        int dy = ui_cursorY - ui_oldCursorY;
        *ioX += dx;
        *ioY += dy;
    }
}

// this callback might be called multiple times inside a single frame
int ZH_UI_OnMouseButton( int down ) {
    if ( down ) {
        ui_mouseButton |= UIMBS_DOWN;
    } else {
        ui_mouseButton |= UIMBS_UP;
    }
    return ( ui_hotWidget || ui_activeWidget );
}

void ZH_UI_Begin( int cursorX, int cursorY ) {
    ui_cursorX = cursorX;
    ui_cursorY = cursorY;
}

void ZH_UI_End( void ) {

#define BUTTON_RELEASED() ((ui_mouseButton & UIMBS_UP)||ui_mouseButton==UIMBS_NONE)

    // check if 'active' widget is still present on screen

    if ( ui_activeWidget ) {
        int i;
        for ( i = 0; i < ui_numRects; i++ ) {
            if ( ui_rects[i].handle == ui_activeWidget ) {
                break;
            }
        }

        // 'active' is no longer drawn/tested
        if ( i == ui_numRects ) {
            ui_activeWidget = 0;
        }
    }

    // 'hot' widgets are obtained each frame
    ui_hotWidget = 0;
    
    // go back-to-front in the rectangles stack and check for hits
    for ( int i = ui_numRects - 1; i >= 0; i-- ) {
        uiRect_t *r = &ui_rects[i];

        if ( UI_CursorInRect( r->x, r->y, r->w, r->h ) ) {
            if ( BUTTON_RELEASED() ) {
                // there is only one hot widget
                ui_activeWidget = 0;
                ui_hotWidget = r->handle;
            } else if ( ! ui_activeWidget ) {
                ui_activeWidget = r->handle;
            }

            break;
        } 
    }

    if ( BUTTON_RELEASED() ) {
        ui_activeWidget = 0;
    }

    // clear rects stack
    ui_numRects = 0;

    // handle mouse buttons specially
    if ( ui_mouseButton & UIMBS_UP ) {
        ui_mouseButton = UIMBS_NONE;
    }

    ui_oldCursorX = ui_cursorX;
    ui_oldCursorY = ui_cursorY;

#undef BUTTON_RELEASED

}

uiWidgetResult_t ZH_UI_ClickRect_wg( int x, int y, int w, int h, int handle ) {
    uiWidgetResult_t res = UIBR_IDLE;

    // push rectangle in the list

    // stop checking widgets if too many
    if ( ui_numRects == ZH_UI_MAX_RECTS ) {
        return res;
    }

    ui_rects[ui_numRects] = ( uiRect_t ) { x, y, w, h, handle, };
    ui_numRects++;

    // click

    if ( ui_activeWidget == handle ) {
        if ( ( ui_mouseButton & UIMBS_UP ) && UI_CursorInRect( x, y, w, h ) ) {
            res = UIBR_RELEASED;
        } else {
            res = UIBR_ACTIVE;
        }
    } else if ( handle == ui_hotWidget ) {
        res = UIBR_HOT;
    }

    return res;
}

void ZH_UI_Drag_wg( int *ioX, int *ioY, int w, int h, int handle ) {
    uiWidgetResult_t res = ZH_UI_ClickRect_wg( *ioX, *ioY, w, h, handle );
    UI_DragPosition( res, ioX, ioY );
}

#endif // ZH_UI_IMLEMENTATION

#endif // ZH_UI_H
