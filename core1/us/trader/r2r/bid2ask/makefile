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
PROT:=bid2ask
R1:=bid
R2:=ask
OBJR1:=${PROT}-${R1}.o
OBJR2:=${PROT}-${R2}.o
CPPS:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp, %.o, $(CPPS))
HS:=$(wildcard *.h)

all: ${OBJR1} ${OBJR2}

%.o: %.cpp
	${CXX} -fPIC -c ${CXXFLAGS} ${INC} $^ -o $@

${R1}/role.o:
	$(MAKE) CXXFLAGS="${CXXFLAGS}" INC="${INC}" -C ${R1};

${R2}/role.o:
	$(MAKE) CXXFLAGS="${CXXFLAGS}" INC="${INC}" -C ${R2};

${OBJR1}: ${OBJS} ${CPPS} ${HS} ${R1}/role.o
	ld -z defs -r ${R1}/role.o ${OBJS} -o ${OBJR1}

${OBJR2}: ${OBJS} ${CPPS} ${HS} ${R2}/role.o
	ld -z defs -r ${R2}/role.o ${OBJS} -o ${OBJR2}

clean:
	$(MAKE) clean -C ${R1};
	$(MAKE) clean -C ${R2};
	$(RM) -f *.o

.PHONY: all clean

