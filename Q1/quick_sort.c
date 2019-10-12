#include <stdio.h>
#include <time.h>

typedef long long ll;
ll arr[100000];

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
    // for (int i = 0; i < 10; i++)
    //     printf("%lld ", arr[i]);
    // printf("\n");
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
    int part_idx = partition(left, right);
    // printf("%d\n", part_idx);
    quickSort(left, part_idx - 1);
    quickSort(part_idx + 1, right);
    return;
}

int main()
{
    int n, i;
    scanf("%d", &n);
    for (i = 0; i < n; ++i)
        scanf("%lld", &arr[i]);

    clock_t startTime = clock();
    quickSort(0, n - 1);
    clock_t endTime = clock();
    printf("%lf\n", ((double)(endTime - startTime)) / CLOCKS_PER_SEC);

    // for (i = 0; i < n; i++)
    //     printf("%lld ", arr[i]);
    // printf("\n");

    return 0;
}
