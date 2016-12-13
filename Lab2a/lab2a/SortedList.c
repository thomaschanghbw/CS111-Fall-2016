#include "SortedList.h"
#include <stdio.h> //fprintf
#include <stdlib.h> //exit
#include <string.h> //strcmp
#include <sched.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
  int yield_insert = 0;
  if(opt_yield==1 || opt_yield ==3 || opt_yield == 5 || opt_yield==7)
    yield_insert = 1;
  SortedList_t* curr;
  SortedList_t* prev;
  if(list->key!=NULL)
    {
      fprintf(stderr, "Inputed list is not a list head");
      exit(1);
    }
  else if(list->next==NULL)
    {
      list->next=element;
      element->prev=list;
    }
  else
      {
	prev = list;
	curr = list->next;
	while(curr!=NULL && strcmp(element->key, curr->key)>0)
	  {
	    prev=curr;
	    if(yield_insert)
	      sched_yield();
	    curr=curr->next;
	  }
	if(curr==NULL)
	  {
	    element->prev=prev;
	    prev->next=element;
	  }
	else
	  {
	    element->next = curr;
	    element->prev=prev;
	    curr->prev=element;
	    prev->next=element;
	  }
      }
}
int SortedList_delete(SortedListElement_t *element)
{
  int yield_insert = 0;
  if(opt_yield==2 || opt_yield ==3 || opt_yield == 6 || opt_yield==7)
    yield_insert = 1;
  SortedListElement_t* temp;
  if(element->prev==NULL)
    {
      return 1;
    }
  if(element->prev->next!=element)
    {
      return 1;
    }
  if(element->next==NULL)
    {
      element->prev->next=NULL;
      element->prev=NULL;
    }

  else if(element->next!=NULL)
    {
      if(element->next->prev!=element){
	return 1;}
      else {
	element->prev->next=element->next;
	if(yield_insert)
	  sched_yield();
	element->next->prev=element->prev;
	element->prev=NULL;
	element->next=NULL;
      }
    }
  return 0;
}
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char* key)
{
  int yield_insert = 0;
  if(opt_yield==4 || opt_yield ==5 || opt_yield == 6 || opt_yield==7)
    yield_insert = 1;
  SortedList_t* temp;
  if(list->next==NULL)
    return NULL;
  else
    {
      temp=list->next;
    }
  while(strcmp(temp->key, key)!=0 && temp->next != NULL)
    {
      if(yield_insert)
	sched_yield();
      temp=temp->next;
    }
  if(strcmp(temp->key, key)==0)
    return temp;
  if(temp->next==NULL)
    return NULL;
  return temp;
}

int SortedList_length(SortedList_t *list)
{
  int yield_insert = 0;
  if(opt_yield==4 || opt_yield ==5 || opt_yield == 6 || opt_yield==7)
    yield_insert = 1;
  SortedListElement_t* temp;
  if(list->next==NULL)
    return 0;
  long numberOfElements	= 1;
  temp=list->next;
  while(temp->next!=NULL)
    {
      numberOfElements++;
      if(temp->prev==NULL)
	return -1;
      if(temp->prev->next!=temp)
	return -1;
      if(temp->next->prev!=temp)
	return -1;
      if(yield_insert)
	sched_yield();
      temp=temp->next;
    }
  return numberOfElements;
}
