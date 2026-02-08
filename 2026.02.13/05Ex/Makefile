WFLAGS = -O3 -mfpmath=sse -fstack-protector-all -g \
-W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors \
-Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute \
-Wformat=1 -Wwrite-strings -Wno-long-long -Woverloaded-virtual \
-Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format -std=c++14

LINK_FLAGS ?=

ifeq ($(OS),Windows_NT)
	EXE = exe
	CLEAN = del
else
	EXE = out
	CLEAN = rm -f
endif

TARGET = a.$(EXE)
OBJ = main.o launch.o

%.o: %.cpp
	g++ $(WFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	g++ $(WFLAGS) $(LINK_FLAGS) $^ -o $@

static: 
	$(MAKE) LINK_FLAGS="-static"

gdb: WFLAGS = -mfpmath=sse -fstack-protector-all -O0 -g3 -fno-inline -Wall -Wextra -Werror -std=c++14
gdb: clean $(TARGET)

prof: WFLAGS += -pg
prof: clean $(TARGET)

clean celan:
	$(CLEAN) *.o *.$(EXE)

