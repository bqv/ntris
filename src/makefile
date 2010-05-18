
cc=gcc

bin = 4dtris

sources = 4dt_ai.c 4dt_eng.c 4dt_main.c

objects = $(sources:.c=.o) 

cflags =

libs = -lglut

all : $(bin)

$(bin) : $(objects)
	$(cc) $(cflags) -o $(bin) $(objects) $(libs)

%.o : %.c
	$(cc) $(cflags) -c -o $@ $< 
