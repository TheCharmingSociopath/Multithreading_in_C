# Question 1

- In this question, I have written 3 variants of quick sort.
- First is the regular quick sort
- Second if concurrent quick sort
- Third is multithreaded quick sort

## Regular Quick Sort

- Implimented the regular quick sort algorithm, by choosing the center element as the pivot. In this implimentation, the array is divided into left and right subarray, and both of these are are recursively sorted by quick sort. Here, each subarray is sorted one by one, in the order described by tree formed by the function calls, i.e., only one subarray is sorted at one time.

## Concurrent Quick Sort

- In this implimentation of quick sort, each partition of the array is sorted in a separate process. There is a shared memory location where the array is stored. Each process accesses their part of the array and sorts it, after which it ends, and the higher array is then sorted in a similar manner.

## Multithreaded Quick Sort

- In this implimentation of quick sort, each partition of the array is sorted as a separate thread.

## Comparison

| Number of test cases | Regular | Concurrent | Multithreaded |
|:---------------------|:--------|:-----------|:--------------|
|10                    |0.000009 |0.000561    |0.000298       |
|100                   |0.000056 |0.000438    |0.006295       |
|1000                  |0.000810 |0.000476    |0.080666       |
|10000                 |0.003372 |0.001223    |0.473630       |
|100000                |0.020793 |--------    |--------       |

## Analysis

- We can see that Concurrent and Multithreaded are in general larger than regular quick sort. The reason for this is the overhead due to process and thread creation and execution. We can also see that the time for concurrent is less than that of regular quick sort after the size of input reaches about 1000. This is because the speedup acheived by multiprocessing outweighs the overheads mentioned previously.
