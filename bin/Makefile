CC32=C:\\msys64\\mingw32\\bin\\gcc
CC64=C:\\msys64\\mingw64\\bin\\gcc
WIND32=C:\\msys64\\mingw32\\bin\\windres
WIND64=C:\\msys64\\mingw64\\bin\\windres
CFLAGS_32=-m32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_gfx
CFLAGS_64=-m64 -Wall  -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_gfx
LDFLAGS_32=-m32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_gfx
LDFLAGS_64=-m64 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_gfx
EXEC32=BT_32.exe
EXEC64=BT_64.exe

all: mrproper $(EXEC32) $(EXEC64)




BT_32.exe: BT_32.res BT-data_32.o BT-entreprise_32.o BT-functions_32.o BT-ihm_32.o BT-net_32.o 
	$(CC32) -o $(EXEC32) BT_32.res BT-data_32.o BT-entreprise_32.o BT-functions_32.o BT-ihm_32.o BT-net_32.o $(LDFLAGS_32)
	$(MAKE) clean

BT-data_32.o: BT-data.c
	$(CC32) -o BT-data_32.o -c BT-data.c $(CFLAGS_32)

BT-entreprise_32.o: BT-entreprise.c
	$(CC32) -o BT-entreprise_32.o -c BT-entreprise.c $(CFLAGS_32)

BT-functions_32.o: BT-functions.c
	$(CC32) -o BT-functions_32.o -c BT-functions.c $(CFLAGS_32)

BT-ihm_32.o: BT-ihm.c
	$(CC32) -o BT-ihm_32.o -c BT-ihm.c $(CFLAGS_32)

BT-net_32.o: BT-net.c
	$(CC32) -o BT-net_32.o -c BT-net.c $(CFLAGS_32)

BT_32.res: BT.rc
	$(WIND32) BT.rc -O coff -o BT_32.res





BT_64.exe: BT_64.res BT-data_64.o BT-entreprise_64.o BT-functions_64.o BT-ihm_64.o BT-net_64.o 
	$(CC64) -o $(EXEC64) BT_64.res BT-data_64.o BT-entreprise_64.o BT-functions_64.o BT-ihm_64.o BT-net_64.o $(LDFLAGS_64) 2>nul
	$(MAKE) clean

BT-data_64.o: BT-data.c
	$(CC64) -o BT-data_64.o -c BT-data.c $(CFLAGS_64)

BT-entreprise_64.o: BT-entreprise.c
	$(CC64) -o BT-entreprise_64.o -c BT-entreprise.c $(CFLAGS_64)

BT-functions_64.o: BT-functions.c
	$(CC64) -o BT-functions_64.o -c BT-functions.c $(CFLAGS_64)

BT-ihm_64.o: BT-ihm.c
	$(CC64) -o BT-ihm_64.o -c BT-ihm.c $(CFLAGS_64)

BT-net_64.o: BT-net.c
	$(CC64) -o BT-net_64.o -c BT-net.c $(CFLAGS_64)

BT_64.res: BT.rc
	$(WIND64) BT.rc -O coff -o BT_64.res





clean:
	rm -rf *.o *.res

mrproper: clean
	rm -rf $(EXEC32) $(EXEC64)