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
OBJ:=socket
CXXFLAGS=-O0 -g -std=c++20 -DDEBUG -Wno-psabi
CPPS:=$(wildcard *.cpp)
HS:=$(wildcard *.h)
OBJS:=$(patsubst %.cpp, %.o, $(CPPS))
DOBJS:=single/socket_single.o
DOBJS+=multipeer/socket_multipeer.o
DOBJS+=outbound/socket_outbound.o
INC:=-I../../..

all: ${OBJ}.o

%.o: %.cpp
	$(CXX) -fPIC -Wno-psabi -c ${CXXFLAGS} $^ -pthread ${INC} -o $@

deps:
	@for a in $$(ls); do \
	if [ -d $$a ]; then \
	echo "==============directory socket/$$a"; \
	$(MAKE) -C $$a; \
	fi; \
	done;

${OBJ}.o: deps ${OBJS} 
	ld -r ${DOBJS} ${OBJS} -o ${OBJ}.o

clean:
	@for a in $$(ls); do \
	if [ -d $$a ]; then \
	$(MAKE) -C $$a clean; \
	fi; \
	done;
	$(RM) -f *.o

.PHONY: deps clean all

