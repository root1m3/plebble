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
javafiles := $(wildcard */*/*/*/*.java) $(wildcard */*/*/*.java) $(wildcard */*/*.java) $(wildcard */*.java) $(wildcard *.java)
classfiles := $(patsubst %.java, ${build}/%.class, $(javafiles))

flags := -g
libspath := ..:../libs_bouncy/bcprov-jdk15on-164.jar:../libs_bouncy/bcpkix-jdk15on-164.jar:../libs_bouncy/bcpg-jdk15on-164.jar:../libs_bouncy/bctls-jdk15on-164.jar:../libs_bouncy/bcmail-jdk15on-164.jar
depjar := ../${us}-sdk_${suffix}.jar
jarfile := ${us}-sdk-test_${suffix}.jar

all: jar

info:
	echo "javac: ${javac}"
	echo "javafiles: ${javafiles}"
	echo "classfiles ${classfiles}"

jar: ${build}/${jarfile}
	echo ${jarfile}

${build}/${jarfile}: ${depjar} ${classfiles}
	mkdir -p ${build}
	cd ${build}; jar cfe ${jarfile} * main.class; echo "produced file ${jarfile}"; cd ..

${build}/%.class: %.java
	mkdir -p ${build}
	${javac} -Xlint ${flags} -d ${build} -cp ${depjar}:${libspath}:${build} $^

${build}/main.class: ${build}/us/gov/crypto/test/symmetric_encryption.class ${build}/us/gov/crypto/test/ec.class main.java
	${javac} -Xlint ${flags} -d ${build} -cp ${depjar}:${libspath}:${build} main.java

clean:
	rm build_* -rf
	rm -f ${jarfile}

.PHONY: clean jar info

