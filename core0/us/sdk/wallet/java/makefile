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
us := us
javac := $(JAVA_HOME)/bin/javac
suffix := bc
build := build_${suffix}
javafiles := $(wildcard *.java) $(wildcard */*.java)
classfiles := $(patsubst %.java, %.class, $(javafiles))

flags := -g
libspath := .:libs_bouncy/bcprov-jdk15on-164.jar:libs_bouncy/bcpkix-jdk15on-164.jar:libs_bouncy/bcpg-jdk15on-164.jar:libs_bouncy/bctls-jdk15on-164.jar:libs_bouncy/bcmail-jdk15on-164.jar
jarfile := ${us}-sdk_${suffix}.jar

all: bouncy spongybouncy


${us}-sdk_bc.jar: bouncy



#for android app
spongybouncy: bouncy
	./make.sh -spongy

spongy:
	./make.sh -spongy

#for console app
bouncy:
	./make.sh -bouncy

test: jar
#	./make.sh -bouncy test
	$(MAKE) -C test
	cp test/${build}/*.jar .


jar:
	$(MAKE) -C us jar
	cp us/${build}/*.jar .

clean:
	$(MAKE) -C us clean
	$(MAKE) -C test clean
	rm *.jar -f

.PHONY: spongy bouncy clean test jar spongybouncy

