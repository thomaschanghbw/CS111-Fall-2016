#include "SortedList.h"
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//Lock
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t* list_lock;
int excl_lock = 0;
int* list_tslock;
int thread_tslock=0;
long long total_lockwait = 0;
SortedList_t head;
SortedList_t* heads;
SortedListElement_t* listElements;
//Options
char addOptionName[20];
int syncOption=0;
int receivedlist = 1;
char syncSymbol;
int opt_yield;
int numberOfLists;
typedef struct
{
  int numberOfThreads;
  int numberOfIterations;
  int yieldOptionReceived;
  int numberOfLists;
} OptionsForAdd;

typedef struct
{
  int iterations;
  int startElementIndex;
  int numberOfLists;
} PthreadArgs;
//Parses options, modifies the OptionsForAdd struct
void optionParser(int argc, char* argv[], OptionsForAdd* opts);

void setUpAndRunThreads(OptionsForAdd* opts);

//random string generator
char *rand_string(char *str, int size);

//Pthread functions
void* pthread_func(void* pthreadArgs);
void* pthread_mutex(void* pthreadArgs);
void* pthread_ts(void* pthreadArgs);

//Lock implementations                                                                    
void ts_lock(int* excl_lock);
void ts_unlock(int* excl_lock);

void error(char* errmessage);

int main(int argc, char* argv[])
{
  int inc;
  strcpy(addOptionName, "list");
  OptionsForAdd optionsReceived;
  opt_yield = 0;
  numberOfLists=0;
  optionParser(argc, argv, &optionsReceived);
  if(!numberOfLists)
    {
      receivedlist=0;
      list_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex));
      numberOfLists++;
    }

  if(opt_yield==0)
    strcat(addOptionName, "-none");
  else if(opt_yield==1 || opt_yield==3 || opt_yield ==5 || opt_yield==7)
    {
      if(opt_yield==1)
	strcat(addOptionName, "-i");
      else if(opt_yield == 3)
	strcat(addOptionName, "-id");
      else if(opt_yield==5)
	strcat(addOptionName, "-il");
      else if(opt_yield==7)
	strcat(addOptionName, "-idl");
    }
  else if(opt_yield==2 || opt_yield==6 || opt_yield == 4)
    {
      if(opt_yield==2)
	strcat(addOptionName, "-d");
      if(opt_yield==6)
	strcat(addOptionName, "-dl");
      if(opt_yield==4)
	strcat(addOptionName, "-l");
    }
  if(numberOfLists)
    {
    heads = (SortedList_t *) malloc(numberOfLists*sizeof(SortedList_t));
    }
  head.next=NULL;
  head.prev=NULL;
  head.key==NULL;
  list_lock = (pthread_mutex_t*)malloc(numberOfLists*sizeof(pthread_mutex));
  list_tslock = (int*)malloc(numberOfLists*sizeof(int));
  if(numberOfLists)
    {
      for(inc=0; inc<numberOfLists;inc++)
	{
	  heads[inc].next=NULL;
	  heads[inc].prev=NULL;
	  heads[inc].key==NULL;
	  if(receivedlist)
	    list_tslock[inc]=0;
		    
	}
    }
  if(!syncOption)
    strcat(addOptionName, "-none");
  else
    {
      strcat(addOptionName, "-");
      strcat(addOptionName, &syncSymbol);
    }
    int j;
  for(j=0; j<numberOfLists; j++)
    {
      pthread_mutex_init(&list_lock[j], NULL);
    }
  setUpAndRunThreads(&optionsReceived);
  return 0;

  
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
        {"yield", required_argument, 0, 0},
	{"sync", required_argument, 0, 0},
	{"lists", required_argument, 0, 0}
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
  	      if(strchr(optarg, 'i'))
		  opt_yield++;
	      if(strchr(optarg, 'd'))
		opt_yield+=2;
	      if(strchr(optarg, 'l'))
		opt_yield+=4;
            }
          if(longopts[optionIndex].name == "sync")
            {
              syncOption = 1;
              if((*optarg)!='m' && (*optarg)!='s')
                {
                fprintf(stderr, "Invalid argument to sync");
                exit(1);
                }
              else
                syncSymbol = *optarg;
            }
	  if(longopts[optionIndex].name == "lists")
	    {
	      numberOfLists = atoi(optarg);
	      //	      printf("%i", numberOfLists);
	    
	    }
	}
    }
}

void setUpAndRunThreads(OptionsForAdd* opts)
{
  //Timers
  struct timespec start, stop;
  long totalNumOps = 0;
  long totalThreadTime = 0;
  long avgOperationTime=0;
  int i;

  //Set up argument to pthreads
  PthreadArgs* pthreadArgs = (PthreadArgs*)malloc(opts->numberOfThreads*sizeof(PthreadArgs));
  if(pthreadArgs==NULL)
    error("Could not allocate enough memory");
  for(i=0; i<opts->numberOfThreads; i++)
    {
      pthreadArgs[i].iterations = opts->numberOfIterations;
      pthreadArgs[i].numberOfLists = numberOfLists;
    }
  
  //Determine which function to run
  void* (*func_pointer)(void*);
  if(syncOption)
    {
      if(syncSymbol=='m'){
        func_pointer=pthread_mutex;
          }
      else if(syncSymbol=='s'){
        func_pointer=pthread_ts;
      }
    }
  else
    func_pointer=pthread_func;
  
  //Allocate memory for threads
  pthread_t* threadsForAdd = (pthread_t*)malloc(opts->numberOfThreads * sizeof(pthread_t));
  if(threadsForAdd == NULL)
    {
      fprintf(stderr, "Malloc error");
      exit(1);
    }
  
  //Allocate memory for the elements
  int numElements = opts->numberOfThreads * opts->numberOfIterations;
  listElements = (SortedListElement_t*)malloc(numElements*sizeof(SortedListElement_t));
  if(listElements==NULL)
    {
      fprintf(stderr, "Malloc error");
      exit(1);
    }
  
  //Allocate memory for the keys
  char* keyVals = (char*)malloc(numElements*3*sizeof(char));
  if(keyVals==NULL)
    {
      fprintf(stderr, "Malloc error");
      exit(1);
    }
  //Create random keys
  for(i=0; i<numElements; i++)
   {
     rand_string((keyVals+(i*3)), 3);
   }
  //Add the random keys into the elements
  for(i=0; i<numElements; i++)
     {
     listElements[i].prev=NULL;
     listElements[i].next=NULL;
     listElements[i].key=keyVals+(i*3);
    }

  //Start timer
  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
    error("Could not call clock_gettime");


  for(i=0; i<opts->numberOfThreads; i++)
    {
      pthreadArgs[i].startElementIndex = i*opts->numberOfIterations;
      if(pthread_create(&threadsForAdd[i], NULL, func_pointer, (void*)&pthreadArgs[i]) != 0)
        error("Failed to create threads");
    }
  for(i=0; i<opts->numberOfThreads; i++)
    {
      if(pthread_join(threadsForAdd[i], NULL)!=0)
        error("Failed to join threads");
    }
  
  
  //End timer
  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
    error("Could not call clock_gettime");
  if(SortedList_length(&head)!=0)
    {
      fprintf(stderr, "Sorted List length not 0 at the end");
      exit(1);
    }
  totalThreadTime = (1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
  totalNumOps = opts->numberOfThreads * opts->numberOfIterations * 3;
  avgOperationTime = totalThreadTime / totalNumOps;
  int numLists=1;
  if(numberOfLists)
    numLists=numberOfLists;
  printf("%s,%i,%i,%i,%ld,%ld,%ld,%lld\n", addOptionName, opts->numberOfThreads, opts->numberOfIterations,numLists,  totalNumOps, totalThreadTime, avgOperationTime, total_lockwait/(4*opts->numberOfThreads*opts->numberOfIterations));
  free(keyVals);
  free(threadsForAdd);
  free(listElements);
  if(opts->numberOfLists)
    free(heads);
}

char *rand_string(char *str, int size)
{
    const char avail[] = "abcdefghijklmnopqrstuvwxyz";
    size--;
    int inc;
    for (inc = 0; inc < size; inc++) {
      int key = rand() % (int) (sizeof(avail) - 1);
      *(str+inc) = avail[key];
    }
    *(str+size) = '\0';
    return str;
}

void* pthread_func(void* pthreadArgs)
{
  int i=0;
  PthreadArgs* pthreadArguments=(PthreadArgs*)pthreadArgs;
  int index = pthreadArguments->startElementIndex;
  for(i=0; i<pthreadArguments->iterations;i++)
    {
      SortedList_insert(&head, &listElements[index+i]);
    }
  int lengthOfList = SortedList_length(&head);
  for(i=0; i<pthreadArguments->iterations; i++)
    {
      if(SortedList_lookup(&head, listElements[index+i].key)==NULL)
	{
	  fprintf(stderr, "Inserted list element no longer exists in the list!");
	  exit(1);
	}
      if(SortedList_delete(&listElements[index+i])!=0)
      	{
	  fprintf(stderr, "Could not delete and element that was added to the list!");
	  exit(1);
	}
  //  SortedList_delete(SortedList_lookup(&head, listElements[index+i].key));
    }
}
void error(char* errmessage)
{
  perror(errmessage);
  exit(1);
}

void* pthread_mutex(void* pthreadArgs)
{
  long long lock_counter = 0;
  struct timespec start, stop;
  int i=0;
  PthreadArgs* pthreadArguments=(PthreadArgs*)pthreadArgs;
  int index = pthreadArguments->startElementIndex;
  /*********
   Insert
  *********/
  //  printf("%i",pthreadArguments->numberOfLists);
  for(i=0; i<pthreadArguments->iterations;i++)
    {
      if(!pthreadArguments->numberOfLists)
	{
	  //Start timer                                                                                                                                                    
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");
	  
	  pthread_mutex_lock(&my_mutex);
	  
	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  SortedList_insert(&head, &listElements[index+i]);
	  pthread_mutex_unlock(&my_mutex);
	}
      else
	{
	  int head_num = ((int)(*listElements[index+i].key)) % pthreadArguments->numberOfLists;
	  //lock
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");

	  pthread_mutex_lock(&list_lock[head_num]);
	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  SortedList_insert(&heads[head_num], &listElements[index+i]);
	  //unlock
	  pthread_mutex_unlock(&list_lock[head_num]);
	}
      lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
    }
  /**************
   Length
  **************/
  if(!pthreadArguments->numberOfLists)
    {
      if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	error("Could not call clock_gettime");
      
      pthread_mutex_lock(&my_mutex);
      
      if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	error("Could not call clock_gettime");
      
      int lengthOfList = SortedList_length(&head);
      pthread_mutex_unlock(&my_mutex);
      lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
    }
  else
    {
      int lengthOfList=0;
      for(i=0; i<pthreadArguments->numberOfLists; i++)
	{
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");
	  pthread_mutex_lock(&list_lock[i]);
	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  lengthOfList+=SortedList_length(&heads[i]);
	  pthread_mutex_unlock(&list_lock[i]);
	  lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
	}
    }
   
      /*********
       Lookup
      *********/
  for(i=0; i<pthreadArguments->iterations; i++)
    {
      //Start timer
      if(!pthreadArguments->numberOfLists)
	{
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");
      
	  pthread_mutex_lock(&my_mutex);

	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	       
	  if(SortedList_lookup(&head, listElements[index+i].key)==NULL)
	    {
	      fprintf(stderr, "Inserted list element no longer exists in the list!");
	      exit(1);
	    }
	  pthread_mutex_unlock(&my_mutex);
	  
	}
      else
	{
	  int head_num = ((int)(*listElements[index+i].key)) % pthreadArguments->numberOfLists;
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
            error("Could not call clock_gettime");
          pthread_mutex_lock(&list_lock[head_num]);//lock
          if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  if(SortedList_lookup(&heads[head_num], listElements[index+i].key)==NULL)
            {
              fprintf(stderr, "Inserted list element no longer exists in the list!");
              exit(1);
            }
	  pthread_mutex_unlock(&list_lock[head_num]);
	}
      lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));

      /*********
       Delete
      *********/
      if(!pthreadArguments->numberOfLists)
	{
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");
      
	  pthread_mutex_lock(&my_mutex);

	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  
	  if(SortedList_delete(&listElements[index+i])!=0)
	    {
	      fprintf(stderr, "Could not delete an element that was added to the list!");
	      exit(1);
	    }
	  pthread_mutex_unlock(&my_mutex);
	}
      else
	{
	  int head_num = ((int)(*listElements[index+i].key)) % pthreadArguments->numberOfLists;
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
            error("Could not call clock_gettime");
	  pthread_mutex_lock(&list_lock[head_num]);
          if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  if(SortedList_delete(&listElements[index+i])!=0)
            {
              fprintf(stderr, "Could not delete an element that was added to the list!");
              exit(1);
            }
	  pthread_mutex_unlock(&list_lock[head_num]);
	}
      lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
      pthread_mutex_lock(&thread_mutex);
      total_lockwait += lock_counter;
      pthread_mutex_unlock(&thread_mutex);
      
  //  SortedList_delete(SortedList_lookup(&head, listElements[index+i].key));              
    }
}
void* pthread_ts(void* pthreadArgs)
{
  long long lock_counter = 0;
  struct timespec start, stop;
  int i=0;
  PthreadArgs* pthreadArguments=(PthreadArgs*)pthreadArgs;
  int index = pthreadArguments->startElementIndex;
   for(i=0; i<pthreadArguments->iterations;i++)
    {
      if(!pthreadArguments->numberOfLists)
	{
	  //Start timer                                                                                                                                                    
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");
	  
	  ts_lock(&excl_lock);
	  
	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  SortedList_insert(&head, &listElements[index+i]);
	  ts_unlock(&excl_lock);
	}
      else
	{
	  int head_num = ((int)(*listElements[index+i].key)) % pthreadArguments->numberOfLists;
	  //lock
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");

	  ts_lock(&list_tslock[head_num]);
	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  SortedList_insert(&heads[head_num], &listElements[index+i]);
	  //unlock
	  ts_unlock(&list_tslock[head_num]);
	}
      lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
    }
  /**************
   Length
  **************/
  if(!pthreadArguments->numberOfLists)
    {
      if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	error("Could not call clock_gettime");
      
      ts_lock(&excl_lock);
      
      if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	error("Could not call clock_gettime");
      
      int lengthOfList = SortedList_length(&head);
      ts_unlock(&excl_lock);
      lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
    }
  else
    {
      int lengthOfList=0;
      for(i=0; i<pthreadArguments->numberOfLists; i++)
	{
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");
	  ts_lock(&list_tslock[i]);
	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  lengthOfList+=SortedList_length(&heads[i]);
	  ts_unlock(&list_tslock[i]);
	  lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
	}
    }
   
      /*********
       Lookup
      *********/
  for(i=0; i<pthreadArguments->iterations; i++)
    {
      //Start timer
      if(!pthreadArguments->numberOfLists)
	{
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");
      
	  ts_lock(&excl_lock);

	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	       
	  if(SortedList_lookup(&head, listElements[index+i].key)==NULL)
	    {
	      fprintf(stderr, "Inserted list element no longer exists in the list!");
	      exit(1);
	    }
	  ts_unlock(&excl_lock);
	  
	}
      else
	{
	  int head_num = ((int)(*listElements[index+i].key)) % pthreadArguments->numberOfLists;
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
            error("Could not call clock_gettime");
          ts_lock(&list_tslock[head_num]);//lock
          if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  if(SortedList_lookup(&heads[head_num], listElements[index+i].key)==NULL)
            {
              fprintf(stderr, "Inserted list element no longer exists in the list!");
              exit(1);
            }
	  ts_unlock(&list_tslock[head_num]);
	}
      lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));

      /*********
       Delete
      *********/
      if(!pthreadArguments->numberOfLists)
	{
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
	    error("Could not call clock_gettime");
      
	  ts_lock(&excl_lock);

	  if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  
	  if(SortedList_delete(&listElements[index+i])!=0)
	    {
	      fprintf(stderr, "Could not delete an element that was added to the list!");
	      exit(1);
	    }
	  ts_unlock(&excl_lock);
	}
      else
	{
	  int head_num = ((int)(*listElements[index+i].key)) % pthreadArguments->numberOfLists;
	  if(clock_gettime(CLOCK_MONOTONIC, &start)==-1)
            error("Could not call clock_gettime");
	  ts_lock(&list_tslock[head_num]);
          if(clock_gettime(CLOCK_MONOTONIC, &stop)==-1)
	    error("Could not call clock_gettime");
	  if(SortedList_delete(&listElements[index+i])!=0)
            {
              fprintf(stderr, "Could not delete an element that was added to the list!");
              exit(1);
            }
	  ts_unlock(&list_tslock[head_num]);
	}
      lock_counter+=(1000000000*(stop.tv_sec-start.tv_sec)+(stop.tv_nsec-start.tv_nsec));
      ts_lock(&thread_tslock);
      total_lockwait += lock_counter;
      ts_unlock(&thread_tslock);
      
  //  SortedList_delete(SortedList_lookup(&head, listElements[index+i].key));              
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
