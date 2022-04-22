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
LIB:=${LIB0}
HS:=$(wildcard *.h)

all: consumer organization logistics

consumer: ${HS}
	$(MAKE) CXXFLAGS="${CXXFLAGS}" INC="${INC}" LIB0="${LIB}" -C consumer;

organization: ${HS}
	$(MAKE) CXXFLAGS="${CXXFLAGS}" INC="${INC}" LIB0="${LIB}" -C organization;

logistics: ${HS}
	$(MAKE) CXXFLAGS="${CXXFLAGS}" INC="${INC}" LIB0="${LIB}" -C logistics;

clean:
	$(MAKE) clean -C consumer;
	$(MAKE) clean -C organization;
	$(MAKE) clean -C logistics;

.PHONY: all clean consumer organization logistics

