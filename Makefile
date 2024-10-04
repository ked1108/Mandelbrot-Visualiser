CC = gcc

CFLAGS = -Wall -Wextra -pedantic

LFLAGS = -lraylib -fopenmp -lm

SRCS = main.c

OUT = mandelbrot

all: $(OUT)
	
$(OUT):
	$(CC) $(CFLAGS) $(LFLAGS) -o $(OUT) $(SRCS)

clean: 
	rm -f $(OUT)

install:
	install -m 755 $(OUT) /usr/local/bin

uninstall:
	rm -f /usr/local/bin/$(OUT)
