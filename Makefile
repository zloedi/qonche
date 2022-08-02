demo_gui.exe: demo_gui.c zh_ui.h zh_apple_font.h qonche.h
	x86_64-w64-mingw32-gcc -o demo_gui.exe demo_gui.c -O0 -g -Wall -Wextra -Wshadow -mconsole -ISDL2-2.0.22/x86_64-w64-mingw32/include/SDL2 -Dmain=SDL_main -LSDL2-2.0.22/x86_64-w64-mingw32/lib `./SDL2-2.0.22/x86_64-w64-mingw32/bin/sdl2-config --static-libs`

demo.exe: demo.c zh_ui.h zh_apple_font.h qonche.h
	x86_64-w64-mingw32-gcc -o demo.exe demo.c -O0 -g -Wall -Wextra -Wshadow -mconsole -ISDL2-2.0.22/x86_64-w64-mingw32/include/SDL2 -Dmain=SDL_main -LSDL2-2.0.22/x86_64-w64-mingw32/lib `./SDL2-2.0.22/x86_64-w64-mingw32/bin/sdl2-config --static-libs`

demo_emscripten: demo.c qonche.h
	emcc demo.c -s WASM=1 -s USE_SDL=2 -O3 -o index.js

demo_linux: demo.c qonche.h
	gcc -o demo demo.c -O0 -g -Wall -Wshadow `sdl2-config --cflags` `sdl2-config --static-libs`

demo_gui_emscripten: demo_gui.c zh_ui.h qonche.h
	emcc demo_gui.c -s WASM=1 -s USE_SDL=2 -O3 -o index.js

demo_gui_linux: demo_gui.c zh_ui.h qonche.h
	gcc -o demo_gui demo_gui.c -O0 -g -Wall -Wshadow `sdl2-config --cflags` `sdl2-config --static-libs`
