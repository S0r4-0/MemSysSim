CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRC = src/*.cpp
OUT = bin/memsim.exe

ifeq ($(OS),Windows_NT)
	MKDIR = if not exist bin mkdir bin
	RM    = del /Q bin\memsim.exe 2>nul || exit 0
else
	MKDIR = mkdir -p bin
	RM    = rm -f $(OUT)
endif

all: $(OUT)

$(OUT): $(SRC)
	$(MKDIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: $(OUT)
	./$(OUT)

clean:
	$(RM)
