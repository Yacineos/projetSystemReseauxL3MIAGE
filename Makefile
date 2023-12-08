CC = gcc
CFLAGS = -Wall -Wextra -g

all: client serveur

client: utilitaires.o init.o client.o
	$(CC) $(CFLAGS) utilitaires.o init.o client.o -o client_exec

serveur: utilitaires.o init.o serveur.o
	$(CC) $(CFLAGS) utilitaires.o init.o serveur.o -o serveur_exec

utilitaires.o: utilitaires.c
	$(CC) $(CFLAGS) -c utilitaires.c -o utilitaires.o

init.o: init.c
	$(CC) $(CFLAGS) -c init.c -o init.o

client.o: client.c
	$(CC) $(CFLAGS) -c client.c -o client.o

serveur.o: serveur.c
	$(CC) $(CFLAGS) -c serveur.c -o serveur.o

clean:
	rm -f *_exec
	rm -f *.o