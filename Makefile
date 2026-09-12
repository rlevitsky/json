CFLAGS = -Wall -Wextra

default:
	gcc $(CFLAGS) -o demo demo.c json.c
	gcc $(CFLAGS) -o simple_demo simple_demo.c json.c

clean:
	rm -f demo simple_demo
