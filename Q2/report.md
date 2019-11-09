# Question 2

## Assumptions and Implimentation

- All the ranges for random generation of constraints are defined as a macro in #define section
- Robot Chef, Students and tables are all threads and have a struct associated with them that describes their ctates.
- Delay of 1 second when student is eating biryani.

## Code Structure

- Functions:
  - robotThread(): This is the function that is initially executed by all the robo chef processes. It runs until all the students have been served. [numStudentsServed == numStudents] It sleeps for the preparation time, which is randomly generated during initialization. After that it calls biryaniReady() function.
  - biryaniReady(): This function runs while the number of vessels remaining is > 0, and loads all the vessels onto the tables when they are empty. Here, the function has to loop through tables array, to check which one is in notServing mode, and this is a critical sections, so mutex is used here. This function sleeps for a small time, 0.5 sec, as a representation of loading time.
  - tableThread(): This function runs until all the students have been served with biryani. It is stuck in a while loop while table is not in the serving mode (busy waiting). After that, it randomly allocates a certain number of slots to the variable numSlots, and calls readyToServeTable().
  - readayToServeTable(): This function runs until either all slots have been assigned or the table leaves the serving mode.
  - studentThread(): This function represents a student. It initially sleeps for arrivalTime amount of time to represent the arrival of student at a particular time. Then it increases the value of numStudentsWaiting by one, and calls waitForSlot(), which represents student waiting for a slot. Then this function reduces the number of portions in the slot assigned, and increases the number of students served. It then checks if the number of portions remaining in the allocated table is 0, and handles it accordingly (change state to tableNotServing). Then it calls studentInSlot function, and then returns.
  - waitForSlot(): This function loops though the array tables to find a table in serving mode with an empty slot. This then returns the index of the table assigned. This is also a critical section, as 2 students should not be alloted the same slot, and hence mutex is used.
  - studentInSlot(): This function represents student being served with biryani and eating it. Delay of 1 second has been assumed.

## Flow in main()

- Main takes numRobos, numStudets and numTables as input, initializes 3 mutexes, the robo threads, student threads and table threads. It then waits for all the threads to finish executing, and then destroys the mutexes and returns.
