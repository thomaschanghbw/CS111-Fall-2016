#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/wait.h>
#include <pthread.h>

int to_shell[2], from_shell[2];
char minibuff;
int shell_command = 0;
pid_t my_pid;
int wait_pid_status;
int shell_return_code = 0;
//Keeps track of whether the created thread (the one reading from shell) exited                 
  int thread_exited = 0;
//Tracks if program ends from something from the terminal or shell, default assumes shell
int received_from_shell = 1;

void sig_handler(int signo)
{
  exit(1);
}

//atexit function does not take arguments, so make attr_cpy global
struct termios attr_cpy;
void restore_modes()
{
  if(tcsetattr(STDIN_FILENO,TCSANOW, &attr_cpy)!=0)
    {
      perror("Problem restoring terminal settings");
      exit(1);
    }
  if(shell_command)
    {
      if(received_from_shell)
      printf("%i", shell_return_code);
 
    }
}

//pthread start routine
void *readShells(void *name_ptr)
{
  while(1)
    {
      int read_from_shell = 0;
      read_from_shell = read(from_shell[0], &minibuff, 1);
      if(read_from_shell)
	{
	  if(minibuff=='\004')
	    {
	      exit(1);
	    }
	  write(STDOUT_FILENO, &minibuff, 1);
	}
      else
	{
	  //printf("EOF");
	  exit(1);
	}
    }
}

void *wait_for_shell(void *name_ptr)
{
  waitpid(my_pid, &wait_pid_status, 0);
  close(to_shell[0]);
  if(WIFEXITED(wait_pid_status))
    {
     
    shell_return_code = WEXITSTATUS(wait_pid_status);
    exit(1);
    }

}

int main(int argc, char *argv[])
{
    //Signal handler for SIGPIPE                                                                    
  if(signal(SIGPIPE, sig_handler)==SIG_ERR)
    {
      perror("Cannot register signal handle");
      exit(EXIT_FAILURE);
    }
  /***********************
  Parse arguments
  ***********************/   
  while(1)
    {
      int optReceived;
      int optionIndex = 0;
      static struct option longopts[] = {
	{"shell", no_argument, 0, 0}
      };

      optReceived = getopt_long(argc, argv, "", longopts, &optionIndex);
      if(optReceived == -1)
	break;
      if(optReceived == 0)
	{
	  if(longopts[optionIndex].name == "shell")
	    {
	      //Set true
	      shell_command = 1;
	      
	      //Create two pipes, one in each direction
	      pipe(to_shell);
	      pipe(from_shell);
	      //  pid_t pid;
	      my_pid = fork();
	      if(my_pid==-1)
		{
		  perror("Fork error");
		  exit(EXIT_FAILURE); //!!
		}
	      if(my_pid == 0)
		{
		  //child
		  close(0);
		  dup(to_shell[0]);
		  close(1);
		  dup(from_shell[1]);
		  close(2);
		  dup(from_shell[1]);
		  execlp("/bin/bash", "/bin/bash", NULL);
		}
	      else
		{
		  
		}
	    }
	}
    }

  //Restore terminal modes at exit                                                               
  atexit(restore_modes);

  //Signal handler for SIGPIPE

  /*****************
Create pthread parameter types 
   *****************/
  
  pthread_t thread_id;
  pthread_t thread_waitshell_id;
  if(shell_command)
  if(pthread_create(&thread_id, NULL, readShells, NULL))
    {
      //error
      perror("Problem creating thread");
      exit(EXIT_FAILURE);
    }

  //Create pthread to wait and check changes in shell's state
  if(shell_command)
  if(pthread_create(&thread_waitshell_id, NULL, wait_for_shell, NULL))
    {
      perror("Cannot create thread");
      exit(EXIT_FAILURE);
    }
  
  struct termios attr;
  //printf("%i", attr.c_lflag);
  if(tcgetattr(STDIN_FILENO, &attr)!=0)
    {
      perror("Problem getting attributes");
      exit(1); //Change exit number possibly
    }
  //printf("\n%i", attr.c_lflag);
  //printf("\n%i", attr_cpy.c_lflag);
  attr_cpy = attr;
  //printf("\n%i", attr_cpy.c_lflag);
    
  //Change to noncanonical mode with no echo
  attr.c_cc[VMIN]=1;
  attr.c_cc[VTIME]=0;
  attr.c_lflag&= ~(ICANON|ECHO);
  if(tcsetattr(STDIN_FILENO, TCSANOW, &attr)!=0)
    {
      perror("Problem setting attributes");
      exit(1); //Change number possibly
    }



  
  //Create a buffer
  const int INITIALBUFFERSIZE = 10; //Change to a higher number after testing
  char* buffer= (char*) malloc(sizeof(char)*INITIALBUFFERSIZE);
  // char current_buffer_content;
  int numRead;
  int i;
  int break_while_loop = 0;
  while(numRead = read(STDIN_FILENO, buffer, INITIALBUFFERSIZE))
    {
            for(i=0; i<numRead; i++)
      	{
	 	  
	  //place value in buffer in variable
	  	  char current_buffer_content = buffer[i];
	  //Check if shell command, if so, if Ctrl-C received, send SIGINT to shell
	  if(shell_command)
	    {
	      if(current_buffer_content=='\003')
		{
		  kill(my_pid, SIGINT);
		  received_from_shell = 0;
		  printf("SIGINT");
		}
	    }
	    
	  if(current_buffer_content=='\004')
	    {
	      if(shell_command)
		{
		  received_from_shell=0;
		  close(to_shell[0]);
		  close(to_shell[1]);
		  close(from_shell[0]);
		  close(from_shell[1]);
		  kill(my_pid, SIGHUP);
		  printf("SIGHUP");
		}
	      break_while_loop=1;
	      break;
	    }
	  else if(current_buffer_content=='\r' || current_buffer_content == '\n')
	    {
	      char carriage_newline[2] = {'\r', '\n'};
	      if(write(STDOUT_FILENO, carriage_newline, 2)!=2)
		{
		  fprintf(stderr, "Error mapping to carriage return and newline");
		  exit(1); //Change possibly
		}
	      //Check if shell option
	      if(shell_command)
		{
		  char temp_n = '\n';
		  //If shell option, write newline to shell as well
		  if(write(to_shell[1], &temp_n, 1) != 1)
		    {
		      fprintf(stderr, "Error writing to shell");
		      exit(EXIT_FAILURE);
		    }
		}
	    }
	  else
	    {

	      /* Moved below so output is not written to terminal when SIGPIPE received
	      if(write(STDOUT_FILENO, buffer+i, 1)!=1)
		{
		  fprintf(stderr, "Error writing to STDOUT");
		  exit(EXIT_FAILURE); //Change possibly
		} */
	      //Check if shell option
	      if(shell_command)
		{
		  //If shell option, write to shell as well
		  if(write(to_shell[1], buffer+i, 1)!=1)
		     {
		     fprintf(stderr, "Error writing to shell");
		    exit(EXIT_FAILURE);
		    }
		}
	      if(write(STDOUT_FILENO, buffer+i, 1)!=1)
                {
                  fprintf(stderr, "Error writing to STDOUT");
                  exit(EXIT_FAILURE); //Change possibly                                           
                }
	    }
	  	}
      if(break_while_loop)
	{
	  break;
	}
    }
  exit(0);
  //tcsetattr(STDIN_FILENO, TCSANOW, &attr_cpy);

}
