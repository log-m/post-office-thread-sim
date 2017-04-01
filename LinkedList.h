#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
using namespace std;

struct node
{
    int val; //Customer number, worker number, or job
    node * next;
};
class LinkedList
{
    public:
        LinkedList();
        virtual ~LinkedList();
        bool enqueue(int item);
        int dequeue();

    protected:

    private:
        node * head;
};

#endif // LINKEDLIST_H
