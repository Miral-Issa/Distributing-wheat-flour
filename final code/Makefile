CC = gcc
CFLAGS = -Wall
SRCS = perent.c plane.c committee.c disWorker.c family.c OpenGL.c
OBJS = $(SRCS:.c=) 
OBJS += perent.o
LIBS = -lGL -lGLU -lglut -lm

my_program: $(OBJS)
	$(CC) $(CFLAGS) -o $@ perent.o #$(LIBS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(OBJS) my_program


