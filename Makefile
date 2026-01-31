# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra -Isrc 

# Regola principale: compila tutto ciò che è .c in src/
all:
	$(CC) $(CFLAGS) src/*.c -lm -o plant_diag

# Pulisce l'eseguibile (Windows: del / Linux/Mac: rm)
clean:
	rm -f plant_diag
