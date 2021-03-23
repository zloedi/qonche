// enable this for tiny buffers and visible new lines
#if 0
#define QON_DEBUG
#endif
#define QONCHE_IMPLEMENTATION
#define QON_DrawChar DrawChar
#include "qonche.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

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

static SDL_Texture* CreateTexture( SDL_Renderer *renderer ) {
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
    SDL_Texture *tex = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ABGR8888, 
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
    QON_Draw( ( w - prm.x * 2 ) / cellW, ( h - prm.y * 2 ) / cellH, &prm );

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
    x_fontTex = CreateTexture( x_renderer );
      
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
