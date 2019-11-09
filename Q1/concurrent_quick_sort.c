#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

typedef long long ll;
ll *arr;
key_t key = IPC_PRIVATE;

void swap(ll *a, ll *b)
{
    ll t = *a;
    *a = *b;
    *b = t;
    return;
}

int partition(int left, int right)
{
    int p_idx = (left + right) / 2;
    ll pivot = arr[p_idx];
    for (int i = left; i <= right; ++i)
    {
        if (arr[i] < pivot)
        {
            swap(&arr[left], &arr[i]);
            if (left == p_idx)
                p_idx = i;
            ++left;
        }
    }
    swap(&arr[left], &arr[p_idx]);
    return left;
}

void insertionSort(int left, int right)
{
    for (int i = left + 1; i <= right; ++i)
    {
        ll key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
    return;
}

void quickSort(int left, int right)
{
    if (left >= right)
        return;
    if (right - left < 5)
    {
        insertionSort(left, right);
        return;
    }
    pid_t p1, p2;
    int part_idx = partition(left, right), status;
    if ((p1 = fork()) < 0) //ERROR
    {
        perror("Error while forking the process.\n");
        exit(0);
    }
    else if (p1 > 0) //PARENT
    {
        if ((p2 = fork()) < 0)
        {
            perror("Error while forking the process.\n");
            exit(0);
        }
        else if (p2 == 0) //Second child
        {
            quickSort(part_idx + 1, right);
            exit(0);
        }
    }
    else //First Child
    {
        quickSort(left, part_idx - 1);
        exit(0);
    }
    waitpid(p1, &status, 0);
    waitpid(p2, &status, 0);

    return;
}

int main()
{
    int n, i;
    scanf("%d", &n);
    int shmid = shmget(key, sizeof(ll) * n, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("Failed at shmget");
        exit(0);
    }
    arr = shmat(shmid, 0, 0);
    if (arr == (void *)-1)
    {
        perror("Failed at shmat");
        exit(0);
    }
    for (i = 0; i < n; ++i)
        scanf("%lld", &arr[i]);

    clock_t startTime = clock();
    quickSort(0, n - 1);
    clock_t endTime = clock();
    // printf("%lf\n", ((double)(endTime - startTime)) / CLOCKS_PER_SEC);

    for (i = 0; i < n; i++)
        printf("%lld ", arr[i]);
    printf("\n");

    return 0;
}
