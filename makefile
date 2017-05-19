NAME = GAS
CC = g++

CFLAGS = -std=gnu++14 -Ofast -c -Wall
LFLAGS = -std=gnu++14 -Ofast -lncurses

#------------------------------------------------------------------------------
SOURCE = gas.cpp vec.cpp force.cpp gui.cpp dispatch.cpp
HEADER = common.h dispatch.h Dispatcher.h force.h gui.h job.h particle.h vec.h
OBJECT = gas.o vec.o force.o gui.o dispatch.o

#------------------------------------------------------------------------------
$(NAME):$(OBJECT)
	$(CC) -o $@ $(OBJECT) $(LFLAGS)

#------------------------------------------------------------------------------
%.o : %.cpp $(HEADER)
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
run: $(NAME)
	./$(NAME)
#------------------------------------------------------------------------------
gfx: CFLAGS += -DUSE_GUI
gfx: clean $(NAME)
