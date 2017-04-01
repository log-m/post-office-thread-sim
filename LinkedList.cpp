#include "LinkedList.h"
#include <stdio.h>
LinkedList::LinkedList()
{
   head = NULL; //Initialize with an empty head node
}

bool LinkedList::enqueue(int item)
{
    //node queueNode = {item, NULL};
    //fprintf(stderr, "node value = %d\n", queueNode.val);

    if(head == NULL)
    {
        //cout << "head is null\n";
        head = new node;
        head->val = item;
        head->next = NULL;
        //cout << "finished\n";
        return true;
    }
    else
    {
        //node * newNode;

        node * helpPtr;
        helpPtr = head;
        while(helpPtr->next != NULL) //Loop until pointing to last node in the list
            helpPtr = helpPtr->next;
        helpPtr->next = new node; //Add new node at the end with the enqueued value
        helpPtr->next->val = item;
        helpPtr->next->next = NULL;
        //fprintf(stderr, "Value queued: %d\n", helpPtr->next->val);
        //newNode->next = NULL;
        return true;
    }
}

int LinkedList::dequeue()
{
    //fprintf(stderr, "About to dequeue\n");
    node * headptr = head;
    int item = head->val;
    //fprintf(stderr, "Dequeued item: %d\n", item);
    head = head->next; //Move head forward, return value in previous head
   /* if(head == NULL)
    {
        fprintf(stderr, "head is null\n");
    }
    else
    {
        fprintf(stderr, "new head value: %d\n", head->val);
    }*/
    delete headptr; //garbage collection
    return item;
}
LinkedList::~LinkedList()
{
    //dtor
}
