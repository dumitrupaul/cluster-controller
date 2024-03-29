EXE = ClusterController
SRC_DIR = src
OBJ_DIR = obj
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS = -Iinclude
CFLAGS = -c -std=c++14 -Wall -DBOOST_LOG_DYN_LINK
LDLIBS = -lboost_system -lboost_thread -lpthread -lboost_log -lboost_log_setup -lcrypto -lssl

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	g++ $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	g++ $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXE)
	rm -rf obj/*.o all
cleanlog:
	rm -f *.log


