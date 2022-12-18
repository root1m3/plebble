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
all: bid2ask games

bid2ask:
	$(MAKE) CXXFLAGS="${CXXFLAGS}" INC="${INC}" -C bid2ask;

games:
	$(MAKE) CXXFLAGS="${CXXFLAGS}" INC="${INC}" -C games;

clean:
	$(MAKE) clean -C bid2ask;
	$(MAKE) clean -C games;

.PHONY: all clean bid2ask games

