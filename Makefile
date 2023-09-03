CC = g++
CXXFLAGS  = -std=c++14 -Wall

LEXER   = lexer/
PARSER  = parser/
BUILD   = build/
TEST    = test/
BIN	    = bin/

HEADERS  = $(LEXER)finite.hpp $(LEXER)literal.hpp $(LEXER)regex.hpp $(LEXER)node.hpp \
			$(LEXER)lexer.hpp $(PARSER)symbols.hpp $(PARSER)grammar.hpp \
			$(PARSER)state.hpp $(PARSER)solver.hpp \
			$(PARSER)display.hpp $(PARSER)code.hpp $(PARSER)options.hpp

OBJECTS  = $(BUILD)finite.o $(BUILD)literal.o $(BUILD)regex.o $(BUILD)node.o \
			$(BUILD)lexer.o $(BUILD)symbols.o $(BUILD)grammar.o \
			$(BUILD)state.o $(BUILD)solver.o \
			$(BUILD)display.o $(BUILD)code.o $(BUILD)options.o

#*******************************************************************************
all: $(BIN)parser

$(BIN)parser: $(OBJECTS) $(HEADERS) $(BUILD)main.o | $(BIN)
	$(CC) $(CXXFLAGS) -o $@ $(OBJECTS) $(BUILD)main.o

$(BUILD)main.o: main.cpp $(HEADERS) | $(BUILD)
	$(CC) $(CXXFLAGS) -I $(LEXER) -I $(PARSER) -c -o $@ $<

$(BUILD)%.o: $(PARSER)%.cpp $(HEADERS) | $(BUILD)
	$(CC) $(CXXFLAGS) -I $(LEXER) -c -o $@ $<

$(BUILD)%.o: $(LEXER)%.cpp $(HEADERS) | $(BUILD)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)
	
$(BIN):
	mkdir -p $(BIN)

#*******************************************************************************
clean:
	rm -f $(BIN)parser
	rm -f -d $(BIN)

	rm -f $(OBJECTS)
	rm -f $(BUILD)states.o
	rm -f $(BUILD)states.cpp
	rm -f $(BUILD)main.o
	rm -f -d $(BUILD)
