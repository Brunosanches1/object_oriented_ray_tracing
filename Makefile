SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := include

EXE := $(BIN_DIR)/ray_tracing.exe
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) 

CXX = g++
CPPFLAGS := -MMD -MP -fopenmp
CXXFLAGS   := -Wall -O2 -std=c++14  -g
LDFLAGS  := -L./lib -Linclude -Iinclude/sfml-widgets-master/src
LDLIBS   := -lsfml-graphics -lsfml-window -lsfml-system -pthread -lX11 -fopenmp lib/libsfml-widgets.a

.PHONY: all clean

all: clean $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) include/tinyxml2.cpp -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Iinclude/sfml-widgets-master/src -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

install:
	sudo apt-get install libsfml-dev
	cd include/sfml-widgets-master && make
	mkdir -p lib/
	cp include/sfml-widgets-master/lib/libsfml-widgets.a lib/libsfml-widgets.a


test: $(EXE)
	./$(EXE)

-include $(OBJ:.o=.d)
