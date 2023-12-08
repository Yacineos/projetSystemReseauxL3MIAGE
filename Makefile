CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_DIR = sources
HEADERS_DIR = headers
BIN_DIR = bin

all: client serveur

client: $(BIN_DIR)/utilitaires.o $(BIN_DIR)/init.o $(BIN_DIR)/client.o
	$(CC) $(CFLAGS) $(BIN_DIR)/utilitaires.o $(BIN_DIR)/init.o $(BIN_DIR)/client.o -o $(BIN_DIR)/client_exec

serveur: $(BIN_DIR)/utilitaires.o $(BIN_DIR)/init.o $(BIN_DIR)/serveur.o
	$(CC) $(CFLAGS) $(BIN_DIR)/utilitaires.o $(BIN_DIR)/init.o $(BIN_DIR)/serveur.o -o $(BIN_DIR)/serveur_exec

$(BIN_DIR)/utilitaires.o: $(SRC_DIR)/utilitaires.c $(HEADERS_DIR)/utilitaires.h
	$(CC) $(CFLAGS) -I$(HEADERS_DIR) -c $< -o $@

$(BIN_DIR)/init.o: $(SRC_DIR)/init.c $(HEADERS_DIR)/init.h
	$(CC) $(CFLAGS) -I$(HEADERS_DIR) -c $< -o $@

$(BIN_DIR)/client.o: $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) -I$(HEADERS_DIR) -c $< -o $@

$(BIN_DIR)/serveur.o: $(SRC_DIR)/serveur.c
	$(CC) $(CFLAGS) -I$(HEADERS_DIR) -c $< -o $@

clean:
	rm -f *_exec
	rm -f $(BIN_DIR)/*.o