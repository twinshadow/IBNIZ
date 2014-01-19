uname_S:=$(shell uname -s)
uname_O:=$(shell uname -o)

# maybe works
PLATFORM=	Makefile.in

# Define this for custom target platforms
TARGET=

ifeq (${uname_S}, Linux)
PLATFORM=	Makefile.in
endif

ifeq (${uname_S}, Darwin)
PLATFORM=	Makefile.osx
endif

ifeq (${uname_O}, Msys)
PLATFORM=	Makefile.win
endif

include		${PLATFORM}
