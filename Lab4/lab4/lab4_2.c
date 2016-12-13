
/**
 Notes for the README:
 Part 2 - output to stdout even though not in spec. Piazza says okay to do so
 Part 2 - did not test for invalid commands
 Part 2- LCD support
 **/

#include <mraa/aio.h>
#include <mraa/i2c.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

/******
 Note: includes lcd support
 ******/

const int B=4275;
int sockfd;
pthread_mutex_t write_to_log_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t take_readings_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t scale_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lcd_lock = PTHREAD_MUTEX_INITIALIZER;
int fd;
int take_readings = 1;
int in_celsius = 0;
char degree_scale[] = " F\n";
char frequency_string[5];
int frequency = 3;
int use_lcd = 1;

//LCD
const uint8_t HD44780_ENTRYSHIFTDECREMENT = 0x00;
const uint8_t HD44780_ENTRYLEFT = 0x02;
const uint8_t HD44780_ENTRYMODESET = 0x04;
const uint8_t HD44780_CLEARDISPLAY = 0x01;
const uint8_t HD44780_DISPLAYON = 0x04;
const uint8_t HD44780_DISPLAYCONTROL = 0x08;
const uint8_t HD44780_8BITMODE = 0x10;
const uint8_t HD44780_FUNCTIONSET = 0x20;

const uint8_t HD44780_CMD = 0x80;
const uint8_t HD44780_DATA = 0x40;
const uint8_t HD44780_5x10DOTS = 0x04;
const uint8_t HD44780_2LINE = 0x08;

//LCD //////////////////////////////////////
typedef enum {
    UPM_SUCCESS = 0,                        /* Operation is successful, expected response */
    UPM_ERROR_NOT_IMPLEMENTED = 1,          /* Trying to access a feature or mode that is not implemented */
    UPM_ERROR_NOT_SUPPORTED = 2,            /* Trying to access a feature or mode that is not supported */
    UPM_ERROR_NO_RESOURCES = 3,             /* No resources to perform operation */
    UPM_ERROR_NO_DATA = 4,                  /* No data received or available from the sensor */
    UPM_ERROR_INVALID_PARAMETER = 5,        /* Invalid parameter passed to the function*/
    UPM_ERROR_INVALID_SIZE = 6,             /* Invalid buffer size */
    UPM_ERROR_OUT_OF_RANGE = 7,             /* When the input to drive is too high/low or -ve */
    UPM_ERROR_OPERATION_FAILED = 8,         /* When a function isn't able to perform as expected */
    UPM_ERROR_TIMED_OUT = 9,                /* Timed out while communicating with the sensor */
    
    UPM_ERROR_UNSPECIFIED = 99              /* Unspecified/Unknown error */
} upm_result_t;



typedef struct _lcd_display_context {
    mraa_i2c_context         i2cLCD;
    mraa_i2c_context         i2cRGB;
    
    uint8_t                  displayControl;
    uint8_t                  entryDisplayMode;
} *lcd_display_context;

void upm_delay_us(int time)
{
    if (time <= 0)
        time = 1;
    
#if defined(UPM_PLATFORM_LINUX)
    
    usleep(time);
    
#elif defined(UPM_PLATFORM_ZEPHYR)
# if KERNEL_VERSION_MAJOR == 1 && KERNEL_VERSION_MINOR >= 6
    
    upm_clock_t timer;
    upm_clock_init(&timer);
    while (upm_elapsed_us(&timer) < time)
        ;
    
# else
    
    struct nano_timer timer;
    void *timer_data[1];
    nano_timer_init(&timer, timer_data);
    nano_timer_start(&timer, USEC(time) + 1);
    nano_timer_test(&timer, TICKS_UNLIMITED);
    
# endif
    
#endif
}

upm_result_t lcd_display_command(const lcd_display_context dev, uint8_t cmd)
{
    
    if (mraa_i2c_write_byte_data(dev->i2cLCD, cmd, HD44780_CMD))
    {
        return UPM_ERROR_OPERATION_FAILED;
    }
    
    return UPM_SUCCESS;
}

upm_result_t lcd_display_backlight_on(const lcd_display_context dev, int on)
{
    
    mraa_result_t rv = MRAA_SUCCESS;
    if (on)
        rv = mraa_i2c_write_byte_data(dev->i2cRGB, 0xaa, 0x08);
    else
        rv = mraa_i2c_write_byte_data(dev->i2cRGB, 0x00, 0x08);
    
    if (rv)
    {
        return UPM_ERROR_OPERATION_FAILED;
    }
    
    return UPM_SUCCESS;
}

upm_result_t lcd_display_set_color(lcd_display_context dev, uint8_t r, uint8_t g,
                                   uint8_t b)
{
    
    mraa_result_t rv = MRAA_SUCCESS;
    rv = mraa_i2c_write_byte_data(dev->i2cRGB, 0, 0);
    rv = mraa_i2c_write_byte_data(dev->i2cRGB, 0, 1);
    
    rv = mraa_i2c_write_byte_data(dev->i2cRGB, r, 0x04);
    rv = mraa_i2c_write_byte_data(dev->i2cRGB, g, 0x03);
    rv = mraa_i2c_write_byte_data(dev->i2cRGB, b, 0x02);
    
    if (rv)
    {
        return UPM_ERROR_OPERATION_FAILED;
    }
    
    return UPM_SUCCESS;
}

upm_result_t lcd_display_display_on(const lcd_display_context dev, int on)
{
    
    if (on)
        dev->displayControl |= HD44780_DISPLAYON;
    else
        dev->displayControl &= ~HD44780_DISPLAYON;
    
    return lcd_display_command(dev, HD44780_DISPLAYCONTROL | dev->displayControl);
}

void lcd_display_close(lcd_display_context dev)
{
    
    if (dev->i2cLCD)
        mraa_i2c_stop(dev->i2cLCD);
    if (dev->i2cRGB)
        mraa_i2c_stop(dev->i2cRGB);
    
    free(dev);
}

upm_result_t lcd_display_clear(const lcd_display_context dev)
{
    
    upm_result_t ret;
    ret = lcd_display_command(dev, HD44780_CLEARDISPLAY);
    upm_delay_us(2000);
    return ret;
}

lcd_display_context lcd_display_init(int bus, int lcd_addr, int rgb_addr)
{
    lcd_display_context dev =
    (lcd_display_context)malloc(sizeof(struct _lcd_display_context));
    
    if (!dev)
        return NULL;
    
    memset((void *)dev, 0, sizeof(struct _lcd_display_context));
    
        // initialize the MRAA contexts
    
    if (!(dev->i2cLCD = mraa_i2c_init(bus)))
    {
        lcd_display_close(dev);
        
        return NULL;
    }
    
    // now check the address...
    if (mraa_i2c_address(dev->i2cLCD, lcd_addr) != MRAA_SUCCESS)
    {
        lcd_display_close(dev);
        
        return NULL;
    }
    
    if (!(dev->i2cRGB = mraa_i2c_init(bus)))
    {
        lcd_display_close(dev);
        
        return NULL;
    }
    
    // now check the address...
    if (mraa_i2c_address(dev->i2cRGB, rgb_addr) != MRAA_SUCCESS)
    {
        lcd_display_close(dev);
        
        return NULL;
    }
    
        upm_delay_us(50000);
    lcd_display_command(dev, HD44780_FUNCTIONSET | HD44780_8BITMODE);
    
   
    upm_delay_us(4500);
    lcd_display_command(dev, HD44780_FUNCTIONSET | HD44780_8BITMODE);
    

    upm_delay_us(150);
    lcd_display_command(dev, HD44780_FUNCTIONSET | HD44780_8BITMODE);
    
    /* Set 2 row mode and font size */
    lcd_display_command(dev, HD44780_FUNCTIONSET | HD44780_8BITMODE
                        | HD44780_2LINE | HD44780_5x10DOTS);
    upm_delay_us(100);
    
    lcd_display_display_on(dev, 1);
    upm_delay_us(100);
    
    lcd_display_clear(dev);
    upm_delay_us(2000);
    
    lcd_display_command(dev, HD44780_ENTRYMODESET | HD44780_ENTRYLEFT
                        | HD44780_ENTRYSHIFTDECREMENT);
    
    lcd_display_backlight_on(dev, 1);
    // full white
    lcd_display_set_color(dev, 0xff, 0xff, 0xff);
    
    return dev;
}

upm_result_t lcd_display_set_cursor(const lcd_display_context dev, unsigned int row,
                                    unsigned int column)
{
    
    column = column % 16;
    uint8_t offset = column;

    offset += row * 0x40;
    
    return lcd_display_command(dev, HD44780_CMD | offset);
}



upm_result_t lcd_display_data(const lcd_display_context dev, uint8_t cmd)
{
    
    if (mraa_i2c_write_byte_data(dev->i2cLCD, cmd, HD44780_DATA))
    {
        return UPM_ERROR_OPERATION_FAILED;
    }
    return UPM_SUCCESS;
}

upm_result_t lcd_display_write(const lcd_display_context dev, char *buffer,
                               int len)
{
    
    upm_result_t error = UPM_SUCCESS;
    
    int i;
    for (i=0; i<len; ++i)
        error = lcd_display_data(dev, buffer[i]);
    
    return error;
}
////////////////////
float convert_to_tmpr(uint16_t value, int fahrenheit)
{
    
    float R = 1023.0/((float)value)-1.0;
    R = 100000.0*R;
    
    float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
    
    if(fahrenheit)
    {
        temperature = temperature * (float)9/5 + 32;
    }
    
    return temperature;
}

void* listenToServer(void* void_ptr)
{
    char buf[50];
    int len;
    while(1)
    {
        //zero out the buffer then read the commands
        bzero(&buf, sizeof(buf));
        read(sockfd, &buf, sizeof(buf));
//        //Send command to stdout
//        write(1, &buf, strlen(buf));
//        pthread_mutex_lock(&write_to_log_lock);
//        //Log the command
//        write(fd, &buf, strlen(buf));
//        pthread_mutex_unlock(&write_to_log_lock);
        
        //Process the command
        if(strcmp("OFF", buf)==0)
        {
            len = strlen(buf);
            buf[len] = '\n';
            buf[len+1] = '\0';
            close(sockfd);
            write(1, &buf, strlen(buf));
            pthread_mutex_lock(&write_to_log_lock);
            write(fd, &buf, strlen(buf));
            close(fd);
            pthread_mutex_unlock(&write_to_log_lock);
            fflush(stdout);
            exit(1);
        }
        else if(strcmp("STOP", buf)==0)
        {
            
            pthread_mutex_lock(&take_readings_lock);
            take_readings = 0;
            pthread_mutex_unlock(&take_readings_lock);
            len = strlen(buf);
            buf[len] = '\n';
            buf[len+1] = '\0';
            write(1, &buf, strlen(buf));
            pthread_mutex_lock(&write_to_log_lock);
            write(fd, &buf, strlen(buf));
            pthread_mutex_unlock(&write_to_log_lock);
            fflush(stdout);
        }
        else if(strcmp("START", buf)==0)
        {
            pthread_mutex_lock(&take_readings_lock);
            take_readings = 1;
            pthread_mutex_unlock(&take_readings_lock);
            len = strlen(buf);
            buf[len] = '\n';
            buf[len+1] = '\0';
            write(1, &buf, strlen(buf));
            pthread_mutex_lock(&write_to_log_lock);
            write(fd, &buf, strlen(buf));
            pthread_mutex_unlock(&write_to_log_lock);
            fflush(stdout);
        }
        else if(strcmp("SCALE=C", buf)==0)
        {
            pthread_mutex_lock(&scale_lock);
            in_celsius = 1;
            strcpy(degree_scale, " C\n");
            pthread_mutex_unlock(&scale_lock);
            len = strlen(buf);
            buf[len] = '\n';
            buf[len+1] = '\0';
            write(1, &buf, strlen(buf));
            pthread_mutex_lock(&write_to_log_lock);
            write(fd, &buf, strlen(buf));
            pthread_mutex_unlock(&write_to_log_lock);
            fflush(stdout);
        }
        else if(strcmp("SCALE=F", buf)==0)
        {
            pthread_mutex_lock(&scale_lock);
            in_celsius = 0;
            strcpy(degree_scale, " F\n");
            pthread_mutex_unlock(&scale_lock);
            len = strlen(buf);
            buf[len] = '\n';
            buf[len+1] = '\0';
            write(1, &buf, strlen(buf));
            pthread_mutex_lock(&write_to_log_lock);
            write(fd, &buf, strlen(buf));
            pthread_mutex_unlock(&write_to_log_lock);
            fflush(stdout);
        }
//        else
//        {
//            printf("%i, %i", strcmp("FREQ=", buf),strcmp("FREQ=3600", buf));
//        }
        else if((strcmp("FREQ=", buf) < 0) && (strcmp("FREQ=9", buf) >= 0))
        {
            bzero(frequency_string, sizeof(frequency_string));
            memmove(frequency_string, &buf[5], strlen(buf)-5);
            frequency = atoi((const char*)frequency_string); //Technically I should put locks around this, but rc is very unlikely
            len = strlen(buf);
            buf[len] = '\n';
            buf[len+1] = '\0';
            write(1, &buf, strlen(buf));
            pthread_mutex_lock(&write_to_log_lock);
            write(fd, &buf, strlen(buf));
            pthread_mutex_unlock(&write_to_log_lock);
            fflush(stdout);
        }
        else if(strcmp("DISP Y", buf) == 0)
        {
            pthread_mutex_lock(&lcd_lock);
            use_lcd = 1;
            pthread_mutex_unlock(&lcd_lock);
            len = strlen(buf);
            buf[len] = '\n';
            buf[len+1] = '\0';
            write(1, &buf, strlen(buf));
            pthread_mutex_lock(&write_to_log_lock);
            write(fd, &buf, strlen(buf));
            pthread_mutex_unlock(&write_to_log_lock);
            fflush(stdout);

        }
        else if(strcmp("DISP N", buf) == 0)
        {
            pthread_mutex_lock(&lcd_lock);
            use_lcd = 0;
            pthread_mutex_unlock(&lcd_lock);
            len = strlen(buf);
            buf[len] = '\n';
            buf[len+1] = '\0';
            write(1, &buf, strlen(buf));
            pthread_mutex_lock(&write_to_log_lock);
            write(fd, &buf, strlen(buf));
            pthread_mutex_unlock(&write_to_log_lock);
            fflush(stdout);

        }
        else //if the command is invalid, append " I" and write to stdout and the log
        {
            strcat(buf, " I");
            len = strlen(buf);
            buf[len] = '\n';
            buf[len+1] = '\0';
            write(1, &buf, strlen(buf));
            pthread_mutex_lock(&write_to_log_lock);
            write(fd, &buf, strlen(buf));
            pthread_mutex_unlock(&write_to_log_lock);
            fflush(stdout);
        }
    }
}

int main()
{
    //Socket data structures
    int portnum, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    lcd_display_context my_lcd = lcd_display_init(0, 0x7C>>1, 0xC4>>1);
    lcd_display_clear(my_lcd);
    lcd_display_set_color(my_lcd, 0xff, 0x00, 0xff);
    lcd_display_set_cursor(my_lcd, 0, 1);
    lcd_display_write(my_lcd, " ", 1);
    sleep(.15);

    
    //Connect to server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("Problem connecting to server\n");
        exit(1);
    }
    server = gethostbyname("lever.cs.ucla.edu");
    if(server==NULL)
    {
        fprintf(stderr, "No host by name\n");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portnum = 16000;
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portnum);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    {
        perror("Error connecting\n");
        exit(1);
    }
    
    //Get new port number and connect
    char buf[] = "Port request 004632787";
    uint32_t newportnum;
    write(sockfd, buf, strlen(buf));
    read(sockfd, &newportnum, sizeof(newportnum));
    close(sockfd);
    
    //Connect to new port
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(newportnum);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    {
        perror("Error connecting\n");
        exit(1);
    }
    
    
    /**
     Temperature stuff below
     **/
    uint16_t value = 0;
    mraa_aio_context tmpr = mraa_aio_init(0);
    char log_str[100];
    char log_server[100];
    time_t rawtime;
    struct tm *info;
    char buffer[80];
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    fd = creat("./lab4_2_temperature.log", mode);
    //int fd2 = creat("./server_gets.log", mode); //note: delete this later
    float temperature;
    
    // create a thread to monitor the server
    pthread_t server_listener;
    if(pthread_create(&server_listener, NULL, listenToServer, NULL))
    {
        fprintf(stderr, "Error creating threads\n");
        exit(1);
    }

    while(1)
    {
       // pthread_mutex_lock(&take_readings_lock);
        if(take_readings)
        {
        bzero(&log_str, sizeof(log_str));
        bzero(&log_server, sizeof(log_server));
        value = mraa_aio_read(tmpr);
        time(&rawtime);
        info = localtime(&rawtime);
        strftime(buffer, 80,"%H:%M:%S ", info);
        pthread_mutex_lock(&scale_lock);
        if (in_celsius)
        {
            pthread_mutex_unlock(&scale_lock);
             temperature = convert_to_tmpr(value, 0);
        }
        else
        {
            pthread_mutex_unlock(&scale_lock);
             temperature = convert_to_tmpr(value, 1);
        }
        sprintf(log_str, "%g", temperature);
        log_str[4] = '\0';
        
        strcpy(log_server, "004632787 TEMP=");
        strcat(log_server, log_str);
        strcat(log_server, "\n");
        write(sockfd, log_server, strlen(log_server)); //Change this to send to server later
        
        // Write to log
        pthread_mutex_lock(&write_to_log_lock);
        write(fd, buffer, 9);
        write(fd, log_str, strlen(log_str));
        write(fd, degree_scale, 3); //Change this
        pthread_mutex_unlock(&write_to_log_lock);
            

        //Write to stdout
        printf("%s %s %s", buffer, log_str, degree_scale); //Change this
            
            
            //Write to lcd
            pthread_mutex_lock(&lcd_lock);
            if(use_lcd)
            {
                pthread_mutex_unlock(&lcd_lock);
                lcd_display_clear(my_lcd);
                lcd_display_set_color(my_lcd, 0xff, 0x00, 0xff);
                lcd_display_set_cursor(my_lcd, 0, 0);
                lcd_display_write(my_lcd, log_str, strlen(log_str));
            }
            else
                pthread_mutex_unlock(&lcd_lock);
        
        fflush(stdout);
        pthread_mutex_unlock(&take_readings_lock);
        sleep(frequency);
            
        }
        else
            pthread_mutex_unlock(&take_readings_lock);
        //sched_yield();
        

    }
    mraa_aio_close(tmpr);
    close(sockfd);
}






