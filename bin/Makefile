# all:
# 	g++ -g --std=c++17 -I../include -L../lib ../src/*.cpp ../src/imgui/*.cpp ../src/glad.c -lassimpdll -lglfw3dll  -o main

# Kompilator
CC = g++
# Flagi kompilatora
CFLAGS = -g --std=c++17 -I../include -L../lib
# Biblioteki
LIBS = -lassimpdll -lglfw3dll
# Pliki źródłowe
SRCS = ../src/*.cpp ../src/imgui/*.cpp ../src/robot/*.cpp ../src/vectors/*.cpp ../src/glad.c 
# Wyjściowy plik wykonywalny
OUTPUT = main

# # Główna reguła
# all:
# 	$(CC) $(CFLAGS) $(SRCS) $(LIBS) -o $(OUTPUT)

# Główna reguła
all: $(OUTPUT)
	./$(OUTPUT)

# Reguła kompilacji programu
$(OUTPUT):
	$(CC) $(CFLAGS) $(SRCS) $(LIBS) -o $(OUTPUT)
