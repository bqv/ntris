
cc=gcc
lint=splint

bin = 4dtris

sources = 4dt_ai.c \
          4dt_eng.c \
          4dt_main.c \
          4dt_scn.c \
          4dt_m3d.c \
          4dt_g3d.c \
          4dt_g4d.c \
          4dt_ui.c

objects = $(sources:.c=.o)

cflags = -Wall

libs = -lglut -lGL -lGLU

all : $(bin)

lint : $(sources)
	$(lint) $< | perl -pe 'chomp; s/^([^\s])/\n\1/' 2>&1

clean:
	rm -rf $(bin) $(objects)

$(bin) : $(objects)
	$(cc) $(cflags) -o $(bin) $(objects) $(libs)

%.o : %.c
	$(cc) $(cflags) -c -o $@ $<
