EXE = ClusterController
SRC_DIR = src
OBJ_DIR = obj
INCL_DIR = include
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS = -Iinclude
CFLAGS = -c -std=c++11 -Wall
LDLIBS = -lboost_system -lboost_thread -lpthread

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	g++ $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CFLAGS) $< -o $@ -I$(INCL_DIR)

clean:
	rm -rf obj/*.o all

