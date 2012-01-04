CC:=gcc
EXE=ibniz
OPTIMIZATION:=-O2
OBJ=clipboard.o ui_sdl.o vm_slow.o

prefix:=/usr/local
uname_S:=$(shell sh -c 'uname -s 2>/dev/null || echo not')

CFLAGS=-std=c99
LDFLAGS=-lm

# SDL
CFLAGS+=$(shell sh -c 'sdl-config --cflags')
LDFLAGS+=$(shell sh -c 'sdl-config --libs')

ifeq ($(uname_S),Darwin)
  X11=no
  LDFLAGS+=-framework Cocoa
endif

ifeq (${uname_S},MINGW32_NT-5.1)
  # For win32 builds using mingw32 (you'll probably need to modify these)
  EXE=ibniz.exe
  CFLAGS+=-static
  LDFLAGS+=-lwinmm
$(EXE): whole.c
	${CC} ${CFLAGS} ${LDFLAGS} -s -ffast-math -fwhole-program whole.c -o $@

whole.c: vm_slow.c ui_sdl.c clipboard.c texts.i font.i vm.h ibniz.h
	cat ui_sdl.c vm_slow.c clipboard.c > whole.c
else
${EXE}: ${OBJ}
	${CC} $^ ${LDFLAGS} -o ${EXE}

endif

ifneq (${X11},no)
  CFLAGS+=-DX11 -lX11
  LDFLAGS+=-lX11
endif

ifeq (${DEBUG},yes)
CFLAGS+=-O0 -g
endif

all: ${EXE}

clean:
	rm -f *.o *~ ibniz vmtest ibniz.exe whole.c

package: clean
	tar -czf ibniz-1.1.tar.gz -C ../ ibniz-1.1/

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
