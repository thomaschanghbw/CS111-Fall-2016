Thomas Chang


The profile report is a file called list.prof. It has the results of pprof with --text, --list=SortedList_insert, and --list=SortedList_lookup. The performance report is a file called raw.perf. I run lab2_list and create raw.perf by setting LD_PRELOAD and CPUPROFILE on the same line that I run the executable. From the topmost folder with my files, pprof is located in ./bin/pprof and the so file is located in ./lib/libprofiler.so.0. The files for gprof are too big to include in the submission, so I do not include them but the file setup above is important in how I reference the files in my Makefile. There are no known bugs in any of the executables. The profile states that strcmp takes the most time, but strcmp is only found within SortedList_insert and SortedList_lookup so I did not consider this a bug.

I split the csv file into two files, lab_2b_list.csv and lab_2b1_list.csv, in order to avoid gnuplot issues with the first two graphs and the last graph. lab_2b1_list.csv handles graphs 1 and 2, and lab_2b_list.csv handles the other 3.

In lab2b_3.png, spin locks, represented by a blue dot, run successfully in every test. They do not seem to appear on the graph however because they are being overlapped by the red crosses. Upon inspection of lab_2b_list.csv, I can see that they are there.


QUESTION 2.3.1 - Cycles in the basic implementation:
Where do you believe most of the cycles are spent in the 1 and 2-thread tests (for both add and list)?  Why do you believe these to be the most expensive parts of the code?
Most of the cycles are spent with threads performing the routines they are supposed to, such as incrementing in the case of add and implementing the list in the case of list. There is also overhead associated with running the timers. 

Where do you believe most of the time/cycles are being spent in the high-thread spin-lock tests?
Most time is likely spent spinning, using up cpu time. There is higher contention with more threads, so it is likely that threads are waiting to acquire a lock.

Where do you believe most of the time/cycles are being spent in the high-thread mutex tests?
Threads are mostly asleep waiting to acquire the lock. Therefore in terms of cpu time most time is spent performing the intended tasks. There is also the costs of waking and sleeping the threads as they wait and acquire locks.

QUESTION 2.3.2 - Execution Profiling:
Where (what lines of code) are consuming most of the cycles when the spin-lock version of the list exerciser is run with a large number of threads?
Why does this operation become so expensive with large numbers of threads?

Running lab2_list for spin locks at 1000 iterations and 12 threads under the profile gives
Total: 1537 samples
    1101  71.6%  71.6%     1101  71.6% ts_lock
     310  20.2%  91.8%      310  20.2% __strcmp_sse42
      63   4.1%  95.9%      226  14.7% SortedList_lookup
      58   3.8%  99.7%      209  13.6% SortedList_insert
       4   0.3%  99.9%        4   0.3% _init
       1   0.1% 100.0%        1   0.1% __clock_gettime
       0   0.0% 100.0%     1537 100.0% __clone
       0   0.0% 100.0%     1537 100.0% pthread_ts
       0   0.0% 100.0%     1537 100.0% start_thread
From these results, we see that the spin lock consumes the most cycles. This operation gets very expensive with large numbers of threads because there is heavy contention for the single lock that protects the list. The threads that do not have the lock just keep spinning waiting for the lock.

QUESTION 2.3.3 - Mutex Wait Time:
Look at the average time per operation (vs # threads) and the average wait-for-mutex time (vs #threads).
Why does the average lock-wait time rise so dramatically with the number of contending threads?
The average lock wait time rises dramatically the more threads there are because there is more contention. This contention increases the likelihood that a thread will be forced to wait for the lock, and if many other threads are waiting for the lock, the wait time will be long.

Why does the completion time per operation rise (less dramatically) with the
number of contending threads?
The completion time rises less dramatically because the combined effects of all threads waiting is not accounted for. There is a counter for each thread, and their timers are all combined for lock wait times.

How is it possible for the wait time per operation to go up faster (or higher) than the completion time per operation?
The wait time can go up faster because there are many threads that are all waiting for the lock. The cummulative totals of all their waiting adds up much faster since only one thread can hold the lock at a time.

QUESTION 2.3.4 - Performance of Partitioned Lists
Explain the change in performance of the synchronized methods as a function of the number of lists.
The more lists there are, the closer the ops/sec approaches a constant value. This is because more and more lists decrease the average length of a list, which in turn decreases the time needed to insert and lookup. This must reach a floor, however, as there are minimum times required to do the list tasks.
Should the throughput continue increasing as the number of lists is further increased?  If not, explain why not.
No, the throughput should not increase because although the average length of the list goes down, there are minimum times that it takes the cpu to handle the instructions. This minimum time is the floor that is approached as the number of lists increases. 
It seems reasonable to suggest the throughput of an N-way partitioned list should be equivalent to the throughput of a single list with fewer (1/N) threads.  Does this appear to be true in the above curves?  If not, explain why not.
This appears to be roughly true based on the graph. It is true because more lists brings down the average list size. Having more lists and more threads is slightly slower due to the costs of implementing threads, putting some threads to sleep if necessary, and determining which list to use. These effects seem minimal, however.
