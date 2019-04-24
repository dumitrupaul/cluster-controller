EXE = ClusterController
SRC_DIR = src
OBJ_DIR = obj
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS = -Iinclude
CFLAGS = -c -std=c++11 -Wall -DBOOST_LOG_DYN_LINK
LDLIBS = -lboost_system -lboost_thread -lpthread -lboost_log -lboost_log_setup

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	g++ $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	g++ $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	rm $(EXE)
	rm -rf obj/*.o all
cleanlog:
	rm *.log

