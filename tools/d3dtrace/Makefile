# i686 MinGW cross build of the proxy (MFL.exe is a 32 bit process)
d3d9.dll: d3d9.c
	i686-w64-mingw32-gcc -O2 -Wall -Wno-unused -Wno-format -shared -o $@ $< -Wl,--kill-at -static-libgcc -s
