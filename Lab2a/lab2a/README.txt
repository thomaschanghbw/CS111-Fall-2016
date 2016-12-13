Description and notes on included files:
lab2_add and lab2_list work according to the spec and implement different locks and yield options. They appear to work with no known errors. No modifications were made to SortedList.h that was given. SortedList.c implementations of list functions have no known bugs. The Makefile compiles both executables. Check includes all the test scripts, with the --correct option removed if applicable. Graph creates the required graphs using gnuplot. Tarball creates the tarball, and clean deletes the executables. I added many test cases to increase the accuracy of the graphs. For example, lab2_list-1.png graph has double the test cases involved. lab2_add.csv and lab2_list.csv have the results of one round of test cases.

Questions
2.1.1 - causing conflicts:
Why does it take many iterations before errors are seen?
Why does a significantly smaller number of iterations so seldom fail?

Answer: More iterations greatly increases the chance that threads will trample over each other's data accesses and settings. Having a very small number of iterations decreases the liklihood of errors because the chance of one thread updating a variable in a critical section after another thread reads the variable, but before it writes to the variable, is small.

2.1.2 - cost of yielding:
Why are the --yield runs so much slower?  Where is the additional time going?  Is it possible to get valid per-operation timings if we are using the --yield option?  If so, explain how.  If not, explain why not.

Answer: Yields are slower because switching between threads or processes adds overhead. For example, data must be transfered to and from registers with every context switch. It would be optimal to continue running until the scheduler stops the thread instead of arbitrarily conceding cpu time when it is unnecessary. We could attempt to get more accurate per-operation timing by starting the clock when the thread resumes and when it yields, but we would have to store these timestamps each time so it adds a bit of overhead.

2.1.3 - measurement errors:
Why does the average cost per operation drop with increasing iterations?
If the cost per iteration is a function of the number of iterations, how do we know how many iterations to run (or what the “correct” cost is)?

Answer: The actual cost per operation does not decrease, but the average does because the measurements of more and more iterations bring down the influence of operations that are not directly involved in incrementing or decrementing the counter. For example, the costs of creating all the threads, allocating a stack for each thread, and updating registers for each thread weighs heavily on the results of the timer. When there are a lot of iterations, they drown out the effect these startup costs have. The true cost is the cost that we converge to as the number of iterations approaches infinity. We can find this by finding the negative logarithmic function created by the data points.

2.1.4 - costs of serialization:
Why do all of the options perform similarly for low numbers of threads?
Answer: For a low number of threads, there is less concurrency and so there is less likely to be contention for the lock. At lower number of threads, the implementation of the lock does not matter as much as it does when there are more threads. 
Why do the three protected operations slow down as the number of threads rises?
Answer: More threads means more work is being done concurrently. If all threads are trying to operate within the same critical section, there is more contention for the lock as a result.
Why are spin-locks so expensive for large numbers of threads?
Answer: Spin locks are expensive with large numbers of threads because they cause wastage of valuable cpu time. A thread might use time spinning when it could be doing a useful operation.

2.2.1 - scalability of Mutex
Compare the variation in time per protected operation vs the number of threads (for mutex-protected operations) in Part-1 and Part-2, commenting on similarities/differences and offering explanations for them.

Answer: Increasing the threads raises the cost per operation in both Part 1 and Part 2. The rate of increase appears to decrease and the costs per operation begins to level off. Mutex locks offer the best performance in both, as putting the waiting threads to sleep is better for performance, especially at larger thread numbers, than using cpu time to wait for the lock.

scalability of spin locks
Compare the variation in time per protected operation vs the number of threads for Mutex vs Spin locks, commenting on similarities/differences and offering explanations for them.

Answer: Mutex locks perform better than spinlocks because they do not waste cpu time spinning. When looking at the raw completion times and the raw cost per operations, mutex locks perform vastly better than spin locks. The length adjusted values underscore how much better mutex locks actually perform. Putting threads to sleep is much more preferable to constantly checking if a lock is available.
