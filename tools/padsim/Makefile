# i686 MinGW cross build (MFL.exe is a 32 bit process), as tools/d3dtrace
CC = i686-w64-mingw32-gcc -O2 -Wall -Wno-unused -Wno-format -static-libgcc -s
padsim8.dll: padsim.c
	$(CC) -shared -o $@ $< -Wl,--kill-at -ldxguid
# the self-check gets the proxy the same way as the game: its dinput8.dll import is renamed
test: padsim_test.exe
padsim_test.exe: padsim_test.c padsim8.dll patch_import.py
	$(CC) -o $@ $< -ldinput8 -ldxguid
	python3 patch_import.py $@
