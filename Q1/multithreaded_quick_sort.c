#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

typedef long long ll;
ll arr[100000];

typedef struct LR
{
    ll l;
    ll r;
} LR;

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

void *quickSort(void *boundary)
{
    int left = ((LR *)boundary)->l, right = ((LR *)boundary)->r;

    if (left >= right)
        return NULL;
    if (right - left < 5)
    {
        insertionSort(left, right);
        return NULL;
    }
    pthread_t t1, t2;
    LR *boundary1 = (LR *)malloc(sizeof(LR)), *boundary2 = (LR *)malloc(sizeof(LR));
    int part_idx = partition(left, right);
    boundary1->l = left;
    boundary1->r = part_idx - 1;

    boundary2->l = part_idx + 1;
    boundary2->r = right;
    pthread_create(&t1, NULL, quickSort, (void *)boundary1);
    pthread_create(&t2, NULL, quickSort, (void *)boundary2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return NULL;
}

int main()
{
    int n, i;
    scanf("%d", &n);
    for (i = 0; i < n; ++i)
        scanf("%lld", &arr[i]);

    pthread_t t;
    LR *boundary = (LR *)malloc(sizeof(LR));
    boundary->l = 0;
    boundary->r = n - 1;

    clock_t startTime = clock();
    pthread_create(&t, NULL, quickSort, (void *)boundary);
    pthread_join(t, NULL);
    clock_t endTime = clock();
    // printf("%lf\n", ((double)(endTime - startTime)) / CLOCKS_PER_SEC);

    for (i = 0; i < n; i++)
        printf("%lld ", arr[i]);
    printf("\n");

    return 0;
}
