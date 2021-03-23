demo.exe: demo.c qonche.h
	x86_64-w64-mingw32-gcc -o demo.exe demo.c -O0 -g -Wall -Wextra -Wshadow -mconsole -I/usr/local/x86_64-w64-mingw32/include -Dmain=SDL_main `sdl2-config --static-libs`

demo_emscripten: demo.c qonche.h
	emcc demo.c -s WASM=1 -s USE_SDL=2 -O3 -o index.js

demo_linux: demo.c qonche.h
	gcc -o demo demo.c -O0 -g -Wall -Wshadow `sdl2-config --cflags` `sdl2-config --static-libs`
