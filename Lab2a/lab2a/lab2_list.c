#include "SortedList.h"
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//Lock
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
int excl_lock = 0;
SortedList_t head;
SortedListElement_t* listElements;
//Options
char addOptionName[20];
int syncOption=0;
char syncSymbol;
int opt_yield;
typedef struct
{
  int numberOfThreads;
  int numberOfIterations;
  int yieldOptionReceived;
} OptionsForAdd;

typedef struct
{
  int iterations;
  int startElementIndex;
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
  strcpy(addOptionName, "list");
  OptionsForAdd optionsReceived;
  opt_yield = 0;
  optionParser(argc, argv, &optionsReceived);
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
  head.next=NULL;
  head.prev=NULL;
  head.key==NULL;
  if(!syncOption)
    strcat(addOptionName, "-none");
  else
    {
      strcat(addOptionName, "-");
      strcat(addOptionName, &syncSymbol);
    }
  setUpAndRunThreads(&optionsReceived);
  return 0;
  /*
  printf("%s", addOptionName);
  
  SortedList_t head;
  head.key=NULL;
  head.next = NULL;
  head.prev = NULL;
  char aa, bb, cc, dd;
  aa=1;
  bb=2;
  cc=3;
  dd=4;
  SortedListElement_t a, b, c, d;
  a.key = &aa;
  b.key = &bb;
  c.key = &cc;
  d.key = &dd;
  a.next=NULL;
  a.prev=NULL;
  b.next=NULL;
  b.prev=NULL;
  c.next=NULL;
  c.prev=NULL;
  d.next=NULL;
  d.prev=NULL;
  SortedList_insert(&head, &a);
  SortedList_insert(&head, &b);
  SortedList_insert(&head, &c);
  SortedList_insert(&head, &d);
  int result = SortedList_length(&head);
  printf("%i", result);
  SortedListElement_t* tester = SortedList_lookup(&head, "dicks");
  if(tester==NULL)
    printf("dicks");
  tester = SortedList_lookup(&head, &cc);
  if(tester!=NULL)
    printf("out");
  //  printf("%i", result);
  result = SortedList_delete(&d);
  printf("\n%i", result);
  result = SortedList_delete(&a);
  printf("\n%i", result);
  result = SortedList_delete(&c);
  printf("\n%i", result);
  result = SortedList_delete(&b);
  printf("\n%i", result);
  */
  
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
  printf("%s,%i,%i,1,%ld,%ld,%ld\n", addOptionName, opts->numberOfThreads, opts->numberOfIterations, totalNumOps, totalThreadTime, avgOperationTime);
  free(keyVals);
  free(threadsForAdd);
  free(listElements);
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
  int i=0;
  PthreadArgs* pthreadArguments=(PthreadArgs*)pthreadArgs;
  int index = pthreadArguments->startElementIndex;
  for(i=0; i<pthreadArguments->iterations;i++)
    {
      pthread_mutex_lock(&my_mutex);
      SortedList_insert(&head, &listElements[index+i]);
      pthread_mutex_unlock(&my_mutex);
    }
  pthread_mutex_lock(&my_mutex);
  int lengthOfList = SortedList_length(&head);
  pthread_mutex_unlock(&my_mutex);
  for(i=0; i<pthreadArguments->iterations; i++)
    {
      pthread_mutex_lock(&my_mutex);
      if(SortedList_lookup(&head, listElements[index+i].key)==NULL)
        {
          fprintf(stderr, "Inserted list element no longer exists in the list!");
          exit(1);
        }
      pthread_mutex_unlock(&my_mutex);
      pthread_mutex_lock(&my_mutex);
      if(SortedList_delete(&listElements[index+i])!=0)
        {
          fprintf(stderr, "Could not delete and element that was added to the list!");
          exit(1);
        }
      pthread_mutex_unlock(&my_mutex);
  //  SortedList_delete(SortedList_lookup(&head, listElements[index+i].key));              
    }
}
void* pthread_ts(void* pthreadArgs)
{
  int i=0;
  PthreadArgs* pthreadArguments=(PthreadArgs*)pthreadArgs;
  int index = pthreadArguments->startElementIndex;
  for(i=0; i<pthreadArguments->iterations;i++)
    {
      ts_lock(&excl_lock);
      SortedList_insert(&head, &listElements[index+i]);
      ts_unlock(&excl_lock);
    }
  
  ts_lock(&excl_lock);
  int lengthOfList = SortedList_length(&head);
  ts_unlock(&excl_lock);
  
  for(i=0; i<pthreadArguments->iterations; i++)
    {
      ts_lock(&excl_lock);
      if(SortedList_lookup(&head, listElements[index+i].key)==NULL)
        {
          fprintf(stderr, "Inserted list element no longer exists in the list!");
          exit(1);
        }
      ts_unlock(&excl_lock);
      ts_lock(&excl_lock);
      if(SortedList_delete(&listElements[index+i])!=0)
        {
          fprintf(stderr, "Could not delete and element that was added to the list!");
          exit(1);
        }
      ts_unlock(&excl_lock);
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
