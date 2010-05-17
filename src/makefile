
cc=gcc

bin = 4dtris

sources = 4DTris_AI.c 4DTris_engine.c 4DTris_main.c

objects = $(sources:.c=.o)

cflags =

libs = -lglut

all : $(bin)

$(bin) : $(objects)
	$(cc) $(cflags) -o $(bin) $(objects) $(libs)

%.o : %.c
	$(cc) $(cflags) -c -o $@ $<
