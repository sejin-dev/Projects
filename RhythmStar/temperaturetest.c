#include <stdint.h> 
#include <string.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <getopt.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <sys/ioctl.h> 
#include <linux/types.h> 
#include <linux/spi/spidev.h> 
#include "temperature.h"

char gbuf[10];




int main(int argc, char **argv)
{
	char *buffer;
	int file;
	 
	file=spi_init("/dev/spidev1.0"); //dev

	buffer=(char *)spi_read_lm74(file); 
	 
	close(file);

	int value = (buffer[0] << 8 ) | buffer[1];
	if ( value < 0 )
	{
		value = (int)(value^(-1));
	}
	value >>= 7; // unit 1 
	printf("%d \n", value);

	return 0;
}
