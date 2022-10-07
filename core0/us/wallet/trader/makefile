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
OBJ := libtrader
CPPS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(CPPS))
HS := $(wildcard *.h)
INC := ../../..
CXXFLAGS = -O0 -g -std=c++20 -DDEBUG
LIBS := workflow/libtrader-workflow.o personality/libtrader-personality.o bootstrap/libtrader-bootstrap.o r2r/w2w/libtrader-r2r-w2w.o

.PHONY: all clean

all: ${OBJ}.o

r2r/w2w/libtrader-r2r-w2w.o:
	$(MAKE) -C r2r

workflow/libtrader-workflow.o:
	$(MAKE) -C workflow

personality/libtrader-personality.o:
	$(MAKE) -C personality

bootstrap/libtrader-bootstrap.o:
	$(MAKE) -C bootstrap

${OBJ}.o: ${OBJS} ${LIBS}
	ld -r -o ${OBJ}.o ${LIBS} ${OBJS}

%.o: %.cpp
	$(CXX) -fPIC -c ${CXXFLAGS} -I${INC} $^ -o $@

clean:
	$(MAKE) -C r2r clean
	$(MAKE) -C workflow clean
	$(MAKE) -C personality clean
	$(MAKE) -C bootstrap clean
	rm *.o -f


