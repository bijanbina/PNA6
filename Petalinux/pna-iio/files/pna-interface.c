#include "pna-interface.h"
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/ioctl.h>

int interface_id = PNA_INTERFACE_CONSOLE;
int connfd, sockfd;

void pna_printf(char *format, ...)
{
    va_list args;
    va_start(args, format);

    if(interface_id == PNA_INTERFACE_CONSOLE)
    {
        vprintf(format, args);
    }
    else if(interface_id == PNA_INTERFACE_TCP)
    {
        char buffer[1000];
        vsprintf(buffer, format, args);
        write(connfd, buffer, strlen(buffer));

    }
    va_end(args);
}

int pna_gets(char *buffer, int max_len)
{
    if(interface_id == PNA_INTERFACE_CONSOLE)
    {
        int ret;
        ret = gets(buffer);
        // return ret;
        return 0;
    }
    else if(interface_id == PNA_INTERFACE_TCP)
    {
        int ret, size_data = 0;
        int end_of_string = 0;
        do
        {
            ret = read(connfd, buffer + size_data, max_len);
            for(int i=size_data; i<(ret+size_data); i++)
            {
                // printf("%d: 0x%x %c\r\n", i, buffer[i], buffer[i]);
                if(buffer[i] == '\n')
                {
                    buffer[i] = 0;
                    end_of_string = 1;
                    break;
                }
            }
            size_data += ret;
        }
        while(!end_of_string);
        return size_data;
    }
    else
    {
        return -1;
    }
    
}

/*
 * @return: -1 if escape character is seen
 */
// FIXME: should be implemented with timer not escape char
size_t pna_read(unsigned char *data, int len, int escape_set)
{
    if(interface_id == PNA_INTERFACE_CONSOLE)
    {
    	// 1 denote number of bytes in each read
    	int sum = 0;
		while( sum<len )
		{
//			int ret = fread(data + sum, 1, len - sum, stdin);
			int ret = read(STDIN_FILENO, data + sum, len - sum);
			sum += ret;
			if( escape_set )
			{
				if( pna_is_escaped(data + sum - 3) )
				{
					return 1;
				}
			}
		}
    }
    else if( interface_id==PNA_INTERFACE_TCP )
    {
    	int sum = 0;
    	while( sum<len )
        {
    		int ret = read(connfd, data + sum, len - sum);
    		sum += ret;
    		if(escape_set)
    		{
    			if(pna_is_escaped(data + sum - 3))
    			{
    				return 1;
    			}
    		}
        }
    }
    return 0;
}

int pna_is_escaped(unsigned char *data)
{
//	pna_printf("***%c%c%c", data[0], data[1], data[2]);
	if(data[0] == 'k' && data[1] == '\r' && data[2] == '\n')
	{
		return 1;
	}
	else if(data[1] == 'k' && data[2] == '\n')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void pna_write(unsigned char *data, int len)
{
    if(interface_id == PNA_INTERFACE_CONSOLE)
    {
        fwrite(data, 1, len, stdout);
    }
    else if(interface_id == PNA_INTERFACE_TCP)
    {
        int write_len = send(connfd, data, len, 0);
//        fixme : use usleep
        //usleep(100);
//	     printf("ERROR: %d \n",    write_len);
        if(write_len != len)
        {
		     printf("ERROR: written bytes %d is not the same as len=%d \n",
		    		    write_len, len);
        }
    }
}

void pna_set_interface(int id)
{

    if(id == PNA_INTERFACE_TCP)
    {
        ;
    }
    else
    {
        pna_close_interface();
    }
    pna_init_interface(id);
}

void pna_init_interface(int id)
{
    interface_id = id;
    if(interface_id == PNA_INTERFACE_TCP)
    {
        struct sockaddr_in servaddr, cli;
        printf("\r\r\r\r\r");

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            printf("socket creation failed...\r\n>>");
            return;
        }
        else
        {
            printf("Socket successfully created...\n");
        }

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT_NUMBER);

        // reuse address in case of time wait in closing socket
        int option = 1;
        int ret = setsockopt(sockfd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&option, sizeof(option));
        if(ret < 0)
        {
            printf("setsockopt failed\r\n>>");
            return;
        }

        ret = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
        if (ret != 0)
        {
            printf("socket bind failed...\r\n>>");
            exit(0);
        }
        else
        {
            printf("Socket successfully binded...\n");
        }

        // Now server is ready to listen and verification
        ret = listen(sockfd, 5);
        if (ret != 0)
        {
            printf("Listen failed...\r\n>>");
            exit(0);
        }
        else
        {
        	pna_printf("Server listening...\r\n>>");
        }

        // Accept the data packet from client and verification
        socklen_t len;
        len = sizeof(cli);
        connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
        if (connfd < 0) {
            printf("server acccept failed...\n");
            exit(0);
        }
        else
        {
            printf("server acccept the client...\n");
            int iMode = 0;
            ioctl(connfd, FIONBIO, &iMode);
        }
    }
}

int getSO_ERROR(int fd)
{
    int err = 1;
    socklen_t len = sizeof err;
    if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&err, &len))
    {
        printf("getSO_ERROR\r\n");
    }
    if (err)
    {
        errno = err;              // set errno to the socket SO_ERROR
    }
    return err;
}

void closeSocket(int fd)
{      // *not* the Windows closesocket()
    if (fd >= 0)
    {
        getSO_ERROR(fd); // first clear any errors, which can cause close to fail
        if (shutdown(fd, SHUT_RDWR) < 0) // secondly, terminate the 'reliable' delivery
        {
            if (errno != ENOTCONN && errno != EINVAL) // SGI causes EINVAL
            {
                printf("shutdown\r\n");
            }
        }
        if (close(fd) < 0) // finally call close()
        {
            printf("close\r\n");
        }
   }
}

void pna_close_interface()
{
    if(interface_id == PNA_INTERFACE_TCP)
    {
        closeSocket(connfd);
        closeSocket(sockfd);
    }
}
