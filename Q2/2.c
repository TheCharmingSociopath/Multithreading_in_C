#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define tableServing 1
#define tableNotServing 0
#define minArrival 1
#define maxArrival 10
#define minPrepareTime 2
#define minNumVesselsPrepared 1
#define minVesselCapacity 25
#define maxPrepareTime 5
#define maxNumVesselsPrepared 10
#define maxVesselCapacity 50
#define minNumSlots 1
#define maxNumSlots 10

int numRobo, numTable, numStudents, numWaitingStudents = 0, numStudentsServed = 0;
pthread_mutex_t mutex1, mutex2, mutex3;

typedef struct robot
{
    pthread_t tid;
    int idx, prepareTime, numVesselsPrepared, vesselCapacity;
} robot;

typedef struct table
{
    pthread_t tid;
    int idx, containerStatus, servingMode, numSlots;
} table;

typedef struct student
{
    pthread_t tid;
    int idx, arrivalTime, served;
} student;

student students[100000];
table tables[100000];
robot robots[100000];

void *studentThread(void *arg);
void *tableThread(void *arg);
void *robotThread(void *arg);
void biryaniReady(int roboIdx, int numVessels, int vesselCapacity); //load vessels into container
void readyToServeTable(int tableIdx);                               //return when no waiting student or all serving slots full
int waitForSlot();                                                  // run till one servingMode is tableServing
void studentInSlot(int tableIdx, int studentIdx);                   // let serving table know student in slot
int randomGen(int a, int b);

void *robotThread(void *arg)
{
    while (1)
    {
        if (numStudentsServed == numStudents)
            return NULL;
        int idx = ((robot *)arg)->idx;
        robots[idx].numVesselsPrepared = randomGen(minNumVesselsPrepared, maxNumVesselsPrepared);
        printf("Robot Chef %d is preparing %d vessels of Biryani.\n", idx, robots[idx].numVesselsPrepared);
        sleep(robots[idx].prepareTime);
        robots[idx].vesselCapacity = randomGen(minVesselCapacity, maxVesselCapacity);
        printf("Robot Chef %d has prepared %d vessels of Biryani. Waiting for all the vessels to be emptied to resume cooking.\n", idx, robots[idx].numVesselsPrepared);
        biryaniReady(idx, robots[idx].numVesselsPrepared, robots[idx].vesselCapacity);
        printf("All the vessels prepared by Robot Chef %d are emptied. Resuming cooking now.\n", idx);
    }
}

void biryaniReady(int roboIdx, int numVessels, int vesselCapacity)
{
    while (numVessels > 0)
    {
        pthread_mutex_lock(&mutex1);

        for (int i = 0; i < numTable && numVessels > 0; ++i)
        {
            if (tables[i].servingMode == tableNotServing)
            {
                printf("Robot Chef %d is refilling Serving Container of Serving Table %d.\n", roboIdx, i);
                tables[i].containerStatus = vesselCapacity;
                tables[i].servingMode = tableServing;
                --numVessels;
                sleep(0.5);
                printf("Serving Container of Table %d is refilled by Robot Chef %d, the table is resuming serving now.\n", i, roboIdx);
            }
        }
        pthread_mutex_unlock(&mutex1);
    }
}

void *tableThread(void *arg)
{
    while (1)
    {
        if (numStudentsServed >= numStudents)
            return NULL;
        int idx = ((table *)arg)->idx;
        while (tables[idx].servingMode == tableNotServing)
            ;
        // printf("Serving table %d entering Serving Phase.\n", idx);
        tables[idx].numSlots = randomGen(minNumSlots, maxNumSlots);
        printf("feknjfnj %d\n", numWaitingStudents);
        // printf("Serving Table %d is ready to serve with %d slots.\n", idx, tables[idx].numSlots);
        readyToServeTable(idx);
    }
}

void readyToServeTable(int tableIdx)
{
    printf("%d %d\n", tables[tableIdx].numSlots, tables[tableIdx].servingMode);
    while (tables[tableIdx].numSlots > 0 && tables[tableIdx].servingMode == tableServing)
        ;
}

void *studentThread(void *arg)
{
    int idx = ((table *)arg)->idx;
    sleep(students[idx].arrivalTime);
    printf("Student %d has arrived.\n", idx);

    pthread_mutex_lock(&mutex2);
    ++numWaitingStudents;
    // pthread_mutex_unlock(&mutex1);

    printf("Student %d is waiting to be allocated a slot on the serving table.\n", idx);

    int tableIdx = waitForSlot();
    tables[tableIdx].containerStatus -= 1;
    ++numStudentsServed;
    if (tables[tableIdx].containerStatus == 0)
    {
        tables[tableIdx].servingMode = tableNotServing;
        printf("Serving Container of Table %d is empty, waiting for refill.\n", tableIdx);
    }

    // pthread_mutex_lock(&mutex1);
    --numWaitingStudents;
    pthread_mutex_unlock(&mutex2);

    printf("Student %d assigned a slot on the serving table %d and waiting to be served.\n", idx, tableIdx);
    studentInSlot(tableIdx, idx);
}

int waitForSlot()
{
    while (1)
    {
        pthread_mutex_lock(&mutex3);

        for (int i = 0; i < numTable; ++i)
        {
            if (tables[i].numSlots > 0 && tables[i].servingMode == tableServing)
            {
                tables[i].numSlots -= 1;
                pthread_mutex_unlock(&mutex1);
                return i;
            }
        }
        pthread_mutex_unlock(&mutex3);
    }
}

void studentInSlot(int tableIdx, int studentIdx)
{
    sleep(1);
    printf("Student %d on serving table %d has been served.\n", studentIdx, tableIdx);
}

void main()
{
    scanf("%d %d %d", &numRobo, &numTable, &numStudents);
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);

    for (int i = 0; i < numRobo; ++i)
    {
        robots[i].idx = i;
        robots[i].prepareTime = randomGen(minPrepareTime, maxPrepareTime);
        robots[i].numVesselsPrepared = randomGen(minNumVesselsPrepared, maxNumVesselsPrepared);
        robots[i].vesselCapacity = randomGen(minVesselCapacity, maxVesselCapacity);
        pthread_create(&robots[i].tid, NULL, robotThread, (void *)&robots[i]);
    }

    for (int i = 0; i < numTable; ++i)
    {
        tables[i].idx = i;
        tables[i].containerStatus = 0;
        tables[i].servingMode = tableNotServing;
        pthread_create(&tables[i].tid, NULL, tableThread, (void *)&tables[i]);
    }

    for (int i = 0; i < numStudents; ++i)
    {
        students[i].idx = i;
        students[i].arrivalTime = randomGen(minArrival, maxArrival);
        students[i].served = 0;
        pthread_create(&tables[i].tid, NULL, tableThread, (void *)&students[i]);
    }

    for (int i = 0; i < numRobo; ++i) // Wait for robots
        pthread_join(robots[i].tid, NULL);
    for (int i = 0; i < numStudents; ++i) // wait for students
        pthread_join(students[i].tid, NULL);
    for (int i = 0; i < numTable; ++i) // wait for tables
        pthread_join(tables[i].tid, NULL);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    printf("Simulation Over.\n");
    return;
}

int randomGen(int a, int b) // a < b
{
    return (rand() % (b - a)) + a;
}
