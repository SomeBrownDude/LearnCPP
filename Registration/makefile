CPPFLAGS = -std=c++17 -Wall -Wextra -Werror -Wfatal-errors -Wno-sign-compare -Wnon-virtual-dtor -g


OBJS = Users.o User.o Color.o 
TARGET = Users

all: $(TARGET)

$(TARGET): $(OBJS)
	g++ $(CPPFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	g++ $(CPPFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)


# all: Users

# Users: Users.o User.o Color.o
# 	g++ $(CPPFLAGS) Users.o User.o Color.o -o Users

# Users.o: Users.cpp
# 	g++ $(CPPFLAGS) -c Users.cpp

# User.o: User.cpp
# 	g++ $(CPPFLAGS) -c User.cpp


# Color.o: Color.cpp
# 	g++ $(CPPFLAGS) -c Color.cpp

# clean:
# 	rm -f *.o Users
