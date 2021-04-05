// Copyright (c) 2021 Stoiko Todorov
// This work is licensed under the terms of the MIT license.  
// For a copy, see https://opensource.org/licenses/MIT.

#define QONCHE_IMPLEMENTATION
#define QON_DrawChar DrawChar
#include "qonche.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "zh_apple_font.h"

typedef struct {
    int x, y;
    int scaleX, scaleY;
    int spaceX, spaceY;
} dcParam_t;

SDL_Renderer *x_renderer;
SDL_Window *x_window;
SDL_Texture *x_fontTex;
    
void DrawChar( int c, int x, int y, int isUnderCursor, void *data ) {
    dcParam_t *prm = data;
    int blink = SDL_GetTicks() & 256;
    int trueChar = ( isUnderCursor && blink ) ? 127 : c;
    int idx = trueChar & ( APPLEIIF_ROWS * APPLEIIF_CLMS - 1 );
    SDL_SetTextureAlphaMod( x_fontTex, 0xff );
    SDL_SetTextureBlendMode( x_fontTex, SDL_BLENDMODE_BLEND );
    SDL_Rect src = {
        idx % APPLEIIF_CLMS * APPLEIIF_CW,
        idx / APPLEIIF_CLMS * APPLEIIF_CH,
        APPLEIIF_CW,
        APPLEIIF_CH,
    };
    SDL_SetTextureColorMod( x_fontTex, 0xff, 0xff, 0xff );
    SDL_Rect dst = { 
        prm->x + x * ( APPLEIIF_CW + prm->spaceX ) * prm->scaleX, 
        prm->y + y * ( APPLEIIF_CH + prm->spaceY ) * prm->scaleY, 
        APPLEIIF_CW * prm->scaleX, 
        APPLEIIF_CH * prm->scaleY,
    };
    SDL_RenderCopy( x_renderer, x_fontTex, &src, &dst );
}

static inline const char* va( const char *fmt, ... ) {
    static char buf[1024];
    va_list argptr;
    va_start( argptr, fmt );
    vsnprintf( buf, 1024, fmt, argptr );
    va_end( argptr );
    return buf;
}

void MainLoop( void *arg ) {
    int *quit = arg;
    SDL_Event event;
    while ( SDL_PollEvent( &event ) ) {
        int code = event.key.keysym.sym;
        switch( event.type ) {
            case SDL_TEXTINPUT:
                QON_Insert( event.text.text );
                break;
            case SDL_KEYDOWN:
                switch ( code ) {
                    case SDLK_RIGHT:     QON_MoveRight( 1 ); break;
                    case SDLK_LEFT:      QON_MoveLeft( 1 );  break;
                    case SDLK_DELETE:    QON_Delete( 1 );    break;
                    case SDLK_BACKSPACE: QON_Backspace( 1 ); break;
                    case SDLK_PAGEUP:    QON_PageUp();       break;
                    case SDLK_PAGEDOWN:  QON_PageDown();     break;
                    case SDLK_RETURN: {
                        char buf[64];
                        const int max = 16;
                        QON_EmitCommand( sizeof( buf ), buf );
                        int len = strlen( buf );
                        buf[max] = 0;
                        QON_Print( va( "You typed '%s%s'\n", buf,
                                                    len > max 
                                                    ? " blah blah ..." 
                                                    : "" ) );
                    }
                    break;
                    default: break;
                }
                break;
            case SDL_QUIT:
                *quit = 1;
                break;
            default:
                break;
        }
    }

    SDL_SetRenderDrawColor( x_renderer, 40, 45, 50, 255 );
    SDL_RenderClear( x_renderer );

    // just to show param passing
    static dcParam_t prm = {
        .x = 10, .y = 10,
        .scaleX = 2, .scaleY = 2,
        .spaceX = 1, .spaceY = 3,
    };
    int cellW = prm.scaleX * ( APPLEIIF_CW + prm.spaceX );
    int cellH = prm.scaleY * ( APPLEIIF_CH + prm.spaceY );
    int w, h;
    SDL_GetWindowSize( x_window, &w, &h );
    QON_DrawEx( ( w - prm.x * 2 ) / cellW, ( h - prm.y * 2 ) / cellH, 0, &prm );

    SDL_RenderPresent( x_renderer );
    SDL_Delay( 10 );
}

int main( int argc, char *argv[] ) {
    // shut up gcc
    (void)argc;
    (void)argv;

    SDL_Init( SDL_INIT_VIDEO );
    int flags;
#ifdef __EMSCRIPTEN__
    flags = 0;
#else
    flags = SDL_WINDOW_RESIZABLE;
#endif
    SDL_CreateWindowAndRenderer( 640, 480, flags, &x_window, &x_renderer );
    x_fontTex = ZH_APLF_CreateTexture( x_renderer );
      
    QON_Print( "\n" );
    QON_Print( "                  /   \\     \n" );
    QON_Print( "                 |     |     \n" );
    QON_Print( "                  \\_T_/ onche\n" );
    QON_Print( "                    |        \n" );
    QON_Print( "\n" );
    QON_Print( "'qonche' is a quake style console in a\nsingle C header.\n" );
    QON_Print( "\n" );
    QON_Print( "Enter a command and watch it being printed\nto the log. "
                                                            "It's magic!..\n" );
    QON_Print( "\n" );

    int quit = 0;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg( MainLoop, &quit, -1, 1 );
#else
    SDL_SetWindowTitle( x_window, "qonche" );
    while ( 1 ) {
        MainLoop( &quit );
        if ( quit ) {
            break;
        }
    }
#endif

    SDL_DestroyRenderer( x_renderer );
    SDL_DestroyWindow( x_window );
    SDL_Quit();

    return 0;
}
