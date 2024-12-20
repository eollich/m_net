CC=clang++
SRC=src
OBJ=obj
BIN=bin
INCLUDE=include
LIB=lib
LDFLAGS=-L$(LIB) -lncurses
CCFLAGS=-I$(INCLUDE) -std=c++17 -D_XOPEN_SOURCE_EXTENDED

# SOURCES = main ui/chat_ui
# OBJECTS=$(foreach source,$(SOURCES),$(OBJ)/$(source).o)

SOURCES = $(shell find $(SRC) -name '*.cc')

# Define object files with the appropriate path
OBJECTS = $(SOURCES:$(SRC)/%.cc=$(OBJ)/%.o)


#SERVER_SRCS = tcp_server tcp_echo_server
SERVER_SRCS = tcp_server 
SERVER_OBJS=$(foreach source,$(SERVER_SRCS),$(OBJ)/net/$(source).o)

CLIENT_SRCS = tcp_client
CLIENT_OBJS=$(foreach source,$(CLIENT_SRCS),$(OBJ)/net/$(source).o)

M_NET_CLIENT_SRCS = m_net_client
M_NET_CLIENT_OBJS=$(foreach source,$(M_NET_CLIENT_SRCS),$(OBJ)/net/$(source).o)

M_NET_SERVER_SRCS = m_net_server
M_NET_SERVER_OBJS=$(foreach source,$(M_NET_SERVER_SRCS),$(OBJ)/net/$(source).o)

TCP_SRCS = net_util tcp
TCP_OBJS=$(foreach source,$(TCP_SRCS),$(OBJ)/net/$(source).o)

all: main


eserver: $(OBJ)/eserver.o $(SERVER_OBJS) $(TCP_OBJS)
	[ -d $(BIN) ] || mkdir -p $(BIN)
	$(CC) $^ -o $(BIN)/$@ $(LDFLAGS)

eclient: $(OBJ)/eclient.o $(CLIENT_OBJS) $(TCP_OBJS)
	[ -d $(BIN) ] || mkdir -p $(BIN)
	$(CC) $^ -o $(BIN)/$@ $(LDFLAGS)

mnetclient: $(M_NET_CLIENT_OBJS) $(CLIENT_OBJS) $(TCP_OBJS)
	[ -d $(BIN) ] || mkdir -p $(BIN)
	$(CC) $^ -o $(BIN)/$@ $(LDFLAGS)

mnetserver: $(M_NET_SERVER_OBJS) $(SERVER_OBJS) $(TCP_OBJS)
	[ -d $(BIN) ] || mkdir -p $(BIN)
	$(CC) $^ -o $(BIN)/$@ $(LDFLAGS)

main: $(OBJECTS)
	[ -d $(BIN) ] || mkdir -p $(BIN)
	$(CC) $^ -o $(BIN)/$@ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cc
	@mkdir -p $(dir $@)        # Creating the directory if it doesn't exist
	#[ -d $(OBJ) ] || mkdir -p $(OBJ)
	$(CC) $(CCFLAGS) $^ -c -o $@

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)
