#include <mraa/aio.h>
#include<mraa/i2c.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**********************
 LCD Support included - Will return errors without lcd attached
 **********************/

const int B=4275;
///
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

//////
//
//LCD Stuff//////////////////////////////////////
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

//////////////////////////////////////////////////////
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
int main()
{
	uint16_t value = 0;
	mraa_aio_context tmpr = mraa_aio_init(0);
	char log_str[100];
    
    /**
     Experimental
     **/
    lcd_display_context my_lcd = lcd_display_init(0, 0x7C>>1, 0xC4>>1);
    lcd_display_clear(my_lcd);
    lcd_display_set_color(my_lcd, 0xff, 0x00, 0xff);
    lcd_display_set_cursor(my_lcd, 0, 1);
    lcd_display_write(my_lcd, " ", 1);
    sleep(.25);
    /////////////////////////
    
	time_t rawtime;
	struct tm *info;
	char buffer[80];
	int fd;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	fd = creat("./lab4_1_temperature.log", mode);

	while(1)
	{
		value = mraa_aio_read(tmpr);
		time(&rawtime);
		info = localtime(&rawtime);
		strftime(buffer, 80,"%H:%M:%S ", info);
        float temperature = convert_to_tmpr(value, 1);
		sprintf(log_str, "%g\n", temperature);
        log_str[4] = '\0';
		write(fd, buffer, 9);
		write(fd, log_str, strlen(log_str));
        write(fd, "\n", 1);
        lcd_display_clear(my_lcd);
        lcd_display_set_color(my_lcd, 0xff, 0x00, 0xff);
        lcd_display_set_cursor(my_lcd, 0, 0);
        lcd_display_write(my_lcd, log_str, strlen(log_str));
		printf("%s %s\n", buffer, log_str);
		fflush(stdout);
		sleep(1);
	}
	mraa_aio_close(tmpr);
}
