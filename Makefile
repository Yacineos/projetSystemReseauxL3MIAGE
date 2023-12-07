all: serveur client multi_client client_test serveur_test
serveur:
	gcc serveur.c -o serveur_exec

client:
	gcc client.c -o client_exec

multi_client:
	gcc multi_client.c -o multi_client_exec

client_test:
	gcc -c utilitaires.c -o utilitaires.o
	gcc -c init.c -o init.o
	gcc -c client_test.c -o client_test.o
	gcc utilitaires.o init.o client_test.o -o client_test_exec -lm

serveur_test:
	gcc -c serv_test.c -o serv_test.o
	gcc utilitaires.o init.o serv_test.o -o serveur_test_exec -lm

clean:
	rm -f *_exec
	rm -f *.o