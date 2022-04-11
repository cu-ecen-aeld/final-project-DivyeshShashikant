#include<stdio.h>
#include<stdlib.h>
#include<linux/i2c-dev.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{

	int file;
	char *bus = "/dev/i2c-2";

	if((file = open(bus, O_RDWR)) < 0)
	{
		printf("FAILED to read bus\n");
		exit(1);
	}

	ioctl(file, I2C_SLAVE, 0x48);

	char config[1] = {0};
	config[0] = 0x00;
	write(file, config, 1);
	sleep(1);
	
	float temp, final_temp;
	int negative_temp;

	char read_data[2] = {0};
	
	//if((read_data[0] >> 7) == 1)
	//{
	//	negative_temp = 1;	
	//}
	

	if(read(file, read_data, 2)!=2)
	{
		printf("Error: Could not read byte\n");
	}
	else
	{
		temp = ((read_data[0] << 4 ) | ( read_data[1] >> 4));
	}

	if(negative_temp)
	{
		printf("negative temperature\n");
		final_temp = (( 257 - (temp * 0.0625))  * (-1));
		return -1 ;
	}
	
	final_temp = temp * 0.0625;

	printf("The temperature in celsius %f", final_temp);

	return 0;

}
