#compiler used
CC = gcc

#flags for individual object file compilation
CFLAGS = -Wall -g -lpthread -lSDL2 -lSDL2_image
#RELEASE
# -Wall -O3
#DEVELOPMENT
# -Wall -g

#source .c files
CLIENT_SRC = $(wildcard src/client/*.c)
SERVER_SRC = $(wildcard src/server/*.c)
3RD_SRC = $(wildcard 3rd/*.c)
#object files created
CLIENT_OBJ = $(patsubst src/client/%.c, bin/client/%.o,$(CLIENT_SRC))
SERVER_OBJ = $(patsubst src/server/%.c, bin/server/%.o,$(SERVER_SRC))
3RD_OBJ = $(patsubst 3rd/%.c, bin/3rd/%.o,$(3RD_SRC))

#executable name
CLIENT = bin/client_app
SERVER = bin/server_app

############################################################

all: $(CLIENT) $(SERVER)

#compile the client
$(CLIENT): $(CLIENT_OBJ) $(3RD_OBJ)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJ) $(3RD_OBJ)
#client objects
bin/client/%.o: src/client/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#compile the server
$(SERVER): $(SERVER_OBJ) $(3RD_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJ) $(3RD_OBJ)
#server objects
bin/server/%.o: src/server/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#3rd party objects
bin/3rd/%.o: 3rd/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm bin/client/*.o
	rm bin/server/*.o