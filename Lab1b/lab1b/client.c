#include <mcrypt.h>
#include<pthread.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <termios.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

char carriage_newline[2] = {'\r', '\n'};


//file descriptor for log option
int ifd;
//Bytes sent and received
int bytes_sent=0;
int bytes_received=0;
//Buffer to record input

char* sent_one_bytes = "SENT 1 bytes: ";
char* received_one_bytes = "RECEIVED 1 bytes: ";

//For option parsing
int log_messages = 0;
char* file_name;

int has_connected = 0;
//Declare socket variables                                                                      
  int sockfd, portnum, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

//variable set to true 

//MCRYPT
const int KEYSIZE = 16;
char* key_filename = "my.key";
//Decryption variables
MCRYPT td_dec;
int encrypt_option_received = 0;

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
  srand(2);
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

//Encryption Variables
MCRYPT td_enc;
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
  srand(4);
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

void error(char* msg)
{
  perror(msg);
  exit(EXIT_FAILURE); //Change based on spec
}

//restore terminal modes
struct termios attr_cpy;
void restore_modes()
{
  if(encrypt_option_received)
    {
    mcrypt_module_close(td_dec);
    mcrypt_module_close(td_enc);
    }
  if(tcsetattr(STDIN_FILENO, TCSANOW, &attr_cpy)!=0)
    {
      perror("Problem restoring terminal settings");
      exit(1);
    }
  if(log_messages)
    {
      close(ifd);
      free(file_name);
    }
}

void set_noncanon_noecho()
{

  struct termios attr;
  if(tcgetattr(STDIN_FILENO, &attr)!=0)
    {
      perror("Problem getting attributes");
      exit(1);
    }
  attr_cpy=attr;

  //Noncanonical mode with no echo                                                                
  attr.c_cc[VMIN]=1;
  attr.c_cc[VTIME]=0;
  attr.c_lflag&=~(ICANON|ECHO);
  if(tcsetattr(STDIN_FILENO, TCSANOW, &attr)!=0)
    {
      perror("Problem setting attributes");
      exit(1);
    }
}

void *readSocket(void *name_ptr)
{
    //Socket                                                                                        
  char buf[256];
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd<0)
    error("Error opening socket");
  server = gethostbyname("localhost");
  if(server==NULL)
    {
      fprintf(stderr, "No host by name");
      exit(EXIT_FAILURE);
    }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portnum);

  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    error("ERROR connecting");
  has_connected = 1;
  int read_from_server = 0;
  char buffythevampire;
  while(1)
    {
      read_from_server = read(sockfd, &buffythevampire, 1);
      if(read_from_server==1)
	{
	  if(log_messages)
            {
              if(write(ifd, received_one_bytes, 18)!=18)
                error("Error writing from server to file");
              if(write(ifd, &buffythevampire, 1)!=1)
                error("Error writing from server to file");
              if(write(ifd, carriage_newline+1, 1)!=1)
                error("Error writing from server to file");
            }
	  if(encrypt_option_received)
	    mdecrypt_generic(td_dec, &buffythevampire, 1);
	  write(STDOUT_FILENO, &buffythevampire, 1);
	 
	}
      else
	{
	  close(sockfd);
	  exit(1);
	}
    }
  /*
  printf("Please enter the message: ");
    bzero(buf,256);
    fgets(buf,255,stdin);
    n = write(sockfd,buf,strlen(buf));
    if (n < 0)
         error("ERROR writing to socket");
    bzero(buf,256);
    n = read(sockfd,buf,255);
    if (n < 0)
         error("ERROR reading from socket");
    printf("%s\n",buf);
    return 0;
  */
}

int main(int argc, char* argv[])
{


  
  //Restore modes on exit
  atexit(restore_modes);
  
  int port_option_received = 0;
  while(1)
    {
      int optReceived;
      int optionIndex = 0;
      static struct option longopts[] = {
        {"encrypt", no_argument, 0, 0},
        {"log", required_argument, 0, 0},
        {"port", required_argument, 0, 0}
      };
      optReceived = getopt_long(argc, argv, "", longopts, &optionIndex);
      if(optReceived == -1)
        break;
      if(optReceived == 0)
        {
          if(longopts[optionIndex].name == "encrypt")
            {
	      setup_encryption();
	      setup_decryption();
              encrypt_option_received = 1;
            }
          if(longopts[optionIndex].name == "log")
            {
              log_messages = 1;
	      file_name = (char*)malloc(sizeof(optarg));
	      strcpy(file_name, optarg);
            }
          if(longopts[optionIndex].name == "port")
            {
              port_option_received = 1;
              portnum = atoi(optarg);
            }
        }
    }

  
  //Make sure that port option was received                                                       
  if(!port_option_received)
    {
      fprintf(stderr, "Port option is required");
      exit(EXIT_FAILURE);
    }

//Open the file to log messages
  if(log_messages)
     ifd = creat(file_name, S_IRWXU);
  
  //Create a thread to read from the server
  pthread_t thread_id_server;
  if(pthread_create(&thread_id_server, NULL, readSocket, NULL))
    {
      error("Problem creating thread");
    }
  
  
    set_noncanon_noecho();
  /*
  //Socket
  char buf[256];
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd<0)
    error("Error opening socket");
  server = gethostbyname("localhost");
  if(server==NULL)
    {
      fprintf(stderr, "No host by name");
      exit(EXIT_FAILURE);
    }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portnum);

  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    error("ERROR connecting");

  printf("Please enter the message: ");
    bzero(buf,256);
    fgets(buf,255,stdin);
    n = write(sockfd,buf,strlen(buf));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buf,256);
    n = read(sockfd,buf,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buf);
    return 0;
  */
  /*
  if(tcgetattr(STDIN_FILENO, &attr)!=0)
    {
      perror("Problem getting attributes");
      exit(1);
    }
  attr_cpy=attr;

  //Noncanonical mode with no echo
  attr.c_cc[VMIN]=1;
  attr.c_cc[VTIME]=0;
  attr.c_lflag&=~(ICANON|ECHO);
  if(tcsetattr(STDIN_FILENO, TCSANOW, &attr)!=0)
    {
      perror("Problem setting attributes");
      exit(1);
    }
  */
  //Variables for reading and writing
  const int INITIALBUFFERSIZE = 1;
  int numRead;
  int i;
  char buffer; // = (char*)malloc(sizeof(char)*INITIALBUFFERSIZE);
  int break_while_loop = 0;
  while(!has_connected)
    {

    }
  while(1)
    {
      numRead = read(STDIN_FILENO, &buffer, INITIALBUFFERSIZE);
   
    if(numRead)
	{
	  //place value in buffer into a variable
	  //char current_buffer_content = buffer[i];
	  if(buffer=='\004')
	    {
	      close(sockfd);
	      break_while_loop=1;
	      break;
	    }
	  else if(buffer=='\r' || buffer == '\n')
            {
	      if(encrypt_option_received)
	      	{
		  buffer='\n';
		  mcrypt_generic(td_enc, &buffer, 1);
	      	}
	      if(log_messages)
		{
		  if(write(ifd, sent_one_bytes ,14) != 14)
		    error("Problem writing to file");
		  if(encrypt_option_received)
		    {
		      write(ifd, &buffer, 1);
		    }
		  else
		    {
		      write(ifd, carriage_newline+1, 1);
		    }
		  if(write(ifd, carriage_newline+1, 1)!=1)
		    error ("Problem writing to file");
		}
              if(write(STDOUT_FILENO, carriage_newline, 2)!=2)
                {
                  fprintf(stderr, "Error mapping to carriage return and newline");
                  exit(1); //Change possibly                                                           
                }
	      if (!encrypt_option_received) {
		if(write(sockfd, carriage_newline+1, 1)!=1)
		  error("Problem writing to socket");
	      }
	      else
		if(write(sockfd, &buffer, 1)!=1)
                  error("Problem writing to socket");
	    }
	  else
	    {

	     
	      if(write(STDOUT_FILENO, &buffer, 1)!=1)
                {
                  fprintf(stderr, "Error writing to STDOUT");
                  exit(EXIT_FAILURE); //Change possibly                                                
                }
	      if(encrypt_option_received)
               {
                mcrypt_generic(td_enc, &buffer, 1);
              }
	      if(log_messages)
                {

                  if(write(ifd, sent_one_bytes,14)!=14)
                    error("Problem writing to file");
                  if(write(ifd, &buffer, 1) !=1)
                    error("Problem writing to file");
                  if(write(ifd, carriage_newline+1, 1)!=1)
                    error("Problem writing to file");
                }
	      if(write(sockfd, &buffer, 1)!=1)
		{
		error("Problem writing to socket");
		}
	    }
	}
      if(break_while_loop)
	break;

    }
  return 0;
}
