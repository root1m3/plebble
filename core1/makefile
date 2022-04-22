US:=us
DEFAULTDEBUG:=0
STDFLAGS:=-std=c++20 -Wno-psabi
DEBUGFLAGS:=-g -O0 ${STDFLAGS} -DDEBUG
RELEASEFLAGS:=-O3 ${STDFLAGS} -DNDEBUG
PREFIX:=/usr/local
ARCH:=$(shell uname -m)
CXXFLAGS=${DEBUGFLAGS}
CXX:=g++
CORE0:=$(shell realpath .)
CORE1:=$(shell realpath .)
INCC0:=-I$(shell realpath ./include)
LIBC0:=-L$(shell realpath ./lib)
INCC1:=-I${CORE1}
INC:=${INCC0} ${INCC1}
LIB:=${LIBC0}

all:
	$(MAKE) CXX="${CXX}" INC="${INC}" LIBC0="${LIB}" -C us/trader

clean:
	$(MAKE) -C us/trader clean

.PHONY: all clean
