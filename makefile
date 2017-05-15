NAME = GAS
CC = g++

CFLAGS = -std=gnu++14 -Ofast -c -Wall
LFLAGS = -std=gnu++14 -Ofast -lncurses

#------------------------------------------------------------------------------
SOURCE = gas.cpp vec.cpp force.cpp gui.cpp
OBJECT = gas.o vec.o force.o gui.o

#------------------------------------------------------------------------------
$(NAME):$(OBJECT)
	$(CC) -o $@ $(OBJECT) $(LFLAGS)

#------------------------------------------------------------------------------
%.o : %.cpp $(HEADER)
	$(CC) $(CFLAGS) $< -o $@
#------------------------------------------------------------------------------
clean:
	@rm -f *.o
	@rm -f $(NAME)

new: clean $(NAME)


run: $(NAME)
	./$(NAME)
