output: lastuptime.c
	gcc -o lastuptime lastuptime.c -lm -O2 -s

install:
	cp lastuptime /usr/local/bin/

clean:
	rm lastuptime
