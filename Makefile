# Declaratiile de variabile
CC = gcc
CFLAGS = -g -Wall -lm -std=c99
 
# Regula de compilare
build: tema1.c
	$(CC) $(CFLAGS) -o tema1 tema1.c

run: tema1
	./tema1

# Regulile de "curatenie" (se folosesc pentru stergerea fisierelor intermediare si/sau rezultate)
clean :
	rm -f tema1
	rm -f *.out

