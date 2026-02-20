FLAGS = -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format 

LINK_FLAGS ?=
M ?= 10

OBJ_COMMON = 

NUMS = 1 2

OUTS = $(foreach n,$(NUMS),$(shell printf "a%02d.out\n" "$(n)"))

all: $(OUTS)

%.o: %.cpp
	-g++ -DM=$(M) -c $(FLAGS) $<

a%.out: main_%.o solve_%.o $(OBJ_COMMON)
	-g++ $(FLAGS) $(LINK_FLAGS) $^ -o $@ -lm

static: 
	$(MAKE) LINK_FLAGS="-static"

clean:
	 rm -f *.o *.out
