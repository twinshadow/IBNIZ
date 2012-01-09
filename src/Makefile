CC:=gcc
EXE=ibniz
OPTIMIZATION:=-Os
OBJ=clipboard.o ui_sdl.o vm_slow.o
VERSION=1.18

prefix:=/usr/local
uname_S:=$(shell sh -c 'uname -s 2>/dev/null || echo not')
uname_O:=$(shell sh -c 'uname -o 2>/dev/null || echo not')

CFLAGS=-std=c99
LDFLAGS=-lm

# SDL
CFLAGS+=$(shell sh -c 'sdl-config --cflags')
LDFLAGS+=$(shell sh -c 'sdl-config --static-libs')

ifeq (${uname_S},Darwin)
  X11=no
  LDFLAGS+=-framework Cocoa
endif

ifeq (${uname_O},Msys)
  EXE=ibniz.exe
  CFLAGS=-ffast-math -fwhole-program
  LDFLAGS=-static
  X11=no
endif

ifneq (${X11},no)
  CFLAGS+=-DX11
  LDFLAGS+=-lX11
endif

ifeq (${DEBUG},yes)
  CFLAGS+=-g
  OPTIMIZATION=-O0
else
  CFLAGS+=-s
endif

all: ${EXE}

${EXE}: ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} ${OPTIMIZATION} -o ${EXE} $^

vmtest: vm_test.c vm_slow.o
	${CC} ${CFLAGS} ${LDFLAGS} ${OPTIMIZATION} -o $@ $^

clean:
	rm -f *.o *~ ibniz vmtest ibniz.exe whole.c

package: clean
	cp -r ../src/ ../ibniz-${VERSION} &&\
	tar -czf ../ibniz-${VERSION}.tar.gz -C ../ ibniz-${VERSION}/ &&\
	rm -rf ../ibniz-${VERSION}
.c.o:
	${CC} -c ${CFLAGS} $< -o $@
ui_sdl.o: ui_sdl.c ibniz.h font.i vm.h texts.i vm.h
clipboard.o: clipboard.c ibniz.h
vm_slow.o: vm_slow.c ibniz.h vm.h
	${CC} -c ${CFLAGS} ${LDFLAGS} -O3 $< -o $@
font.i: font.pl
	perl font.pl > font.i

runtest: vmtest
	./vmtest

.PHONY:clean package runtest
