# Compiler and Standard
CC	= @clang++
STD	= c++11

# Directories
OUT	= ./build
SRC	= ./src
TST = ./test

# Compiler options
WARNL = -Wall -fstandalone-debug -D_DEBUG_
OPT = -O0

# Flags
FLAGS = -std=$(STD) $(WARNL) $(OPT) -g

all: pre $(OUT)/main

pre:
	@mkdir -p build

# Targets
TARGETS = $(OUT)/Consola.o $(OUT)/Imagen.o $(OUT)/Volumen.o $(OUT)/Controlador.o $(OUT)/Huffman.o $(OUT)/GeneradorSemillas.o $(OUT)/Semilla.o

# Object code
$(OUT)/Consola.o: $(SRC)/Consola.cpp $(SRC)/Consola.h
	$(CC) -c $(FLAGS) $< -o $@ -fPIE

$(OUT)/Controlador.o: $(SRC)/Controlador.cpp $(SRC)/Controlador.h
	$(CC) -c $(FLAGS) $< -o $@ -fPIE

$(OUT)/Imagen.o: $(SRC)/Imagen.cpp $(SRC)/Imagen.h
	$(CC) -c $(FLAGS) $< -o $@ -fPIE

$(OUT)/Volumen.o: $(SRC)/Volumen.cpp $(SRC)/Volumen.h
	$(CC) -c $(FLAGS) $< -o $@ -fPIE

$(OUT)/Huffman.o: $(SRC)/Huffman.cpp $(SRC)/Huffman.h $(SRC)/CodigoElemento.hxx $(SRC)/ArbolCodificacion.hxx $(SRC)/NodoCodificacion.hxx $(SRC)/NodoElemento.hxx $(SRC)/NodoFrecuencia.hxx
	$(CC) -c $(FLAGS) $< -o $@ -fPIE

$(OUT)/GeneradorSemillas.o: $(SRC)/GeneradorSemillas.cpp $(SRC)/GeneradorSemillas.h $(SRC)/Imagen.h $(OUT)/Semilla.o
	$(CC) -c $(FLAGS) $< -o $@ -fPIE

$(OUT)/Semilla.o: $(SRC)/Semilla.cpp $(SRC)/Semilla.h
	$(CC) -c $(FLAGS) $< -o $@ -fPIE

# Main compilation target
$(OUT)/main: $(SRC)/main.cpp $(TARGETS)
	$(CC) $(FLAGS) $^ -o $@

# Run
.PHONY: run
run:
	@cd $(OUT) && ./main

# Eliminación
.PHONY: clean
clean:
	rm $(OUT)/*

# Copiar los tests
.PHONY: test1
test1:
	cp $(TST)/IM-126-0002-epiT2/* $(OUT)/

.PHONY: test2
test2:
	cp $(TST)/t1_icbm_5mm_/* $(OUT)/

.PHONY: clear_tests
clear_tests:
	rm -f $(OUT)/*.pgm