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
OBJ:=libtrader
CXXFLAGS = -O0 -g -std=c++20 -DDEBUG
CPPS:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp, %.o, $(CPPS))
HS:=$(wildcard *.h)

INC:= ../../..

LIBS:=workflow/libtrader-workflow.o
LIBS+=personality/libtrader-personality.o
LIBS+=bootstrap/libtrader-bootstrap.o
LIBS+=r2r/w2w/w2w-w.o

all: ${OBJ}.o

personality/libtrader-personality.o:
	$(MAKE) -C personality

bootstrap/libtrader-bootstrap.o:
	$(MAKE) -C bootstrap

workflow/libtrader-workflow.o:
	$(MAKE) -C workflow

r2r/w2w/w2w-w.o:
	$(MAKE) -C r2r

%.o: %.cpp
	$(CXX) -fPIC -c ${CXXFLAGS} -I${INC} $^ -o $@

${OBJ}.o: ${OBJS} ${LIBS}
	ld -r ${OBJS} ${LIBS} -o ${OBJ}.o

clean:
	$(MAKE) -C r2r clean
	$(MAKE) -C workflow clean
	$(MAKE) -C personality clean
	$(MAKE) -C bootstrap clean
	$(RM) -f *.o

.PHONY: all clean


