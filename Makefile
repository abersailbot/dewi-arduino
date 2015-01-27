all: build

dep: lib/CMPS10/CMPS10.h lib/CMPS10/CMPS10.cpp

lib/CMPS10/CMPS10.h lib/CMPS10/CMPS10.cpp:
	git submodule init
	git submodule update

build: dep
	ino build

clean:
	ino clean

install: build
	ino upload
