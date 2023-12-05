all: serveur client multi_client client_test serveur_test
serveur:
	gcc serveur.c -o serveur_exec

client:
	gcc client.c -o client_exec

multi_client:
	gcc multi_client.c -o multi_client_exec

client_test:
	gcc client_test.c -o client_test_exec

serveur_test:
	gcc serv_test.c -o serveur_test_exec

clean:
	rm -f *_exec