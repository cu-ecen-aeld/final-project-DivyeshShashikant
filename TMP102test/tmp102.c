/*@author: Divyesh  Patel
 *@filename: tmp102.c
 *@brief: Test code to read raw values from the temperature sensor connected
 	* to i2c-2 bus on the beaglebone black and convert it to celsius values
*/


#include<stdio.h>
#include<stdlib.h>
#include<linux/i2c-dev.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<time.h>

int main()
{

	time_t rawtime;
	struct tm* timeinfo;
	
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	
	
	
	//open file to read data on the i2c bus
	int file;
	char *bus = "/dev/i2c-2";

	if((file = open(bus, O_RDWR)) < 0)
	{
		printf("FAILED to read bus\n");
		exit(1);
	}
	
	//get the i2c device, TMP102 I2C address is 0x48
	ioctl(file, I2C_SLAVE, 0x48);

	//send temperature regiser command
	char config[1] = {0};
	config[0] = 0x00;
	write(file, config, 1);
	sleep(1);
	
	

	float temp, final_temp, fahrenheit;
	
	unsigned char read_data[2] = {0};
	
	//read 2 bytes of temperature data
	if(read(file, read_data, 2)!=2)
	{
		printf("Error: Could not read byte\n");
	}
	else
	{
		temp = ((read_data[0] << 4 ) | ( read_data[1] >> 4)); //convert data to 12 bit temperature values
	}
	
	
	final_temp = temp * 0.0625; //final temperature values
	fahrenheit = (1.8 * final_temp) + 32;
	
	printf("The temperature in celsius %fC %s\r\n", final_temp, asctime(timeinfo)); //print C temperature data to terminal
	printf("The temperature in fahrenheit %fF %s\r\n", fahrenheit, asctime(timeinfo)); //print F temperature to terminal
	return 0;

}
