all: ld-preload-reverse.so

ld-preload-reverse.so: ld-preload-reverse.c
	gcc -Wall -fPIC -shared -o ld-preload-reverse.so ld-preload-reverse.c -ldl 

.PHONY clean:
	rm -f ld-preload-reverse.so
