WFLAGS = -O3 -mfpmath=sse -fstack-protector-all -g \
	-W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors \
	-Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute \
	-Wformat=1 -Wwrite-strings -Wno-long-long -Woverloaded-virtual \
	-Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format -std=c++14

LINK_FLAGS ?=

ifeq ($(OS),Windows_NT)
	EXE = .exe
	CLEAN = del /Q
else
	EXE =
	CLEAN = rm -f
endif

DB_DIR = database
CL_DIR = frontend

DB_TARGET = server$(EXE)
CL_TARGET = client$(EXE)

DB_SRC = $(DB_DIR)/main.cpp \
		$(DB_DIR)/solve.cpp \
		$(DB_DIR)/pattern.cpp \
		$(DB_DIR)/record.cpp \
		$(DB_DIR)/command.cpp \
		$(DB_DIR)/parse_command.cpp \
		$(DB_DIR)/apply_command.cpp \
		$(DB_DIR)/server.cpp \
		$(DB_DIR)/database_manager.cpp

CL_SRC = $(CL_DIR)/main.cpp \
		$(CL_DIR)/solve.cpp \
		$(CL_DIR)/client.cpp \
		$(CL_DIR)/database_client.cpp \

DB_OBJ = $(DB_SRC:.cpp=.o)
CL_OBJ = $(CL_SRC:.cpp=.o)

all: $(DB_TARGET) $(CL_TARGET)

$(DB_TARGET): $(DB_OBJ)
	g++ $(WFLAGS) $(LINK_FLAGS) $^ -o $@

$(CL_TARGET): $(CL_OBJ)
	g++ $(WFLAGS) $(LINK_FLAGS) $^ -o $@

$(DB_DIR)/%.o: $(DB_DIR)/%.cpp
	g++ $(WFLAGS) -I$(DB_DIR) -c $< -o $@

$(CL_DIR)/%.o: $(CL_DIR)/%.cpp
	g++ $(WFLAGS) -I$(CL_DIR) -c $< -o $@

static:
	$(MAKE) LINK_FLAGS="-static"

gdb: WFLAGS = -mfpmath=sse -fstack-protector-all -O0 -g3 -fno-inline -Wall -Wextra -Werror -std=c++14
gdb: clean all

prof: WFLAGS += -pg
prof: clean all

clean celan:
ifeq ($(OS),Windows_NT)
	-$(CLEAN) $(DB_DIR)\*.o $(CL_DIR)\*.o $(DB_TARGET) $(CL_TARGET)
else
	$(CLEAN) $(DB_DIR)/*.o $(CL_DIR)/*.o $(DB_TARGET) $(CL_TARGET)
endif
