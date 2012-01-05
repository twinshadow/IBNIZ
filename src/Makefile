CC:=gcc
EXE=ibniz
OPTIMIZATION:=-O2
OBJ=clipboard.o ui_sdl.o vm_slow.o
VERSION=1.18

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
  EXE=ibniz.exe
  CFLAGS+=-static -ffast-math -fwhole-program
  LDFLAGS+=-lwinmm
  X11=no
whole.c: vm_slow.c ui_sdl.c clipboard.c texts.i font.i vm.h ibniz.h
	cat ui_sdl.c vm_slow.c clipboard.c > whole.c
endif

ifneq (${X11},no)
  CFLAGS+=-DX11
  LDFLAGS+=-lX11
endif

ifeq (${DEBUG},yes)
  CFLAGS+=-O0 -g
else
  CFLAGS+=-s
endif

all: ${EXE}

${EXE}: ${OBJ}
	${CC} $^ ${LDFLAGS} -o ${EXE}

vmtest: vm_test.c vm_slow.o
	${CC} $^ ${LDFLAGS} -o $@

clean:
	rm -f *.o *~ ibniz vmtest ibniz.exe whole.c

package: clean
	tar -czf ibniz-${VERSION}.tar.gz -C ../ ibniz-${VERSION}/

ui_sdl.o: ui_sdl.c ibniz.h font.i vm.h texts.i vm.h
clipboard.o: clipboard.c ibniz.h
vm_slow.o: vm_slow.c ibniz.h vm.h
font.i: font.pl
	perl font.pl > font.i

runtest: vmtest
	./vmtest

.PHONY:clean package runtest
