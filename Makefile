CC=clang++
SRC=src
OBJ=obj
BIN=bin
INCLUDE=include
LIB=lib
LDFLAGS=-L$(LIB)
CCFLAGS=-I$(INCLUDE) -std=c++17

SOURCES = main
OBJECTS=$(foreach source,$(SOURCES),$(OBJ)/$(source).o)


all: main

main: $(OBJECTS)
	[ -d $(BIN) ] || mkdir -p $(BIN)
	$(CC) $^ -o $(BIN)/$@ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cc
	[ -d $(OBJ) ] || mkdir -p $(OBJ)
	$(CC) $(CCFLAGS) $^ -c -o $@

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)
