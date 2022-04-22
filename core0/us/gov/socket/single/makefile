#===-                           P L E B B L E
#===-                         https://plebble.us
#===-
#===-              Copyright (C) 2017-2022 root1m3@plebble.us
#===-
#===-                      GNU GENERAL PUBLIC LICENSE
#===-                       Version 3, 29 June 2007
#===-
#===-    This program is free software: you can redistribute it and/or modify
#===-    it under the terms of the AGPLv3 License as published by the Free
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
OBJ:= socket_single
INC:= ../../../..
CPPS:= $(wildcard *.cpp)
OBJS:= $(patsubst %.cpp, %.o, $(CPPS))
HS:= $(wildcard *.h)
CXXFLAGS=-O0 -g -std=c++20 -DDEBUG -Wno-psabi
CXX:=g++

${OBJ}.o: deps ${OBJS}
	ld -r -o ${OBJ}.o ${OBJS}

%.o: %.cpp
	$(CXX) -fPIC -Wno-psabi -c ${CXXFLAGS} $^ -pthread -I${INC} -o $@

deps:
	@for a in $$(ls); do \
	if [ -d $$a ]; then \
	    $(MAKE) -C $$a; \
	fi; \
	done;

clean:
	@for a in $$(ls); do \
	if [ -d $$a ]; then \
	    $(MAKE) -C $$a clean; \
	fi; \
	done;
	rm -f *.o

.PHONY: all deps

