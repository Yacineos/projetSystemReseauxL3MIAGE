all: serveur client multi_client
serveur:
	gcc serveur.c -o serveur_exec

client:
	gcc client.c -o client_exec

multi_client:
	gcc multi_client.c -o multi_client_exec

clean:
	rm -f *_exec