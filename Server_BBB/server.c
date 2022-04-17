/*@author: Divyesh  Patel
 *@filename: server.c
 *@brief: Code to open a socket on port 3457 and send temperature sensor values
 *	  to client connected on the respective port
*/



#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<signal.h>
#include<syslog.h>
#include<arpa/inet.h>
#include<sys/queue.h>
#include<pthread.h>
#include<stdbool.h>
#include<time.h>
#include<linux/fs.h>
#include<sys/times.h>
#include<linux/i2c-dev.h>
#include<sys/ioctl.h>
#include<sys/wait.h>


#define PORT "3457"     //port  
#define BACKLOG 10      //pending connections
#define MAX_DATA_BYTES 100    



void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int get_temp_values(void)
{
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
	//sleep(1);
	
	

	float temp, final_temp;
	int negative_temp;

	char read_data[2] = {0};
	
	
	
	//read 2 bytes of temperature data
	if(read(file, read_data, 2)!=2)
	{
		printf("Error: Could not read byte\n");
	}
	else
	{
		temp = ((read_data[0] << 4 ) | ( read_data[1] >> 4)); //convert data
	}
	
	if((read_data[0] >> 7) == 1) //check for negative temperature values
	{
		negative_temp = 1;	
	}

	if(negative_temp)
	{
		printf("negative temperature\n");
		final_temp = (( 257 - (temp * 0.0625))  * (-1)); //convert data
		return final_temp ;
	}
	
	final_temp = temp * 0.0625; //final temperature values

	printf("The temperature in celsius %f", final_temp); //print data to terminal
	
	return final_temp;

}







int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	float 		temp = 0;	// temparature data
	char 		data_buf[] = "test string from server\n";	// the string that server sends to client
	
	char *ptr = NULL;
	
	while(1) 
	{  	// main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) 
		{
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);
		
		char *receive = "READTEMP";
		
		while(1)
		{
			memset(data_buf, 0, MAX_DATA_BYTES);
			
			
			strcpy(data_buf, "server send: ");
			
			 
			
			//sprintf(data_buf, "%.2f", temp);
			
			int total_bytes = strlen(data_buf)+1;
			int bytes_sent = 0;
			
			//ptr = test_buf;
			int ret = 0;
			
			
			
			do
			{
				bytes_sent = send(new_fd, data_buf, total_bytes, 0);
				if (bytes_sent == -1)
				{
					perror("send");
					close(new_fd);
					exit(-1);
				}
				
				total_bytes -= bytes_sent;
				
			}while(total_bytes != 0);
			


			//ptr = test_buf;
			ret = 0;
/*			while(1){*/
/*				ret = recv(new_fd, ptr, 1, 0);*/
/*				if(ret == -1){*/
/*					syslog(LOG_ERR, "Error client recv: %d", errno);*/
/*					closelog();*/
/*					close(new_fd);*/
/*					// rc = FAIL_READ;*/
/*					exit(-1);*/
/*				}*/

/*				if(*ptr == '\0'){*/
/*					//End of reading string*/
/*					// rc = SUCCESSFUL_READ;*/
/*					break;*/
/*				}*/
/*				// else if(recv_cnt >=(MAXDATALEN-1)){*/
/*				// 	syslog(LOG_ERR, "Client reciver buffer size exceeded");*/
/*				// 	printf("Buffer size exceeded\n");*/
/*				// 	rc = OVERFLOW;*/
/*				// 	break;*/
/*				// }   */
/*				else{*/
/*					//Increment count if received a valid char successfully*/
/*					// recv_cnt++;                                     //Maintain the receiver count to keep check*/
/*					ptr++;*/
/*				}*/
/*			}*/

			while(1)
			{	
				char test_buf[MAX_DATA_BYTES];
				memset(data_buf, 0, sizeof data_buf);
				memset(test_buf, 0, sizeof test_buf);
		
				ptr = test_buf;
				printf("receivinf again\r\n");	
				while(1)
				{
					ret = recv(new_fd, ptr, 1, 0);
					if(ret == -1)
					{
						syslog(LOG_ERR, "Error client recv: %d", errno);
						closelog();
						close(new_fd);
						// rc = FAIL_READ;
						exit(-1);
					}
					if((*ptr == '\r') || (*ptr == '\0'))
					{
						printf("newline pressed\r\n");
						break;
					}
					else
					{
						ptr++;
					}
				}
				
				printf("test_buf is %s\r\n",test_buf);
					
				if(strcmp(test_buf, receive)==0)
				{
					//char *sendt = "HI";
					printf("received command is %s\r\n", test_buf);
					//sprintf(data_buf, "%s", sendt);
					temp = get_temp_values();
					sprintf(data_buf, "%.2f", temp);
					total_bytes = strlen(data_buf)+1;
					do
					{
						bytes_sent = send(new_fd, data_buf, total_bytes, 0);
						if (bytes_sent == -1)
						{
							perror("send");
							close(new_fd);
							exit(-1);
						}
						
						total_bytes -= bytes_sent;
						
					}while(total_bytes != 0);
					
					//printf("feedback from client: %s\n", test_buf);
										
				}
				else
				{
					//Increment count if received a valid char successfully
					printf("invalid command\r\n");
					char *sendt = "enter valid command";
					printf("received command is %s\r\n", test_buf);
					sprintf(data_buf, "%s", sendt);
					total_bytes = strlen(data_buf)+1;
					do
					{
						bytes_sent = send(new_fd, data_buf, total_bytes, 0);
						if (bytes_sent == -1)
						{
							perror("send");
							close(new_fd);
							exit(-1);
						}
						
						total_bytes -= bytes_sent;
						
					}while(total_bytes != 0);
					
					//printf("feedback from client: %s\n", test_buf);
				}
			}
			//printf("feedback from client: %s\n", test_buf);
		}
	}

	return 0;
}
