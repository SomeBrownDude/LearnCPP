CPPFLAGS = -std=c++17 -Wall -Wextra -Werror -Wfatal-errors -Wno-sign-compare -Wnon-virtual-dtor -g
LDFLAGS=-lncurses

# OBJS = Snake.o	
# TARGET = Snake

OBJS = Clean.o #wildcard for all cpp files
TARGET = Clean

OTHERFLAGS = -O2
DEBUG = -g

all: $(TARGET)

$(TARGET): $(OBJS)
	g++ $(OTHERFLAGS) $(DEBUG) $(CPPFLAGS) -o $(TARGET) $(OBJS) $(OTHER) $(LDFLAGS) 

%.o: %.cpp
	g++ $(CPPFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)


