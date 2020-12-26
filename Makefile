demo_linux: demo.c
	gcc -o demo demo.c -O0 -g -Wall `sdl2-config --cflags` `sdl2-config --static-libs`

demo_mingw: demo.c
	x86_64-w64-mingw32-gcc -o demo.exe demo.c -O0 -g -Wall `sdl2-config --cflags` `sdl2-config --static-libs`
