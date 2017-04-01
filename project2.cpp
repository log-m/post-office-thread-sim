#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "LinkedList.h"
#include <time.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <assert.h>

using namespace std;
//function declarations
void doStamps();
void doLetter();
void doPackage();
void* customer(void *);
void* postWorker(void *);

//global variables
    sem_t mutex1;
    sem_t mutex2;
    sem_t capacity;
    sem_t cust_ready;
    sem_t worker_ready;
    sem_t finished [50];
    sem_t scale;
    sem_t line;
    sem_t service;
    sem_t goAhead;
     int served = 0; //counter for workers to know when to join

LinkedList customers; //Pretty much just queues. This one is for customer numbers
LinkedList jobs; //For job IDs
LinkedList workers; //For worker numbers
#define CUST 50
int main()
{
    pthread_t customers[CUST] ={0}; //Used to create the customer threasd
    pthread_t employee[3] = {0}; //Used to create employee threads
    int thr;
    //Initializing semaphores
    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&capacity, 0, 10);
    sem_init(&cust_ready, 0, 0);
    sem_init(&worker_ready, 0, 0);
    sem_init(&scale, 0, 1);
    sem_init(&line, 0, 1);
    sem_init(&service, 0, 0);
    sem_init(&goAhead, 0, 0);
    //
    int *number; //Used to pass argument along with thread into functions
    void * end; //An empty void * variable to use as argument for pthread_join

    srand (time(NULL)); //Seed RNG before threading so that each thread gets different values for their job
    fprintf(stderr, "Simulating Post Office with 50 customers and 3 postal workers\n\n");
    for(int count = 0; count < 50; count++) //Initialize finished[] semaphores
    {
        sem_init(&finished[count], 0, 0);
    }
//Creating threads for customers and workers
    for(int count = 0; count < CUST; count++){
        cout << "Customer " << count << " created" << endl;
        number = (int*)malloc(sizeof(int)); //This + assert(number) used to ensure there is no null references during runtime
        assert(number);
        *number = count;
        thr = pthread_create(&customers[count], NULL, customer, number);
    }
    for (int count = 0; count < 3; count++){
        cout << "Postal worker " << count << " created" << endl;
        number = (int*)malloc(sizeof(int));
        assert(number);
        *number = count;
        thr = pthread_create(&employee[count], NULL, postWorker,  number);
    }

    //loops for joining pthreads.
    for (int count = 0; count < CUST; count++)
    {
        pthread_join(customers[count], &end);
        fprintf(stderr, "Joined customer %d\n", count);
    }
     for (int count = 0; count < 3; count++)
    {
        pthread_join(employee[count], &end);
        fprintf(stderr, "Joined postal worker %d\n", count);
        //exit(0);
    }
    //exit(0);

    exit(0);
    return 0;
}


void* customer(void* cust_num)
{

    int * numptr = (int*)cust_num; //Casts the pointer back to int and brings the customer number in
    int c_num = *numptr;
    int server; //Holds worker number that is helping them
    //fprintf(stderr, "Customer %d\n", c_num);
    int job = rand() % 3 + 1; //Generates int between 1-3

    sem_wait(&capacity); //Wait for open space in office

    fprintf(stderr, "Customer %d enters post office\n", c_num);//enter_office();

    sem_wait(&mutex2);//Waiting to be 'first to the desk', with an available worker and no worker taking a job
    sem_wait(&worker_ready);
   // fprintf(stderr, "Customer %d queueing\n", c_num);
    sem_wait(&line);

    //Put customer number and job in LinkedLists to be read by postal workers
    customers.enqueue(c_num);
    jobs.enqueue(job);

    sem_post(&line); //Letting someone else come to the desk/take a job, signal that a customer is ready for service
    sem_post(&cust_ready);
    sem_wait(&service); //Wait for worker to put their number in a queue so they know who they are working with
    server = workers.dequeue();
    switch(job){ //Print statements based on job
        case 1: fprintf(stderr, "Customer %d asks postal worker %d to buy stamps\n", c_num, server);
        break;
        case 2: fprintf(stderr, "Customer %d asks postal worker %d to mail a letter\n", c_num, server);
        break;
        case 3: fprintf(stderr, "Customer %d asks postal worker %d to mail a package\n", c_num, server);
        break;
    }
    sem_post(&goAhead);
    sem_post(&mutex2); //Next customer can do their stuff


    sem_wait(&finished[c_num]); //Wait for worker to do the job
    switch(job){
        case 1: fprintf(stderr, "Customer %d finished buying stamps\n", c_num);
        break;
        case 2: fprintf(stderr, "Customer %d finished mailing a letter\n", c_num);
        break;
        case 3: fprintf(stderr, "Customer %d finished mailing a package\n", c_num);
        break;
    }
    sem_post(&capacity); //Add a space inside the office

    fprintf(stderr, "Customer %d leaves post office\n", c_num);//leave();
    pthread_exit(0);
}

void* postWorker(void* work_num)
{
    int *numptr = (int *) work_num; //Casting work_num back to a tangible int, w_num
    int w_num = *numptr;
    int curr_cust; //customer being worked with
    int job; //job of customer

    while(true) //Workers work until there are no more customers
    {

        sem_post(&worker_ready);
        //fprintf(stderr, "%d Ready to serve\n", w_num);
        sem_wait(&cust_ready);//Wait on next customer
        sem_wait(&mutex1);//Wait for other workers to be done taking jobs so there are no mixups

        sem_wait(&line);
        //Obtained customer information from LinkedLists
        curr_cust = customers.dequeue();
        job = jobs.dequeue();
        //Add worker number to linked list so customer can see it
        workers.enqueue(w_num);

        //fprintf(stderr, "job obtained\n");
        sem_post(&line); //Allow next action by other threads
        //fprintf(stderr, "Worker %d got cust %d and job %d\n", w_num, curr_cust, job);
        fprintf(stderr, "Postal worker %d serving customer %d\n", w_num, curr_cust);
        sem_post(&service);
        sem_post(&mutex1);
        sem_wait(&goAhead);
        switch(job){ //Sleep based on job being done
            case 1:
                doStamps();
                break;
            case 2: doLetter();
                break;
            case 3: //If package must be sent, wait for scales to be available, use them to do job, free them up
                sem_wait(&scale);
                fprintf(stderr, "Scales in use by postal worker %d\n", w_num);
                doPackage();
                sem_post(&scale);
                fprintf(stderr, "Scales released by postal worker %d\n", w_num);
                break;}
        fprintf(stderr, "Postal worker %d finished serving customer %d\n", w_num, curr_cust);
        served++; //Increment amount of customers served
        sem_post(&finished[curr_cust]);//Let customer know it can go
        if(served >= 48) //If number is greater than or equal 48, the other workers are doing the last remaining jobs, so this worker can join
            break;
    }
    pthread_exit(0);

    //return NULL;
}

void doStamps()
{
   // fprintf(stderr, "about to stamps sleep\n");
    sleep(1);
}

void doLetter() //Unix sleep does not allow fractions of a second, but nanosleep does.
{
     struct timespec ts;
    ts.tv_sec = 1.5; //Time to wait in seconds
    ts.tv_nsec = 1500000000;//Time to wait in nanoseconds
    //fprintf(stderr, "about to nanosleep\n");
    nanosleep(&ts, NULL);
}

void doPackage()
{
    //fprintf(stderr, "about to package sleep\n");
    sleep(2);
}
