SERVER = ./server/server.c
CLIENT = ./client/client.c
ROUTER = ./router/router.c

SERVER_NAME = server
CLIENT_NAME = client
ROUTER_NAME = router
DAT_NAME = test.dat

EXEC_DIR = ./exec

service: server client router

all: server client router $(DAT_NAME)

server:
	$(CC) $(SERVER) -o $(EXEC_DIR)/$(SERVER_NAME)

client:
	$(CC) $(CLIENT) -o $(EXEC_DIR)/$(CLIENT_NAME)

router:
	$(CC) $(ROUTER) -o $(EXEC_DIR)/$(ROUTER_NAME)

$(DAT_NAME):
	head -c 2000m /dev/urandom > $(EXEC_DIR)/$(DAT_NAME)

clean:
	$(RM) $(EXEC_DIR)/*

.PHONY: server client router
