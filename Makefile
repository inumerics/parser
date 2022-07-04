CC = g++
CFLAGS  = -std=c++14 -Wall

SRC     = parser/
BUILD   = build/
TEST    = test/
BIN	    = bin/

HEADERS  = $(SRC)finite.hpp  $(SRC)literal.hpp $(SRC)regex.hpp $(SRC)node.hpp
HEADERS += $(SRC)lexer.hpp   $(SRC)symbols.hpp $(SRC)grammar.hpp
HEADERS += $(SRC)state.hpp   $(SRC)solver.hpp  $(SRC)parser.hpp
HEADERS += $(SRC)display.hpp $(SRC)code.hpp

OBJECTS  = $(BUILD)finite.o  $(BUILD)literal.o $(BUILD)regex.o $(BUILD)node.o
OBJECTS += $(BUILD)lexer.o   $(BUILD)symbols.o $(BUILD)grammar.o
OBJECTS += $(BUILD)state.o 	 $(BUILD)solver.o  $(BUILD)parser.o
OBJECTS += $(BUILD)display.o $(BUILD)code.o

#*******************************************************************************
all: $(BIN)parser

$(OBJECTS): | $(BUILD)

$(BUILD):
	mkdir -p $(BUILD)
	mkdir -p $(BIN)

$(BUILD)finite.o: $(SRC)finite.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)literal.o: $(SRC)literal.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)regex.o: $(SRC)regex.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)node.o: $(SRC)node.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)lexer.o: $(SRC)lexer.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)symbols.o: $(SRC)symbols.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)grammar.o: $(SRC)grammar.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)state.o: $(SRC)state.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)solver.o: $(SRC)solver.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)display.o: $(SRC)display.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(BUILD)parser.o: $(SRC)parser.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(BUILD)code.o: $(SRC)code.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(BUILD)main.o: $(SRC)main.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN)parser: $(OBJECTS) $(HEADERS) $(BUILD)main.o
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(BUILD)main.o

#*******************************************************************************
.PHONY: test

test: $(BIN)test
	cd $(TEST); ../$(BIN)test

$(BIN)test: $(OBJECTS) $(HEADERS) $(BUILD)test.o
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(BUILD)test.o

$(BUILD)test.o: $(TEST)test.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $< -I parser/

#*******************************************************************************
.PHONY: calculator

calculator: $(BIN)calculator
	$(BIN)calculator "(5 + 5) * 3"

$(BIN)calculator: $(BUILD)calculator.o $(BUILD)states.o
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD)calculator.o: $(TEST)calculator.cpp $(TEST)calculator.hpp | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $< -I src/

$(BUILD)states.o: $(BUILD)states.cpp | $(BUILD)
	$(CC) $(CFLAGS) -c -o $@ $< -I test/ -I src/

$(BUILD)states.cpp: $(TEST)calculator.bnf $(BIN)parser
	./$(BIN)parser < $(TEST)calculator.bnf > $(BUILD)states.cpp

#*******************************************************************************
clean:
	rm -f $(BIN)calculator
	rm -f $(BIN)parser
	rm -f $(BIN)test
	rm -f -d $(BIN)

	rm -f $(OBJECTS)
	rm -f $(BUILD)calculator.o
	rm -f $(BUILD)test.o
	rm -f $(BUILD)states.o
	rm -f $(BUILD)states.cpp
	rm -f $(BUILD)main.o
	rm -f -d $(BUILD)
