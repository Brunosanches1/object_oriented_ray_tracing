SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := include

EXE := $(BIN_DIR)/ray_tracing.exe
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXX = g++
CPPFLAGS := -MMD -MP
CXXFLAGS   := -Wall -O2 -std=c++14
LDFLAGS  := -Llib -L$(INC_DIR)/tinyxml2
LDLIBS   := -lsfml-graphics -lsfml-window -lsfml-system -pthread -lX11 -ltinyxml2
ifdef DEBUG
CXXFLAGS += -g
endif

.PHONY: all clean

all: $(INC_DIR)/tinyxml2/tinyxml2.a | $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

$(INC_DIR)/tinyxml2/tinyxml2.a:
	cd $(INC_DIR)/tinyxml2 && make all

-include $(OBJ:.o=.d)
