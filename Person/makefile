CPPFLAGS = -std=c++17 -Wall -Wextra -Werror -Wfatal-errors -Wno-sign-compare -Wnon-virtual-dtor -g


main: main.o person.o
	g++ $(CPPFLAGS) main.o person.o -o main

main.o: main.cpp
	g++ $(CPPFLAGS) -c main.cpp

person.o: person.cpp person.h
	g++ -c person.cpp

clean: 
	rm *.o