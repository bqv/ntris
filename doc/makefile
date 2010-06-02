
all: swstruct doxygen

swstruct: gen/4dtris-sw-struct.png

doxygen: swstruct
	cd ../src; doxygen

gen/4dtris-sw-struct.png:
	perl gen/sw-struct.pl ../src 4dt_ gen/4dtris-sw-struct.png