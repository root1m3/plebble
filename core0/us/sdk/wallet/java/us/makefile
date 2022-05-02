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
us := us
javac := $(JAVA_HOME)/bin/javac
suffix := bc
build := build_${suffix}
javafiles := $(wildcard */*/*/*/*.java) $(wildcard */*/*/*.java) $(wildcard */*/*.java) $(wildcard */*.java) $(wildcard *.java)
classfiles := $(patsubst %.java, ${build}/us/%.class, $(javafiles))

flags := -g
libspath := ..:../libs_bouncy/bcprov-jdk15on-164.jar:../libs_bouncy/bcpkix-jdk15on-164.jar:../libs_bouncy/bcpg-jdk15on-164.jar:../libs_bouncy/bctls-jdk15on-164.jar:../libs_bouncy/bcmail-jdk15on-164.jar
jarfile := ${us}-sdk_${suffix}.jar

all: jar

jar:
	mkdir -p ${build}
jar: ${build}/${jarfile}


${build}/${jarfile}: ${classfiles}
	cd ${build}; jar cf ${jarfile} *; echo "produced file ${jarfile}"; cd ..

${build}/us/%.class: %.java
	mkdir -p ${build}
	${javac} -Xlint -Xdiags:verbose ${flags} -d ${build} -cp ${libspath} $^

clean:
	rm build_* -rf
	rm -f *.jar

info:
	echo "build: ${build}"
	echo "javac: ${javac}"
	echo "javafiles: ${javafiles}"
	echo "classfiles ${classfiles}"

.PHONY: clean all jar info

