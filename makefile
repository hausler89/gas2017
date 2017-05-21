NAME = GAS
CC = g++

CFLAGS = -std=gnu++14 -Ofast -c -Wall -Wno-unknown-pragmas
LFLAGS = -std=gnu++14 -Ofast -lncurses -Wno-unknown-pragmas

#------------------------------------------------------------------------------
SOURCE = src/gas.cpp src/vec.cpp src/force.cpp src/gui.cpp src/dispatch.cpp
HEADER = src/common.h src/dispatch.h src/Dispatcher.h src/force.h src/gui.h src/job.h src/particle.h src/vec.h
OBJECT = obj/gas.o obj/vec.o obj/force.o obj/gui.o obj/dispatch.o

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
