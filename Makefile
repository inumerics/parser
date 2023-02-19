CC = g++
CXXFLAGS  = -std=c++14 -Wall

SRC     = src/
LEXER   = lexer/
BUILD   = build/
TEST    = test/
CALC    = calculator/
BIN	    = bin/

HEADERS  = $(LEXER)finite.hpp $(LEXER)literal.hpp $(LEXER)regex.hpp $(LEXER)node.hpp \
			$(LEXER)lexer.hpp $(SRC)symbols.hpp $(SRC)grammar.hpp \
			$(SRC)state.hpp $(SRC)solver.hpp \
			$(SRC)display.hpp $(SRC)code.hpp $(SRC)options.hpp

OBJECTS  = $(BUILD)finite.o $(BUILD)literal.o $(BUILD)regex.o $(BUILD)node.o \
			$(BUILD)lexer.o $(BUILD)symbols.o $(BUILD)grammar.o \
			$(BUILD)state.o $(BUILD)solver.o \
			$(BUILD)display.o $(BUILD)code.o $(BUILD)options.o

#*******************************************************************************
all: $(BIN)parser

$(BIN)parser: $(OBJECTS) $(HEADERS) $(BUILD)main.o | $(BIN)
	$(CC) $(CXXFLAGS) -o $@ $(OBJECTS) $(BUILD)main.o

$(BUILD)main.o: main.cpp $(HEADERS) | $(BUILD)
	$(CC) $(CXXFLAGS) -I $(LEXER) -I $(SRC) -c -o $@ $<

$(BUILD)%.o: $(SRC)%.cpp $(HEADERS) | $(BUILD)
	$(CC) $(CXXFLAGS) -I $(LEXER) -c -o $@ $<

$(BUILD)%.o: $(LEXER)%.cpp $(HEADERS) | $(BUILD)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)
	
$(BIN):
	mkdir -p $(BIN)

#*******************************************************************************
.PHONY: calculator

calculator: $(BIN)calculator
	cd $(CALC); ../$(BIN)calculator "(5 + 6 - 1) * 0x0A / 2"

$(BIN)calculator: $(BUILD)calculator.o $(BUILD)states.o
	$(CC) $(CXXFLAGS) -o $@ $^

$(BUILD)calculator.o: $(CALC)calculator.cpp $(CALC)calculator.hpp $(HEADERS) | $(BUILD)
	$(CC) $(CXXFLAGS) -c -o $@ $<

$(BUILD)states.o: $(BUILD)states.cpp
	$(CC) $(CXXFLAGS) -I $(CALC) -c -o $@ $<

$(BUILD)states.cpp: $(CALC)calculator.bnf $(BIN)parser | $(BUILD)
	$(BIN)parser -o $(BUILD)states.cpp $(CALC)calculator.bnf

#*******************************************************************************
clean:
	rm -f $(BIN)parser
	rm -f $(BIN)calculator
	rm -f -d $(BIN)

	rm -f $(OBJECTS)
	rm -f $(BUILD)calculator.o
	rm -f $(BUILD)states.o
	rm -f $(BUILD)states.cpp
	rm -f $(BUILD)main.o
	rm -f -d $(BUILD)
