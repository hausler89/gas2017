NAME = GAS
CC = g++

CFLAGS = -std=gnu++14 -Ofast -c -Wall -Wno-unknown-pragmas
LFLAGS = -std=gnu++14 -Ofast -lncurses -Wno-unknown-pragmas

SRC_PREFIX = src/
OBJ_PREFIX = obj/

#------------------------------------------------------------------------------
SOURCE_FILES = gas.cpp vec.cpp force.cpp gui.cpp dispatch.cpp
HEADER_FILES = common.h dispatch.h Dispatcher.h force.h gui.h job.h particle.h vec.h
OBJECT_FILES = gas.o vec.o force.o gui.o dispatch.o

SOURCE = $(addprefix $(SRC_PREFIX),$(SOURCE_FILES))
HEADER = $(addprefix $(SRC_PREFIX),$(HEADER_FILES))
OBJECT = $(addprefix $(OBJ_PREFIX),$(OBJECT_FILES))

#------------------------------------------------------------------------------
$(NAME):$(OBJECT)
	$(CC) -o $@ $(OBJECT) $(LFLAGS)

#------------------------------------------------------------------------------
obj/%.o : src/%.cpp $(HEADER)
	$(CC) $(CFLAGS) $< -o $@
#------------------------------------------------------------------------------
debug: CFLAGS = -std=gnu++14 -O0 -c -Wall -g
debug: LFLAGS = -std=gnu++14 -O0 -lncurses -g
debug: $(NAME)
#------------------------------------------------------------------------------
clean:
	@rm -f *.o
	@rm -f $(NAME)
	@rm -f obj/*
#------------------------------------------------------------------------------
new: clean $(NAME)
#------------------------------------------------------------------------------
openmp: CFLAGS += -fopenmp
openmp: LFLAGS += -fopenmp
openmp: $(NAME)
#------------------------------------------------------------------------------
run: $(NAME)
	./$(NAME)
#------------------------------------------------------------------------------
gfx: CFLAGS += -DUSE_GUI
gfx: clean $(NAME)