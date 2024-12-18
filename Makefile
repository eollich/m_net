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


all: main

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
