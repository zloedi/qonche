// Copyright (c) 2021 Stoiko Todorov
// This work is licensed under the terms of the MIT license.  
// For a copy, see https://opensource.org/licenses/MIT.

#define QON_CUSTOM_DRAW_CALLBACKS
#define QON_USE_PAGER_CALLBACKS
#define QON_DrawChar DrawChar
#define QONCHE_IMPLEMENTATION
#include "qonche.h"

#include <stdio.h>
#include "SDL.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define FILE_UID 0
#define ZH_UI_IMLEMENTATION
#include "zh_ui.h"
#include "zh_apple_font.h"

#define CON_X 10 
#define CON_Y 10
#define CON_SCALE_X 2 
#define CON_SCALE_Y 2
#define CON_SPACE_X 1 
#define CON_SPACE_Y 3

static SDL_Renderer *x_renderer;
static SDL_Window *x_window;
static SDL_Texture *x_fontTex;
static unsigned char x_colorization[4];
static int x_mouseX, x_mouseY;
    
void DrawCharXY( int c, int x, int y, int w, int h ) {
    int idx = c & ( APPLEIIF_ROWS * APPLEIIF_CLMS - 1 );
    SDL_SetTextureAlphaMod( x_fontTex, 0xff );
    SDL_SetTextureBlendMode( x_fontTex, SDL_BLENDMODE_BLEND );
    SDL_Rect src = {
        idx % APPLEIIF_CLMS * APPLEIIF_CW,
        idx / APPLEIIF_CLMS * APPLEIIF_CH,
        APPLEIIF_CW,
        APPLEIIF_CH,
    };
    SDL_SetTextureColorMod( x_fontTex, 0xff, 0xff, 0xff );
    SDL_Rect dst = { x, y, w, h };
    SDL_RenderCopy( x_renderer, x_fontTex, &src, &dst );
}

void DrawChar( int c, int qonX, int qonY, int isUnderCursor, void *data ) {
    ( void )data;
    int blink = SDL_GetTicks() & 256;
    int trueChar = ( isUnderCursor && blink ) ? 127 : c;
    int x = CON_X + qonX * ( APPLEIIF_CW + CON_SPACE_X ) * CON_SCALE_X;
    int y = CON_Y + qonY * ( APPLEIIF_CH + CON_SPACE_Y ) * CON_SCALE_Y;
    int w = APPLEIIF_CW * CON_SCALE_X;
    int h = APPLEIIF_CH * CON_SCALE_Y;
    DrawCharXY( trueChar, x, y, w, h );
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
                        QON_EmitCommand( sizeof( buf ), buf );
                    }
                    break;
                    default: break;
                }
                break;

			case SDL_MOUSEMOTION:
				x_mouseX = event.motion.x;
                x_mouseY = event.motion.y;
				break;

            case SDL_MOUSEBUTTONDOWN:
                ZH_UI_OnMouseButton( 1 );
                break;

            case SDL_MOUSEBUTTONUP:
                ZH_UI_OnMouseButton( 0 );
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
    int cellW = CON_SCALE_X * ( APPLEIIF_CW + CON_SPACE_X );
    int cellH = CON_SCALE_Y * ( APPLEIIF_CH + CON_SPACE_Y );
    int w, h;
    SDL_GetWindowSize( x_window, &w, &h );
    QON_Draw( ( w - CON_X * 2 ) / cellW, ( h - CON_Y * 2 ) / cellH );
    ZH_UI_Begin( x_mouseX, x_mouseY );
    ZH_UI_End();
    SDL_RenderPresent( x_renderer );
    SDL_Delay( 10 );
}

static inline int Clamp( int a, int min, int max ) {
    int t = a < min ? min : a;
    return t > max ? max : t;
}

static void ColorPicker_f( int qonX, int qonY, void *param ) {
    ( void )param;
    static SDL_Texture *texSV;
    static SDL_Texture *texHue;

    static const unsigned char hue[7 * 4] = {
        0xff,0x00,0x00,0xff, // red
        0xff,0x00,0xff,0xff, // magenta
        0x00,0x00,0xff,0xff, // blue
        0x00,0xff,0xff,0xff, // cyan
        0x00,0xff,0x00,0xff, // green
        0xff,0xff,0x00,0xff, // yellow
        0xff,0x00,0x00,0xff, // red
    };

    static unsigned char sv[2 * 2 * 4] = {
        0xff,0xff,0xff,0xff,   0xff,0x00,0x00,0xff,
        0x00,0x00,0x00,0xff,   0x00,0x00,0x00,0xff,
    };
    
    // == init == 

    if ( ! texSV ) {
        // lerp quality
        SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
        texSV = SDL_CreateTexture( x_renderer, SDL_PIXELFORMAT_ABGR8888, 
                                            SDL_TEXTUREACCESS_STREAMING, 2, 2 );
        SDL_UpdateTexture( texSV, NULL, sv, 2 * 4 );
        texHue = SDL_CreateTexture( x_renderer, SDL_PIXELFORMAT_ABGR8888, 
                                            SDL_TEXTUREACCESS_STATIC, 1, 7 );
        SDL_UpdateTexture( texHue, NULL, hue, 4 );
    }

    // == draw widgets ==

    int svX = CON_X + qonX * ( APPLEIIF_CW + CON_SPACE_X ) * CON_SCALE_X; 
    int svY = CON_Y + qonY * ( APPLEIIF_CH + CON_SPACE_Y ) * CON_SCALE_Y; 
    int svW = 200;
    int svH = svW;
    uiWidgetResult_t uiSat;

    int hueX = svX + svW + 20;
    int hueY = svY;
    int hueW = 25;
    int hueH = svH;
    uiWidgetResult_t uiHue;

    {
        // in Opengl you just tweak the UV-s to match the lerped region
        // however SDL doesn't allow floating point/subpixel source rectangles
        // so we resolve to clip rect ugliness

        // saturation/value
        SDL_SetTextureBlendMode( texSV, SDL_BLENDMODE_NONE );
        SDL_SetTextureColorMod( texSV, 0xff, 0xff, 0xff );
        SDL_Rect clip = { svX, svY, svW, svH };
        SDL_RenderSetClipRect( x_renderer, &clip );
        SDL_Rect dst = { svX - svW / 2, svY - svH / 2, 2 * svW, 2 * svH };
        SDL_RenderCopy( x_renderer, texSV, NULL, &dst );
        uiSat = ZH_UI_ClickRect( svX, svY, svW, svH );

        // hue
        clip = ( SDL_Rect ){ hueX, hueY, hueW, hueH };
        SDL_RenderSetClipRect( x_renderer, &clip );
        dst = ( SDL_Rect ){ hueX, hueY - hueH / 14, hueW, hueH + hueH / 7 };
        SDL_RenderCopy( x_renderer, texHue, NULL, &dst );
        uiHue = ZH_UI_ClickRect( hueX - 8, svY - 8, hueW + 16, svH + 16 );

        SDL_RenderSetClipRect( x_renderer, NULL );
    }

    // == hue cursor ==

    {
        static int y;
        if ( uiHue == UIBR_ACTIVE ) {
            y = Clamp( x_mouseY - hueY, 0, hueH );
            void *pixels;
            int pitch;
            SDL_LockTexture( texSV, NULL, &pixels, &pitch );
            int seg = hueH / 6;
            int t = ( ( y % seg ) << 8 ) / seg;
            int i = y / seg;
            const unsigned char *c0 = &hue[( ( i + 0 ) % 7 ) * 4];
            const unsigned char *c1 = &hue[( ( i + 1 ) % 7 ) * 4];
            unsigned char *p = pixels;
            sv[4 + 0] = ( c1[0] * t + c0[0] * ( 256 - t ) ) >> 8;
            sv[4 + 1] = ( c1[1] * t + c0[1] * ( 256 - t ) ) >> 8;
            sv[4 + 2] = ( c1[2] * t + c0[2] * ( 256 - t ) ) >> 8;
            memcpy( p, sv, sizeof( sv ) );
            SDL_UnlockTexture( texSV );
        }
        int drawY = hueY + y;
        int w = hueW + 8;
        int h = 2 * APPLEIIF_CH;
        DrawCharXY( '=', hueX - 2, drawY - h / 2 + 1, w, h );
    }

    // == saturation/value cursor ==

    {
        static int x = 99999, y;
        if ( ! x_colorization[3] || uiHue == UIBR_ACTIVE || uiSat == UIBR_ACTIVE ) {
            if ( uiSat == UIBR_ACTIVE ) {
                x = Clamp( x_mouseX - svX, 0, svW );
                y = Clamp( x_mouseY - svY, 0, svH );
            } else {
                x = Clamp( x, 0, svW );
                y = Clamp( y, 0, svH );
            }

            int s = ( x << 8 ) / svW;
            int v = ( y << 8 ) / svH;
            int is = 256 - s;
            int iv = 256 - v;

            int c0r = ( is * iv * sv[ 0 + 0] ) >> 16;
            int c0g = ( is * iv * sv[ 0 + 1] ) >> 16;
            int c0b = ( is * iv * sv[ 0 + 2] ) >> 16;

            int c1r = (  s * iv * sv[ 4 + 0] ) >> 16;
            int c1g = (  s * iv * sv[ 4 + 1] ) >> 16;
            int c1b = (  s * iv * sv[ 4 + 2] ) >> 16;

            int c2r = ( is *  v * sv[ 8 + 0] ) >> 16;
            int c2g = ( is *  v * sv[ 8 + 1] ) >> 16;
            int c2b = ( is *  v * sv[ 8 + 2] ) >> 16;

            int c3r = (  s *  v * sv[12 + 0] ) >> 16;
            int c3g = (  s *  v * sv[12 + 1] ) >> 16;
            int c3b = (  s *  v * sv[12 + 2] ) >> 16;

            x_colorization[0] = c0r + c1r + c2r + c3r;
            x_colorization[1] = c0g + c1g + c2g + c3g;
            x_colorization[2] = c0b + c1b + c2b + c3b;
            x_colorization[3] = 0xff;
        }
        int drawX = svX + x;
        int drawY = svY + y;
        int w = 2 * APPLEIIF_CW;
        int h = 2 * APPLEIIF_CH;
        DrawCharXY( 'o', drawX - w / 2 + 1, drawY - h / 2 - 1, w, h );
    }
}

static void DrawFontAtlas_f( int x, int y, void *param ) {
    ( void )param;
    SDL_SetTextureAlphaMod( x_fontTex, 0xff );
    SDL_SetTextureBlendMode( x_fontTex, SDL_BLENDMODE_BLEND );
    SDL_SetTextureColorMod( x_fontTex, x_colorization[0], 
                                        x_colorization[1], 
                                        x_colorization[2] ); 
    int h = APPLEIIF_HEIGHT * CON_SCALE_Y;
    SDL_Rect dst = { 
        CON_X + x * ( APPLEIIF_CW + CON_SPACE_X ) * CON_SCALE_X, 
        CON_Y + y * ( APPLEIIF_CH + CON_SPACE_Y ) * CON_SCALE_Y, 
        APPLEIIF_WIDTH * CON_SCALE_X,
        h,
    };
    SDL_RenderCopy( x_renderer, x_fontTex, NULL, &dst );
}

int main( int argc, char *argv[] ) {
    // shut up gcc
    (void)argc;
    (void)argv;

    SDL_Init( SDL_INIT_VIDEO );
    int flags;
#ifdef __EMSCRIPTEN__
    flags = 0;
    SDL_CreateWindowAndRenderer( 640, 768, flags, &x_window, &x_renderer );
#else
    flags = SDL_WINDOW_RESIZABLE;
    SDL_CreateWindowAndRenderer( 1024, 768, flags, &x_window, &x_renderer );
#endif
    x_fontTex = ZH_APLF_CreateTexture( x_renderer );
      
    QON_Print( "\n" );
    QON_Print( "                  /   \\     \n" );
    QON_Print( "                 |     |     \n" );
    QON_Print( "                  \\_T_/ onche\n" );
    QON_Print( "                    |        \n" );
    QON_Print( "\n" );
    QON_Print( "The 'GUI' demo extends the console a bit...\n\n" );
    QON_Print( "You can draw pictures inside the log:\n" );
    QON_PrintWithCallback( DrawFontAtlas_f, NULL, "\n\n\n\n\n\n" );
    QON_Print( "Place them next to each other too:\n" );
    QON_PrintWithCallback( DrawFontAtlas_f, NULL, "                       " );
    QON_PrintWithCallback( DrawFontAtlas_f, NULL, "\n\n\n\n\n\n" );
    QON_Print( "You can even embed GUI stuff:\n" );
    QON_PrintWithCallback( ColorPicker_f, NULL, "\n\n\n\n\n\n\n\n\n\n" );

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
