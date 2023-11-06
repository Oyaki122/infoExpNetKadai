SERVER = ./server/server.c
CLIENT = ./client/client.c
ROUTER = ./router/router.c

SERVER_NAME = server
CLIENT_NAME = client
ROUTER_NAME = router
DAT_NAME = test.dat

EXEC_DIR = ./exec

#service: server client router

all: server client router $(EXEC_DIR)/$(DAT_NAME)

server:
	$(CC) $(SERVER) -o $(EXEC_DIR)/$(SERVER_NAME)

client:
	$(CC) $(CLIENT) -o $(EXEC_DIR)/$(CLIENT_NAME)

router:
	$(CC) $(ROUTER) -o $(EXEC_DIR)/$(ROUTER_NAME)

$(EXEC_DIR)/$(DAT_NAME):
ifeq ("$(wildcard $@)", "")
	head -c 100m /dev/urandom > $@
endif

clean:
	$(RM) $(EXEC_DIR)/*

.PHONY: server client router
