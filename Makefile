all: serveur client
serveur:
	gcc serveur.c -o serveur_exec

client:
	gcc client.c -o client_exec

clean:
	rm -f *_exec