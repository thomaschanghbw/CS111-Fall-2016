#include <stdio.h> //perror
#include <getopt.h> //getopt_long
#include <stdlib.h> //atoi, malloc
#include <pthread.h>
#include <time.h> //clock_gettime
#include <sched.h> //sched_yield
#include <string.h> //strcpy

#define true 1

//Global mutex
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
int excl_lock = 0;

//Options
int syncOption = 0;
char syncSymbol;
int opt_yield = 0;
char addOptionName[20];
typedef struct
{
  int numberOfThreads;
  int numberOfIterations;
  int yieldOptionReceived;
} OptionsForAdd;

//Pthread argument
typedef struct
{
  long long counterToAddTo;
  int iterations;
  int yieldOption;
} PthreadArgs;

//Add Function
void add(long long *pointer, long long value);

//Parses options, modifies the OptionsForAdd struct
void optionParser(int argc, char* argv[], OptionsForAdd* opts);

//Adds with the number of threads and iterations specified
void counterAdder(OptionsForAdd* opts);

void error(char* errmessage);

//Pthread Functions
void* pthread_func(void* pthreadArgs);
void* pthread_mutex(void* pthreadArgs);
void* pthread_ts(void* pthreadArgs);
void* pthread_cs(void* pthreadArgs);

//Lock implementations
void ts_lock(int* excl_lock);
void ts_unlock(int* excl_lock);
void cs_lock(int* excl_lock);
void cs_unlock(int* excl_lock);
/******************************
	 MAIN
*******************************/
int main(int argc, char* argv[])
{
  strcpy(addOptionName, "add");
  //Parse Options
  OptionsForAdd optionsReceived;
  optionParser(argc, argv, &optionsReceived);

  //Do Adding
  opt_yield = optionsReceived.yieldOptionReceived;
  if(!syncOption)
    strcat(addOptionName, "-none");
  counterAdder(&optionsReceived);
}

/******
Add Functions
 ******/
void add(long long *pointer, long long value)
{
  long long sum = *pointer + value;
  if(opt_yield)
      sched_yield();
  *pointer = sum;
}

void counterAdder(OptionsForAdd* opts)
{
  pthread_t* threadsForAdd = (pthread_t*)malloc(opts->numberOfThreads * sizeof(pthread_t));
  PthreadArgs pthreadArgs;
  pthreadArgs.counterToAddTo = 0;
  pthreadArgs.yieldOption = opts->yieldOptionReceived;
  pthreadArgs.iterations = opts->numberOfIterations;
  int i;
  long totalNumOps = 0;
  //Setup time
  struct timespec start, stop;
  long totalThreadTime = 0;
  long avgOperationTime = 0;
  //Determine which pthread function to use
  void* (*func_pointer)(void*);
  if(syncOption)
    {
      if(syncSymbol=='m'){
	func_pointer=pthread_mutex;
	  }
      else if(syncSymbol=='s'){
	func_pointer=pthread_ts;
      }
      else if(syncSymbol=='c'){
	func_pointer=pthread_cs;
      }
    }
  else
    {
      func_pointer= pthread_func;
    }
  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
    error("Could not call clock_gettime");
  for(i=0; i<opts->numberOfThreads; i++)
    {
      if(pthread_create(&threadsForAdd[i], NULL, func_pointer, (void*)&pthreadArgs) != 0)
	error("Failed to create threads");
    }
  for(i=0; i<opts->numberOfThreads; i++)
    {
      if(pthread_join(threadsForAdd[i], NULL)!=0)
	error("Failed to join threads");
    }
  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
    error("Could not call clock_gettime");
  free(threadsForAdd);
  //Calculations from data
  totalThreadTime = (1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
  totalNumOps = opts->numberOfThreads * opts->numberOfIterations * 2;
  avgOperationTime = totalThreadTime / totalNumOps;
    if(syncOption) {
      strcat(addOptionName, "-");
      strcat(addOptionName, &syncSymbol);
  }
  printf("%s,%i,%i,%ld,%ld,%ld,%lld\n", addOptionName, opts->numberOfThreads, opts->numberOfIterations, totalNumOps, totalThreadTime,avgOperationTime, pthreadArgs.counterToAddTo);

}



void optionParser(int argc, char* argv[], OptionsForAdd* opts)
{
  opts->numberOfThreads = 1;
  opts->numberOfIterations = 1;
  opts->yieldOptionReceived = 0;

  while(1)
    {
      int optReceived;
      int optionIndex = 0;
      static struct option longopts[] = {
	{"threads", required_argument, 0, 0},
	{"iterations", required_argument, 0, 0},
	{"yield", no_argument, 0, 0},
	{"sync", required_argument, 0, 0}
      };
      optReceived = getopt_long(argc, argv, "", longopts, &optionIndex);
      if(optReceived == -1)
	break;
      if(optReceived == 0)
	{
	  if(longopts[optionIndex].name == "threads")
	    {
	      opts->numberOfThreads = atoi(optarg);
	    }
	  if(longopts[optionIndex].name == "iterations")
	    {
	      opts->numberOfIterations = atoi(optarg);
	    }
	  if(longopts[optionIndex].name == "yield")
	    {
	      // addOptionName = (char*)malloc(sizeof("add-yield"));
	       strcpy(addOptionName, "add-yield");
	      //	      addOptionName = "add-yield";
	      opts->yieldOptionReceived = true;
	    }
	  if(longopts[optionIndex].name == "sync")
	    {
	      syncOption = 1;
	      if((*optarg)!='m' && (*optarg)!='s' && (*optarg)!='c')
		{
		fprintf(stderr, "Invalid argument to sync");
		exit(1);
		}
	      else
		syncSymbol = *optarg;
	    }
	}
    }
}

void error(char* errmessage)
{
  perror(errmessage);
  exit(1);
}

/***************
Pthread functions
 ***************/
void* pthread_func(void* pthreadArgs)
{
  int i;
  PthreadArgs* pthreadArguments= (PthreadArgs*)pthreadArgs;
  for(i=0; i<pthreadArguments->iterations; i++)
    {
      add(&(pthreadArguments->counterToAddTo), 1);
      add(&(pthreadArguments->counterToAddTo), -1);
    }
  return NULL;
}
void* pthread_mutex(void* pthreadArgs)
{
  int i;
  PthreadArgs* pthreadArguments = (PthreadArgs*)pthreadArgs;
  for(i=0; i<pthreadArguments->iterations; i++)
    {
      pthread_mutex_lock(&my_mutex);
      add(&(pthreadArguments->counterToAddTo), 1);
      add(&(pthreadArguments->counterToAddTo), -1);
      pthread_mutex_unlock(&my_mutex);
    }
  return NULL;
}
void* pthread_ts(void* pthreadArgs)
{
  int i;
  PthreadArgs* pthreadArguments = (PthreadArgs*)pthreadArgs;
  for(i=0; i<pthreadArguments->iterations; i++)
    {
      ts_lock(&excl_lock);
      add(&(pthreadArguments->counterToAddTo), 1);
      add(&(pthreadArguments->counterToAddTo), -1);
      ts_unlock(&excl_lock);
    }
}
void* pthread_cs(void* pthreadArgs)
{
  int i;
  PthreadArgs* pthreadArguments = (PthreadArgs*)pthreadArgs;
  for(i=0; i<pthreadArguments->iterations; i++)
    {
      cs_lock(&excl_lock);
      add(&(pthreadArguments->counterToAddTo), 1);
      add(&(pthreadArguments->counterToAddTo), -1);
      cs_unlock(&excl_lock);
    }
}
//Lock implementations
void ts_lock(int* excl_lock)
{
  while(__sync_lock_test_and_set(excl_lock, 1))
    ;
}
void ts_unlock(int* excl_lock)
{
  __sync_lock_release(excl_lock);
}
void cs_lock(int* excl_lock)
{
  while(__sync_val_compare_and_swap(excl_lock, 0, 1))
    ;
}
void cs_unlock(int* excl_lock)
{
  __sync_lock_release(excl_lock);
}
