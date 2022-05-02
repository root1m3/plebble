#===-                           P L E B B L E
#===-                         https://plebble.us
#===-
#===-              Copyright (C) 2017-2022 root1m3@plebble.us
#===-
#===-                      GNU GENERAL PUBLIC LICENSE
#===-                       Version 3, 29 June 2007
#===-
#===-    This program is free software: you can redistribute it and/or modify
#===-    it under the terms of the GPLv3 License as published by the Free
#===-    Software Foundation.
#===-
#===-    This program is distributed in the hope that it will be useful,
#===-    but WITHOUT ANY WARRANTY; without even the implied warranty of
#===-    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#===-
#===-    You should have received a copy of the General Public License
#===-    along with this program, see LICENCE file.
#===-    see https://www.gnu.org/licenses
#===-
#===----------------------------------------------------------------------------
#===-
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
