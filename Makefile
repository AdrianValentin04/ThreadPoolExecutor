build:
	gcc -Wall *.c -o tema2

run:
	./tema2 input output

clean:
	rm tema2

.PHONY: clean