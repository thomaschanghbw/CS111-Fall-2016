#include <signal.h>
#include <mcrypt.h>
#include <pthread.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

//Pipes to and from shell
int to_shell[2];
int from_shell[2];
pid_t my_pid;
int wait_pid_status;

int newsockfd;

//MCRYPT
const int KEYSIZE = 16;
char* key_filename = "my.key";
//Encryption variables
MCRYPT td_enc;
int encrypt_option_received =	0;

void sig_handler(int signo)
{
  close(newsockfd);
  exit(2);
}

//Encryption setup
void setup_encryption()
{
  int i;
  int fd_key_enc;
  char* IV_enc;
  char* key_enc = calloc(1, KEYSIZE);
  
  fd_key_enc = open(key_filename, O_RDONLY);
  if(fd_key_enc == -1)
    {
      perror("Could not open my.key");
      exit(1);
    }
  read(fd_key_enc, key_enc, KEYSIZE);
  td_enc = mcrypt_module_open("twofish", NULL, "cfb", NULL);
  if(td_enc == MCRYPT_FAILED)
    {
    perror("Could not open mcrypt module");
    exit(1);
    }
  IV_enc = malloc(mcrypt_enc_get_iv_size(td_enc));
  srand(2);
  for(i=0; i<mcrypt_enc_get_iv_size(td_enc); i++)
    {
      IV_enc[i]=rand();
    }
  i=mcrypt_generic_init(td_enc, key_enc, KEYSIZE, IV_enc);
  if(i<0)
    {
      mcrypt_perror(i);
      exit(1);;
    }
}

//Decryption variables
MCRYPT td_dec;
//Decryption setup                                                         
void setup_decryption()
{
  int i;
  int fd_key_dec;
  char* IV_dec;
  char* key_dec = calloc(1, KEYSIZE);

  fd_key_dec = open(key_filename, O_RDONLY);
  if(fd_key_dec == -1)
    {
      perror("Could not open my.key");
      exit(1);
    }
  read(fd_key_dec, key_dec, KEYSIZE);
  td_dec = mcrypt_module_open("twofish", NULL, "cfb", NULL);
  if(td_dec == MCRYPT_FAILED)
    {
    perror("Could not open mcrypt module");
    exit(1);
    }
  IV_dec = malloc(mcrypt_enc_get_iv_size(td_dec));
  srand(4);
  for(i=0; i<mcrypt_enc_get_iv_size(td_dec); i++)
    {
      IV_dec[i]=rand();
    }
  i=mcrypt_generic_init(td_dec, key_dec, KEYSIZE, IV_dec);
  if(i<0)
    {
      mcrypt_perror(i);
      exit(1);;
    }
}

void exit_func()
{
  if(encrypt_option_received)
    {
    mcrypt_module_close(td_enc);
    mcrypt_module_close(td_dec);
    }
}
void error(char* msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

//pthread routine

void *readShell(void *name_ptr)
{
  int read_from_shell = 0;
  char warrenBUFFet;
  while(1)
    {
      read_from_shell = read(from_shell[0], &warrenBUFFet, 1);
      if(read_from_shell==1)
	{
	  if(encrypt_option_received)
	    {
	      mcrypt_generic(td_enc, &warrenBUFFet, 1);
	    }
	  if(write(newsockfd, &warrenBUFFet, 1)!=1)
	    error("Could not write to client");
	}
      else
	{
	  close(newsockfd);
	  exit(2);
	}
    }
  
}

void *wait_for_shell(void *name_ptr)
{
  waitpid(my_pid, &wait_pid_status, 0);
  close(to_shell[0]);
  close(from_shell[1]);
  
}

int main(int argc, char* argv[])
{
  atexit(exit_func);

  //Signal handlet for SIGPIPE
  if(signal(SIGPIPE, sig_handler)==SIG_ERR)
    {
      error("Cannot register signal handle");
    }
  //Option parsing note:encrypt option received is a global var
  int port_option_received = 0;
  int portnum;
  while(1)
    {
      int optReceived;
      int optionIndex = 0;
      static struct option longopts[] = {
	{"port", required_argument, 0, 0},
	{"encrypt", no_argument, 0, 0}
      };
      optReceived = getopt_long(argc, argv, "", longopts, &optionIndex);
      if(optReceived ==-1)
	break;
      if(optReceived == 0)
	{
	  if(longopts[optionIndex].name == "port")
	    {
	      port_option_received = 1;
	      portnum = atoi(optarg);
	    }
	  if(longopts[optionIndex].name == "encrypt")
	    {
	      setup_encryption();
	      setup_decryption();
	      encrypt_option_received = 1;
	    }
	}
    }
  //If port not the received option, error
  if(!port_option_received)
    {
      fprintf(stderr, "Port option is required");
      exit(EXIT_FAILURE);
    }

  /***
   Shell
   ***/
  pipe(to_shell);
  pipe(from_shell);
  my_pid = fork();
  if(my_pid==-1)
    error("Fork error");
  if(my_pid==0)
    {
      close(0);
      dup(to_shell[0]);
      close(1);
      dup(from_shell[1]);
      close(2);
      dup(from_shell[1]);
      execlp("/bin/bash", "/bin/bash", NULL);
    }

  
  int sockfd, portno, clilen, n;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    error("Error opening socket");
  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portnum);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr))<0)
    error("Error on binding");
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
  if(newsockfd < 0)
    error("Error on accept");


  //Create threads
  pthread_t thread_id;
  pthread_t thread_waitshell_id;
  if(pthread_create(&thread_id, NULL, readShell, NULL))
     error("Problem creating thread");
  if(pthread_create(&thread_waitshell_id, NULL, wait_for_shell, NULL))
    error("Could not create thread");

  
  int read_from_client = 0;
  char buffythevampire;
  
  while(1)
    {
      read_from_client = read(newsockfd, &buffythevampire, 1);
      if(read_from_client==1)
	{
	  if(encrypt_option_received)
	    {
	      mdecrypt_generic(td_dec, &buffythevampire, 1);
	    }
	  write(to_shell[1], &buffythevampire, 1);
	}
      else
	{
	  close(newsockfd);
	  kill(my_pid, SIGKILL);
	  exit(1);
	}
    }
  /*
  bzero(buffer, 256);
  n = read(newsockfd, buffer, 255);
  if(n<0) error("Error reading from socket");
  //Do some reading
  printf("Message: %s\n", buffer);
  //Do some writing
  n = write(newsockfd, "I got your message", 18);
  if(n<0) error("Error writing to socket");
  return 0; 
  */
}
