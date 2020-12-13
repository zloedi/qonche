#define QONCHE_IMPLEMENTATION
#define QON_DEBUG
#define QON_DrawChar DrawChar
#include "../qonche.h"

#include <SDL.h>

#define APPLEIIF_WIDTH 96
#define APPLEIIF_HEIGHT 64
#define APPLEIIF_CLMS 16
#define APPLEIIF_ROWS 8
#define APPLEIIF_CW (APPLEIIF_WIDTH/APPLEIIF_CLMS)
#define APPLEIIF_CH (APPLEIIF_HEIGHT/APPLEIIF_ROWS)

const unsigned char font[APPLEIIF_WIDTH * APPLEIIF_HEIGHT / 8] = {
    0x70,0x8f,0x1c,0xf3,0xef,0x9e,0x89,0xc0,0xa2,0x82,0x28,0x9c,
    0x89,0x48,0xa2,0x8a,0x08,0x20,0x88,0x80,0xa4,0x83,0x68,0xa2,
    0xaa,0x28,0xa0,0x8a,0x08,0x20,0x88,0x80,0xa8,0x82,0xac,0xa2,
    0xba,0x2f,0x20,0x8b,0xcf,0x20,0xf8,0x80,0xb0,0x82,0xaa,0xa2,
    0xb3,0xe8,0xa0,0x8a,0x08,0x26,0x88,0x80,0xa8,0x82,0x29,0xa2,
    0x82,0x28,0xa2,0x8a,0x08,0x22,0x88,0x88,0xa4,0x82,0x28,0xa2,
    0x7a,0x2f,0x1c,0xf3,0xe8,0x1e,0x89,0xc7,0x22,0xfa,0x28,0x9c,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0xf1,0xcf,0x1c,0xfa,0x28,0xa2,0x8a,0x2f,0xbe,0x03,0xe0,0x00,
    0x8a,0x28,0xa2,0x22,0x28,0xa2,0x8a,0x20,0xb0,0x80,0x60,0x00,
    0x8a,0x28,0xa0,0x22,0x28,0xa2,0x51,0x41,0x30,0x40,0x62,0x00,
    0xf2,0x2f,0x1c,0x22,0x28,0xaa,0x20,0x82,0x30,0x20,0x65,0x00,
    0x82,0xaa,0x02,0x22,0x28,0xaa,0x50,0x84,0x30,0x10,0x68,0x80,
    0x82,0x49,0x22,0x22,0x25,0x36,0x88,0x88,0x30,0x08,0x60,0x00,
    0x81,0xa8,0x9c,0x21,0xc2,0x22,0x88,0x8f,0xbe,0x03,0xe0,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,
    0x00,0x85,0x14,0x23,0x04,0x08,0x20,0x82,0x00,0x00,0x00,0x00,
    0x00,0x85,0x14,0x7b,0x2a,0x08,0x40,0x4a,0x88,0x00,0x00,0x02,
    0x00,0x85,0x3e,0xa0,0x4a,0x08,0x80,0x27,0x08,0x00,0x00,0x04,
    0x00,0x80,0x14,0x70,0x84,0x00,0x80,0x22,0x3e,0x03,0xe0,0x08,
    0x00,0x80,0x3e,0x29,0x0a,0x80,0x80,0x27,0x08,0x20,0x00,0x10,
    0x00,0x00,0x14,0xf2,0x69,0x00,0x40,0x4a,0x88,0x20,0x00,0x20,
    0x00,0x80,0x14,0x20,0x66,0x80,0x20,0x82,0x00,0x40,0x02,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x70,0x87,0x3e,0x13,0xe3,0xbe,0x71,0xc0,0x00,0x10,0x04,0x1c,
    0x89,0x88,0x82,0x32,0x04,0x02,0x8a,0x20,0x00,0x20,0x02,0x22,
    0x98,0x80,0x84,0x53,0xc8,0x04,0x8a,0x22,0x08,0x43,0xe1,0x04,
    0xa8,0x83,0x0c,0x90,0x2f,0x08,0x71,0xe0,0x00,0x80,0x00,0x88,
    0xc8,0x84,0x02,0xf8,0x28,0x90,0x88,0x22,0x08,0x43,0xe1,0x08,
    0x88,0x88,0x22,0x12,0x28,0x90,0x88,0x40,0x08,0x20,0x02,0x00,
    0x71,0xcf,0x9c,0x11,0xc7,0x10,0x73,0x80,0x10,0x10,0x04,0x08,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x70,0x8f,0x1c,0xf3,0xef,0x9e,0x89,0xc0,0xa2,0x82,0x28,0x9c,
    0x89,0x48,0xa2,0x8a,0x08,0x20,0x88,0x80,0xa4,0x83,0x68,0xa2,
    0xaa,0x28,0xa0,0x8a,0x08,0x20,0x88,0x80,0xa8,0x82,0xac,0xa2,
    0xba,0x2f,0x20,0x8b,0xcf,0x20,0xf8,0x80,0xb0,0x82,0xaa,0xa2,
    0xb3,0xe8,0xa0,0x8a,0x08,0x26,0x88,0x80,0xa8,0x82,0x29,0xa2,
    0x82,0x28,0xa2,0x8a,0x08,0x22,0x88,0x88,0xa4,0x82,0x28,0xa2,
    0x7a,0x2f,0x1c,0xf3,0xe8,0x1e,0x89,0xc7,0x22,0xfa,0x28,0x9c,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0xf1,0xcf,0x1c,0xfa,0x28,0xa2,0x8a,0x2f,0xbe,0x03,0xe0,0x00,
    0x8a,0x28,0xa2,0x22,0x28,0xa2,0x8a,0x20,0xb0,0x80,0x60,0x00,
    0x8a,0x28,0xa0,0x22,0x28,0xa2,0x51,0x41,0x30,0x40,0x62,0x00,
    0xf2,0x2f,0x1c,0x22,0x28,0xaa,0x20,0x82,0x30,0x20,0x65,0x00,
    0x82,0xaa,0x02,0x22,0x28,0xaa,0x50,0x84,0x30,0x10,0x68,0x80,
    0x82,0x49,0x22,0x22,0x25,0x36,0x88,0x88,0x30,0x08,0x60,0x00,
    0x81,0xa8,0x9c,0x21,0xc2,0x22,0x88,0x8f,0xbe,0x03,0xe0,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,
    0x40,0x08,0x00,0x08,0x03,0x00,0x80,0x81,0x20,0x60,0x00,0x00,
    0x20,0x08,0x00,0x08,0x04,0x80,0x80,0x00,0x20,0x20,0x00,0x00,
    0x11,0xcf,0x1e,0x79,0xc4,0x1c,0xf1,0x83,0x22,0x23,0x6f,0x1c,
    0x00,0x28,0xa0,0x8a,0x2f,0x22,0x88,0x81,0x24,0x22,0xa8,0xa2,
    0x01,0xe8,0xa0,0x8b,0xe4,0x22,0x88,0x81,0x38,0x22,0xa8,0xa2,
    0x02,0x28,0xa0,0x8a,0x04,0x1e,0x88,0x81,0x24,0x22,0xa8,0xa2,
    0x01,0xef,0x1e,0x79,0xe4,0x02,0x89,0xc9,0x22,0x72,0x28,0x9c,
    0x00,0x00,0x00,0x00,0x00,0x1c,0x00,0x06,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x0e,0x23,0x86,0x80,
    0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x18,0x20,0xcb,0x2a,
    0xf1,0xeb,0x9e,0xf2,0x28,0xa2,0x8a,0x2f,0x98,0x20,0xc0,0x14,
    0x8a,0x2c,0x20,0x42,0x28,0xa2,0x52,0x21,0x30,0x20,0x60,0x2a,
    0x8a,0x28,0x1c,0x42,0x28,0xaa,0x22,0x22,0x18,0x20,0xc0,0x14,
    0xf1,0xe8,0x02,0x4a,0x65,0x2a,0x51,0xe4,0x18,0x20,0xc0,0x2a,
    0x80,0x28,0x3c,0x31,0xa2,0x36,0x88,0x2f,0x8e,0x23,0x80,0x00,
    0x80,0x20,0x00,0x00,0x00,0x00,0x01,0xc0,0x00,0x20,0x00,0x00,
};

static inline SDL_Texture* CreateTexture( SDL_Renderer *renderer ) {
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" );
    SDL_Texture *tex = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ABGR8888, 
                    SDL_TEXTUREACCESS_STATIC, APPLEIIF_WIDTH, APPLEIIF_HEIGHT );
    int pitch = APPLEIIF_WIDTH * 4;
    int bw = APPLEIIF_WIDTH / 8;
    unsigned char bytes[pitch * APPLEIIF_HEIGHT];
    for ( int y = 0, idx = 0; y < APPLEIIF_HEIGHT; y++ ) {
        for ( int x = 0; x < bw; x++ ) {
            int byte = font[x + y * bw];
            for ( int i = 0; i < 8; i++, idx += 4 ) {
                int alpha = ( byte & ( 1 << ( 7 - i ) ) ) ? 0xff : 0;
                bytes[idx + 0] = 0xff;
                bytes[idx + 1] = 0xff;
                bytes[idx + 2] = 0xff;
                bytes[idx + 3] = alpha;
            }
        }
    }
}
    
void DrawChar( int c, int x, int y, int isUnderCursor, void *data ) {
}

int main( int argc, char *argv[] ) {
#if 0
    for ( int y = 0; y < APPLEIIF_HEIGHT; y++ ) {
        for ( int x = 0; x < APPLEIIF_WIDTH; x += 8 ) {
            int c = 0;
            for ( int i = 0; i < 8; i++ ) {
                int bit = appleii_font[x + i + y * APPLEIIF_WIDTH] ? 1 : 0;
                c |= bit << ( 7 - i );
            }
            printf( "0x%.2x,", c );
        }
        printf( "\n" );
    }
#endif
    return 0;
}
