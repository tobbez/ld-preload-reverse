all: ld-preload-reverse.so

ld-preload-reverse.so: ld-preload-reverse.c
	gcc -Wall -ldl -fPIC -shared -o ld-preload-reverse.so ld-preload-reverse.c

.PHONY clean:
	rm -f ld-preload-reverse.so
