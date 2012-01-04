CC?=gcc
EXE=ibniz
OPTIMIZATION?=-O2
OBJ=clipboard.o ui_sdl.o vm_slow.o

prefix:=/usr/local
uname_S:=$(shell sh -c 'uname -s 2>/dev/null || echo not')

CFLAGS=-std=c99 -lm
LDFLAGS=-lm

# SDL
CFLAGS+=$(shell sh -c 'sdl-config --cflags')
LDFLAGS+=$(shell sh -c 'sdl-config --libs')

ifeq ($(uname_S),Linux)
  # X11LIB
  # For normal builds; remove -DX11 -lX11 from flags if you don't have X11
  CFLAGS+=-DX11 -lX11
  LDFLAGS+=-lX11
  ifeq (${DEBUG},yes)
  	CFLAGS+=-O0 -g
  endif
endif

ifeq ($(uname_S),Darwin)
  LDFLAGS+=-framework Cocoa
endif

ifeq (${uname_S},MINGW32_NT-5.1)
  # For win32 builds using mingw32 (you'll probably need to modify these)
  EXE=ibniz.exe
  CFLAGS+=-lwinmm
$(EXE): whole.c
	${CC} ${CFLAGS} -s -ffast-math -fwhole-program whole.c -o $@

whole.c: vm_slow.c ui_sdl.c clipboard.c texts.i font.i vm.h ibniz.h
	cat ui_sdl.c vm_slow.c clipboard.c > whole.c
endif

all: ${EXE}

clean:
	rm -f *.o *~ ibniz vmtest ibniz.exe whole.c

package: clean
	tar -czf ibniz-1.1.tar.gz -C ../ ibniz-1.1/

${EXE}: ${OBJ}
	${CC} $^ ${LDFLAGS} -o ${EXE}

vmtest: vm_test.o vm_slow.o
	${CC} $^ ${LDFLAGS} -o $@

ui_sdl.o: ui_sdl.c ibniz.h font.i vm.h texts.i vm.h
clipboard.o: clipboard.c ibniz.h
vm_slow.o: vm_slow.c ibniz.h vm.h
font.i: font.pl
	perl font.pl > font.i

runtest: vmtest
	./vmtest

.PHONY:clean package runtest
