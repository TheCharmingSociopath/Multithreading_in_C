# Question 3

## Assumnptions and implimentation

- maxWaitTime is random between 5 and 7
- rideTime is random between 2 and 21
- Each cab is a struct
- Each rider is a process and the corresponding attributes are stored in a array of struct.
- Each server is a process and the corresponding attributes are stored in a array of struct.

## Code Structure

- Functions:
  - riderProcess(): This is the function representing a rider. This function is executed by the thread representing a rider. This function initially sleeps for the period defined by arrivalTime. Then, it attempts to book a cab using bookCab(). If the return value is -1, it implies that the function timed out, i.e., time taken has exceeded maxWaitTime(). In this case, the rider prints an appropriate response and exits. Otherwise, it prints an appropriate message and then sleeps for the rideTime period. After this, the state of the cab is appropriately changed, and makePayment() is called. State change of cab should be handled as a critical section for the case when 2 riders share the same cab with the same start and end time. It is handled appropriately using mutex. After payment is made, the process exits.
  
  - paymentServer(): This function represents a server. This function executes till all the riders have finished a ride, i.e., they have made a payment or exited with a timeout. If the server is in processing mode, the function sleeps for an appropriate time.

  - bookCab(): This function times the execution from when called, and returns if it exceeds maxWaitTime. Otherwise, it books a cab when it is available, and returns it's index in the cabs array. This should be treated as a critical section of the code, as it is accessing a global variable that might be accessed by other threads at the same time. This has been handled appropriately with mutex. Only one thread will enter this critical section at a time.

  - makePayment(): This function allocates a server to the rider for processing of payment. (Return value is the index of the server). This function also iterates through the array containing all the servers, and hence multiple processes might try to change it's values at once. Therefore, it should be appropriately handled with mutex, so that only one process can enter the critical section at a time.

## Flow in main()

- Number of cabs, number of riders and number of servers is taken as input.
- Cabs are initialized, their types assigned.
  - ASSUMPTION: First half cabs are pool type, rest are premier.
- Mutex is initialised.
- Riders are initialised. A process corresponding to each rider is created, and other attributes, such as arrivalTime (randomly generated) are stored in a struct.
- Servers are initialised. A process corresponding to each server is created, and other attributes, such as server state are initialized and stored in a struct.
- Then, we wait for all the threads to finish processing, and in the end, destroy the mutexes created initially.
