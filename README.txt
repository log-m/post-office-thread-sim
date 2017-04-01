COMPILATION:
	g++ -pthread -o proj2 project2.cpp LinkedList.cpp
RUN:
	./proj2

NOTES:

	Project makes uses of a LinkedList class that I wrote. This probably should not be a problem, but make sure LinkedList.cpp and LinkedList.h are in the same directory as project2.cpp.