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
OBJ:=apitool
CXXFLAGS=-O3 -std=c++20 -DNDEBUG
CPPS:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp, %.o, $(CPPS))
HS:=$(wildcard *.h)

INC:=../../..

all: generated

debug: export CXXFLAGS:=-O0 -g -std=c++20 -DDEBUG
debug: all

%.o: %.cpp
	${CXX} -fPIC -c ${CXXFLAGS} -I${INC} $^ -o $@

${OBJ}.o: ${OBJS}
	ld -r -o ${OBJ}.o ${OBJS}

apitool: ${OBJ}.o
	${CXX} ${CXXFLAGS} ${OBJ}.o -o apitool

generated: apitool
	./apitool

clean:
	${RM} apitool
	${RM} -r generated
	${RM} *.o

.phony: debug all clean

