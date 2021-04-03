#define QON_CUSTOM_DRAW_CALLBACKS
#define QON_USE_PAGER_CALLBACKS
#define QON_DrawChar DrawChar
#define QONCHE_IMPLEMENTATION
#include "qonche.h"

#define FILE_UID 0
#define ZH_UI_IMLEMENTATION
#include "zh_ui.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define CON_X 10 
#define CON_Y 10
#define CON_SCALE_X 2 
#define CON_SCALE_Y 2
#define CON_SPACE_X 1 
#define CON_SPACE_Y 3

// apple II font

#define APPLEIIF_WIDTH 96
#define APPLEIIF_HEIGHT 64
#define APPLEIIF_CLMS 16
#define APPLEIIF_ROWS 8
#define APPLEIIF_CW (APPLEIIF_WIDTH/APPLEIIF_CLMS)
#define APPLEIIF_CH (APPLEIIF_HEIGHT/APPLEIIF_ROWS)

unsigned char x_font[APPLEIIF_WIDTH * APPLEIIF_HEIGHT / 8] = {
0x0e,0xf1,0x38,0xcf,0xf7,0x79,0x91,0x03,0x45,0x41,0x14,0x39,0x91,0x12,0x45,0x51,
0x10,0x04,0x11,0x01,0x25,0xc1,0x16,0x45,0x55,0x14,0x05,0x51,0x10,0x04,0x11,0x01,
0x15,0x41,0x35,0x45,0x5d,0xf4,0x04,0xd1,0xf3,0x04,0x1f,0x01,0x0d,0x41,0x55,0x45,
0xcd,0x17,0x05,0x51,0x10,0x64,0x11,0x01,0x15,0x41,0x94,0x45,0x41,0x14,0x45,0x51,
0x10,0x44,0x11,0x11,0x25,0x41,0x14,0x45,0x5e,0xf4,0x38,0xcf,0x17,0x78,0x91,0xe3,
0x44,0x5f,0x14,0x39,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x8f,0xf3,0x38,0x5f,0x14,0x45,0x51,0xf4,0x7d,0xc0,0x07,0x00,0x51,0x14,0x45,0x44,
0x14,0x45,0x51,0x04,0x0d,0x01,0x06,0x00,0x51,0x14,0x05,0x44,0x14,0x45,0x8a,0x82,
0x0c,0x02,0x46,0x00,0x4f,0xf4,0x38,0x44,0x14,0x55,0x04,0x41,0x0c,0x04,0xa6,0x00,
0x41,0x55,0x40,0x44,0x14,0x55,0x0a,0x21,0x0c,0x08,0x16,0x01,0x41,0x92,0x44,0x44,
0xa4,0x6c,0x11,0x11,0x0c,0x10,0x06,0x00,0x81,0x15,0x39,0x84,0x43,0x44,0x11,0xf1,
0x7d,0xc0,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,
0x00,0xa1,0x28,0xc4,0x20,0x10,0x04,0x41,0x00,0x00,0x00,0x00,0x00,0xa1,0x28,0xde,
0x54,0x10,0x02,0x52,0x11,0x00,0x00,0x40,0x00,0xa1,0x7c,0x05,0x52,0x10,0x01,0xe4,
0x10,0x00,0x00,0x20,0x00,0x01,0x28,0x0e,0x21,0x00,0x01,0x44,0x7c,0xc0,0x07,0x10,
0x00,0x01,0x7c,0x94,0x50,0x01,0x01,0xe4,0x10,0x04,0x00,0x08,0x00,0x00,0x28,0x4f,
0x96,0x00,0x02,0x52,0x11,0x04,0x00,0x04,0x00,0x01,0x28,0x04,0x66,0x01,0x04,0x41,
0x00,0x02,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0e,0xe1,0x7c,0xc8,0xc7,0x7d,0x8e,0x03,0x00,0x08,0x20,0x38,0x91,0x11,0x41,0x4c,
0x20,0x40,0x51,0x04,0x00,0x04,0x40,0x44,0x19,0x01,0x21,0xca,0x13,0x20,0x51,0x44,
0x10,0xc2,0x87,0x20,0x15,0xc1,0x30,0x09,0xf4,0x10,0x8e,0x07,0x00,0x01,0x00,0x11,
0x13,0x21,0x40,0x1f,0x14,0x09,0x11,0x44,0x10,0xc2,0x87,0x10,0x11,0x11,0x44,0x48,
0x14,0x09,0x11,0x02,0x10,0x04,0x40,0x00,0x8e,0xf3,0x39,0x88,0xe3,0x08,0xce,0x01,
0x08,0x08,0x20,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0e,0xf1,0x38,0xcf,0xf7,0x79,0x91,0x03,0x45,0x41,0x14,0x39,0x91,0x12,0x45,0x51,
0x10,0x04,0x11,0x01,0x25,0xc1,0x16,0x45,0x55,0x14,0x05,0x51,0x10,0x04,0x11,0x01,
0x15,0x41,0x35,0x45,0x5d,0xf4,0x04,0xd1,0xf3,0x04,0x1f,0x01,0x0d,0x41,0x55,0x45,
0xcd,0x17,0x05,0x51,0x10,0x64,0x11,0x01,0x15,0x41,0x94,0x45,0x41,0x14,0x45,0x51,
0x10,0x44,0x11,0x11,0x25,0x41,0x14,0x45,0x5e,0xf4,0x38,0xcf,0x17,0x78,0x91,0xe3,
0x44,0x5f,0x14,0x39,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x8f,0xf3,0x38,0x5f,0x14,0x45,0x51,0xf4,0x7d,0xc0,0x07,0x00,0x51,0x14,0x45,0x44,
0x14,0x45,0x51,0x04,0x0d,0x01,0x06,0x00,0x51,0x14,0x05,0x44,0x14,0x45,0x8a,0x82,
0x0c,0x02,0x46,0x00,0x4f,0xf4,0x38,0x44,0x14,0x55,0x04,0x41,0x0c,0x04,0xa6,0x00,
0x41,0x55,0x40,0x44,0x14,0x55,0x0a,0x21,0x0c,0x08,0x16,0x01,0x41,0x92,0x44,0x44,
0xa4,0x6c,0x11,0x11,0x0c,0x10,0x06,0x00,0x81,0x15,0x39,0x84,0x43,0x44,0x11,0xf1,
0x7d,0xc0,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfc,
0x02,0x10,0x00,0x10,0xc0,0x00,0x01,0x81,0x04,0x06,0x00,0x00,0x04,0x10,0x00,0x10,
0x20,0x01,0x01,0x00,0x04,0x04,0x00,0x00,0x88,0xf3,0x78,0x9e,0x23,0x38,0x8f,0xc1,
0x44,0xc4,0xf6,0x38,0x00,0x14,0x05,0x51,0xf4,0x44,0x11,0x81,0x24,0x44,0x15,0x45,
0x80,0x17,0x05,0xd1,0x27,0x44,0x11,0x81,0x1c,0x44,0x15,0x45,0x40,0x14,0x05,0x51,
0x20,0x78,0x11,0x81,0x24,0x44,0x15,0x45,0x80,0xf7,0x78,0x9e,0x27,0x40,0x91,0x93,
0x44,0x4e,0x14,0x39,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x60,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x70,0xc4,0x61,0x01,0x00,0x00,0x00,0x02,
0x00,0x00,0x00,0x00,0x18,0x04,0xd3,0x54,0x8f,0xd7,0x79,0x4f,0x14,0x45,0x51,0xf4,
0x19,0x04,0x03,0x28,0x51,0x34,0x04,0x42,0x14,0x45,0x4a,0x84,0x0c,0x04,0x06,0x54,
0x51,0x14,0x38,0x42,0x14,0x55,0x44,0x44,0x18,0x04,0x03,0x28,0x8f,0x17,0x40,0x52,
0xa6,0x54,0x8a,0x27,0x18,0x04,0x03,0x54,0x01,0x14,0x3c,0x8c,0x45,0x6c,0x11,0xf4,
0x71,0xc4,0x01,0x00,0x01,0x04,0x00,0x00,0x00,0x00,0x80,0x03,0x00,0x04,0x00,0x00,
};

SDL_Renderer *x_renderer;
SDL_Window *x_window;
SDL_Texture *x_fontTex;
unsigned char x_colorization[4];
int x_mouseX, x_mouseY;
    
static SDL_Texture* CreateFontTexture( void ) {
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
    SDL_Texture *tex = SDL_CreateTexture( x_renderer, SDL_PIXELFORMAT_ABGR8888, 
                    SDL_TEXTUREACCESS_STATIC, APPLEIIF_WIDTH, APPLEIIF_HEIGHT );
    int pitch = APPLEIIF_WIDTH * 4;
    int bw = APPLEIIF_WIDTH / 8;
    unsigned char bytes[pitch * APPLEIIF_HEIGHT];
    for ( int y = 0, idx = 0; y < APPLEIIF_HEIGHT; y++ ) {
        for ( int x = 0; x < bw; x++ ) {
            int byte = x_font[x + y * bw];
            for ( int i = 0; i < 8; i++, idx += 4 ) {
                int alpha = ( byte & ( 1 << i ) ) ? 0xff : 0;
                bytes[idx + 0] = 0xff;
                bytes[idx + 1] = 0xff;
                bytes[idx + 2] = 0xff;
                bytes[idx + 3] = alpha;
            }
        }
    }
    SDL_UpdateTexture( tex, NULL, bytes, pitch );
    return tex;
}

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
    if ( ZH_UI_ClickRect( 10, 10, 50, 20 ) == UIBR_RELEASED ) {
        QON_Print( "Clicked\n" );
    }
    ZH_UI_End();
    SDL_RenderPresent( x_renderer );
    SDL_Delay( 10 );
}

static inline int Clamp( int a, int min, int max ) {
    int result = a > min ? a : min;
    return result < max ? result : max;
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

    // FIXME: could use just one pixel (directly use the colorization), not a buffer
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
        x_colorization[0] = sv[4 + 0];
        x_colorization[1] = sv[4 + 1];
        x_colorization[2] = sv[4 + 2];
        x_colorization[3] = sv[4 + 3];
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
            SDL_Rect rect = { 1, 0, 1, 1 };
            SDL_LockTexture( texSV, &rect, &pixels, &pitch );
            int seg = hueH / 6;
            int t = ( ( y % seg ) << 8 ) / seg;
            int i = y / seg;
            const unsigned char *c0 = &hue[( ( i + 0 ) % 7 ) * 4];
            const unsigned char *c1 = &hue[( ( i + 1 ) % 7 ) * 4];
            unsigned char *p = pixels;
            p[0] = ( c1[0] * t + c0[0] * ( 256 - t ) ) >> 8;
            p[1] = ( c1[1] * t + c0[1] * ( 256 - t ) ) >> 8;
            p[2] = ( c1[2] * t + c0[2] * ( 256 - t ) ) >> 8;
            sv[4 + 0] = p[0];
            sv[4 + 1] = p[1];
            sv[4 + 2] = p[2];
            SDL_UnlockTexture( texSV );
        }
        int drawY = hueY + y;
        int w = 2 * hueW;
        int h = 2 * APPLEIIF_CH;
        DrawCharXY( '=', hueX - hueW / 3, drawY - h / 2 + 1, w, h );
    }

    // == saturation/value cursor ==

    {
        static int x = 99999, y;
        if ( uiSat == UIBR_ACTIVE ) {
            x = Clamp( x_mouseX - svX, 0, svW );
            y = Clamp( x_mouseY - svY, 0, svH );
        }
        if ( uiHue == UIBR_ACTIVE || uiSat == UIBR_ACTIVE ) {
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
        }
        int drawX = svX + Clamp( x, 0, svW );
        int drawY = svY + Clamp( y, 0, svH );
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
    SDL_CreateWindowAndRenderer( 700, 820, flags, &x_window, &x_renderer );
#endif
    x_fontTex = CreateFontTexture();
      
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
