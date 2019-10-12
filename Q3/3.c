#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define waitState 0
#define onRidePremier 1
#define onRidePoolOne 2
#define onRidePoolFull 3
#define pool 0
#define premier 1
#define serverAvailable 1
#define serverProcessing 2
#define ub_maxWaitTime 3 // + 8
#define ub_rideTime 20   // + 8
#define ub_arrivalTime 5

pthread_mutex_t mutex, mutex1, mutex2;

typedef struct cab
{
    int state, type, travelTime;
} cab;

typedef struct rider
{
    pthread_t tid;
    int idx, arrivalTime, paymentMade;
    clock_t start, now;
} rider;

typedef struct server
{
    pthread_t tid;
    int status, idx;
} server;

const int paymentTime = 2;
int numCabs, numRiders, numServers, cabsState[100000];
rider riders[100000];
server servers[100000];
cab cabs_list[100000];

void *riderProcess(void *arg);
void *paymentServer(void *arg);
int bookCab(int cabType, int maxWaitTime, int rideTime, int riderIdx);
int makePayment();

void *riderProcess(void *arg)
{
    int arrivalTime = ((rider *)arg)->arrivalTime;
    int idx = ((rider *)arg)->idx;
    int cabType = rand() % 2, maxWaitTime = (rand() % ub_maxWaitTime) + 5,
        rideTime = (rand() % ub_rideTime) + 2, cabId;
    sleep(arrivalTime);
    // printf("Rider idx: %d wait: %d cabtype: %d\n", riders[idx].tid, maxWaitTime, cabType);
    cabId = bookCab(cabType, maxWaitTime, rideTime, idx);
    if (cabId == -1)
    {
        printf("Cab allocation timed out for rider with index %d.\n", idx);
        riders[idx].paymentMade = 1;
        return NULL;
    }
    else
    {
        printf("Rider at index %d has been allocated cab with index %d.\n", idx, cabId);
    }

    sleep(rideTime);

    pthread_mutex_lock(&mutex2); // for when 2 riders share the same cab with the same start and end time
    // sem_wait(&mutex);

    if (cabs_list[cabId].state == onRidePoolFull)
        cabs_list[cabId].state = onRidePoolOne;
    else
        cabs_list[cabId].state = waitState;
    pthread_mutex_unlock(&mutex2);
    // sem_post(&mutex);

    int serverIdx = makePayment();
    riders[idx].paymentMade = 1;
    printf("Rider at index %d has made a payment through server with index %d.\n", idx, serverIdx);
    return NULL;
}

void *paymentServer(void *arg)
{
    while (1)
    {
        int tempFlag = 1;
        for (int i = 0; i < numRiders; ++i)
        {
            if (!(riders[i].paymentMade))
            {
                tempFlag = 0;
                break;
            }
        }
        if (tempFlag)
            return NULL;
        int idx = ((server *)arg)->idx;
        if (servers[idx].status == serverProcessing)
        {
            sleep(paymentTime);
            servers[idx].status = serverAvailable;
        }
    }
}

int bookCab(int cabType, int maxWaitTime, int rideTime, int riderIdx)
{
    // pthread_t tid = pthread_self();
    // printf("Threadid in bookCab %lld\n", tid);
    time_t start = time(NULL);
    pthread_mutex_lock(&mutex);
    while (1)
    {
        int i, flag = 0;
        time_t now = time(NULL);
        time_t interval = (now - start);
        // printf("esrgtg %d %d\n", now, start);
        if (interval > maxWaitTime)
        {
            pthread_mutex_unlock(&mutex);
            // printf("%d %lld\n",maxWaitTime, interval);
            return -1;
        }
        // sem_wait(&mutex);
        for (i = 0; i < numCabs; ++i)
        {
            if (cabType == pool && cabs_list[i].type == pool)
            {
                if (cabs_list[i].state == onRidePoolOne)
                {
                    cabs_list[i].state = onRidePoolFull;
                    pthread_mutex_unlock(&mutex);
                    return i;
                }
                else if (cabs_list[i].state == waitState)
                {
                    cabs_list[i].state = onRidePoolOne;
                    pthread_mutex_unlock(&mutex);
                    return i;
                }
            }
            else if (cabType == premier && cabs_list[i].type == premier)
            {
                if (cabs_list[i].state == waitState)
                {
                    cabs_list[i].state = onRidePremier;
                    pthread_mutex_unlock(&mutex);
                    return i;
                }
            }
        }
        // sem_post(&mutex);
    }
    pthread_mutex_unlock(&mutex);
}

int makePayment()
{
    int i, flag = 0;
    while (1)
    {
        pthread_mutex_lock(&mutex1);
        // sem_wait(&mutex);

        for (i = 0; i < numServers; ++i)
        {
            if (servers[i].status == serverAvailable)
            {
                servers[i].status = serverProcessing;
                flag = 1;
                break;
            }
        }
        pthread_mutex_unlock(&mutex1);
        // sem_post(&mutex);

        if (flag)
            return i;
    }
}

void main()
{
    scanf("%d %d %d", &numCabs, &numRiders, &numServers);

    for (int i = 0; i < numCabs; ++i) // initialize cabs
    {
        cabs_list[i].state = waitState;
        if (i < numCabs / 2)
            cabs_list[i].type = pool;
        else
            cabs_list[i].type = premier;
    }
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    // sem_init(&mutex, 0, 1);

    for (int i = 0; i < numRiders; ++i) // intialize riders
    {
        riders[i].idx = i;
        riders[i].arrivalTime = rand() % ub_arrivalTime;
        riders[i].paymentMade = 0;
        pthread_create(&riders[i].tid, NULL, riderProcess, (void *)&riders[i]);
    }
    for (int i = 0; i < numServers; ++i) // initialize servers
    {
        servers[i].idx = i;
        servers[i].status = serverAvailable;
        pthread_create(&servers[i].tid, NULL, paymentServer, (void *)&servers[i]);
    }

    for (int i = 0; i < numRiders; ++i) // Wait for riders
        pthread_join(riders[i].tid, NULL);
    for (int i = 0; i < numServers; ++i) // wait for servers
        pthread_join(servers[i].tid, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    // sem_destroy(&mutex);

    return;
}
