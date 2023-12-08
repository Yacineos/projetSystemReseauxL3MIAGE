all: client serveur
client:
	gcc -c utilitaires.c -o utilitaires.o
	gcc -c init.c -o init.o
	gcc -c client.c -o client.o
	gcc utilitaires.o init.o client.o -o client_exec

serveur:
	gcc -c serveur.c -o serveur.o
	gcc utilitaires.o init.o serveur.o -o serveur_exec

clean:
	rm -f *_exec
	rm -f *.o