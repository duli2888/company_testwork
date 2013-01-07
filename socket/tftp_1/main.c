#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/socket.h>
#include <netinet/in.h>

//add zyf 
#define  tftp_debug 1
#define ERROR -1
#define TIMEOUT 2000 /*amount of time to wait for an ACK/Data Packet in 1000microseconds 1000 = 1 second*/
#define MAXDATASIZE 1024 /* Maximum data size allowed */
int datasize = 512;
unsigned short int ackfreq = 1;
#define RETRIES 3 /* Number of times to resend a data OR ack packet beforing giving up */
#define DEFAULT_TFTP_HOST "192.168.0.100"
//end

/*
*This function is called when the client would like to download a file from the server.
*/

void tget (char *pFilename, struct sockaddr_in server, char *pMode, int sock)
{
  /* local variables */
  int len, server_len, opcode, i, j, n, tid = 0, flag = 1;
  unsigned short int count = 0, rcount = 0;
  unsigned char filebuf[MAXDATASIZE + 1];
  unsigned char packetbuf[MAXDATASIZE + 12];
  extern int errno;
  char filename[128], mode[12], *bufindex, ackbuf[512];
  struct sockaddr_in data;
  FILE *fp;			/* pointer to the file we will be getting */

  strcpy (filename, pFilename);	//copy the pointer to the filename into a real array
  strcpy (mode, pMode);		//same as above


  if (tftp_debug)
    printf ("branched to file receive function\n");

  fp = fopen (filename, "w");	/* open the file for writing */
  if (fp == NULL)
    {				//if the pointer is null then the file can't be opened - Bad perms 
      if (tftp_debug)
	printf ("Client requested bad file: cannot open for writing (%s)\n",
		filename);
      return;
    }
  else				/* File is open and ready to be written to */
    {
      if (tftp_debug)
	printf ("Getting file... (destination: %s) \n", filename);
    }
/* zero the buffer before we begin */
  memset (filebuf, 0, sizeof (filebuf));
  n = datasize + 4;
  do
    {
      /* zero buffers so if there are any errors only NULLs will be exposed */
      memset (packetbuf, 0, sizeof (packetbuf));
      memset (ackbuf, 0, sizeof (ackbuf));
	printf ("enter %d \n",count);  
      if (n != (datasize + 4))	/* remember if our datasize is less than a full packet this was the last packet to be received */
	{
	  if (tftp_debug)
	    printf
	      ("Last chunk detected (file chunk size: %d). exiting while loop\n",
	       n - 4);
	  len = sprintf (ackbuf, "%c%c%c%c", 0x00, 0x04, 0x00, 0x00);
	  ackbuf[2] = (count & 0xFF00) >> 8;	//fill in the count (top number first)
	  ackbuf[3] = (count & 0x00FF);	//fill in the lower part of the count
	  if (tftp_debug)
	    printf ("Sending ack # %04d (length: %d)\n", count, len);
	  if (sendto
	      (sock, ackbuf, len, 0, (struct sockaddr *) &server,
	       sizeof (server)) != len)
	    {
	      if (tftp_debug)
		printf ("Mismatch in number of sent bytes\n");
	      return;
	    }
	  if (tftp_debug)
	    printf ("The Client has sent an ACK for packet\n");
	  goto done;		/* gotos are not optimal, but a good solution when exiting a multi-layer loop */
	}

      count++;

      for (j = 0; j < RETRIES; j++)	/* this allows us to loop until we either break out by getting the correct ack OR time out because we've looped more than RETRIES times */
	{
	  server_len = sizeof (data);
	  errno = EAGAIN;	/* this allows us to enter the loop */
	  n = -1;
	  for (i = 0; errno == EAGAIN && i <= TIMEOUT && n < 0; i++)	/* this for loop will just keep checking the non-blocking socket until timeout */
	    {

	      n =
		recvfrom (sock, packetbuf, sizeof (packetbuf) - 1,
			  MSG_DONTWAIT, (struct sockaddr *) &data,
			  (socklen_t *) & server_len);
	      usleep (1000);
	    }
	  /*if(tftp_debug)
	     ip_port (data);  print the vlaue recived from the server */
	  if (!tid)
	    {
	      tid = ntohs (data.sin_port);	//get the tid of the server.
	      server.sin_port = htons (tid);	//set the tid for rest of the transfer 
	    }

	  if (n < 0 && errno != EAGAIN)	/* this will be true when there is an error that isn't the WOULD BLOCK error */
	    {
	      if (tftp_debug)
		printf
		  ("The server could not receive from the client (errno: %d n: %d)\n",
		   errno, n);

	      //resend packet
	    }
	  else if (n < 0 && errno == EAGAIN)	/* this is true when the error IS would block. This means we timed out */
	    {
	      if (tftp_debug)
		printf ("Timeout waiting for data (errno: %d == %d n: %d)\n",
			errno, EAGAIN, n);
	      //resend packet
	    }
	  else
	    {
	      if (server.sin_addr.s_addr != data.sin_addr.s_addr)	/* checks to ensure get from ip is same from ACK IP */
		{
		  if (tftp_debug)
		    printf
		      ("Error recieving file (data from invalid address)\n");
		  j--;
		  continue;	/* we aren't going to let another connection spoil our first connection */
		}
	      if (tid != ntohs (server.sin_port))	/* checks to ensure get from the correct TID */
		{
		  if (tftp_debug)
		    printf ("Error recieving file (data from invalid tid)\n");
		  len = sprintf ((char *) packetbuf,
				 "%c%c%c%cBad/Unknown TID%c",
				 0x00, 0x05, 0x00, 0x05, 0x00);
		  if (sendto (sock, packetbuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)	/* send the data packet */
		    {
		      printf
			("Mismatch in number of sent bytes while trying to send mode error packet\n");
		    }
		  j--;
		  continue;	/* we aren't going to let another connection spoil our first connection */
		}
/* this formatting code is just like the code in the main function */
	      bufindex = (char *) packetbuf;	//start our pointer going
	      if (bufindex++[0] != 0x00)
		printf ("bad first nullbyte!\n");
	      opcode = *bufindex++;
	      rcount = *bufindex++ << 8;
	      rcount &= 0xff00;
	      rcount += (*bufindex++ & 0x00ff);
	      memcpy ((char *) filebuf, bufindex, n - 4);	/* copy the rest of the packet (data portion) into our data array */
	      if (tftp_debug)
		printf
		  ("Remote host sent data packet #%d (Opcode: %d packetsize: %d filechunksize: %d)\n",
		   rcount, opcode, n, n - 4);
	      if (flag)
		{
		  if (n > 516)
		    datasize = n - 4;
		  else if (n < 516)
		    datasize = 512;
		  flag = 0;
		}
	      if (opcode != 3)	/* ack packet should have code 3 (data) and should be ack+1 the packet we just sent */
		{
		  if (tftp_debug)
		    printf
		      ("Badly ordered/invalid data packet (Got OP: %d Block: %d) (Wanted Op: 3 Block: %d)\n",
		       opcode, rcount, count);
/* sending error message */
		  if (opcode > 5)
		    {
		      len = sprintf ((char *) packetbuf,
				     "%c%c%c%cIllegal operation%c",
				     0x00, 0x05, 0x00, 0x04, 0x00);
		      if (sendto (sock, packetbuf, len, 0, (struct sockaddr *) &server, sizeof (server)) != len)	/* send the data packet */
			{
			  printf
			    ("Mismatch in number of sent bytes while trying to send mode error packet\n");
			}
		    }
		}
	      else
		{
		  len = sprintf (ackbuf, "%c%c%c%c", 0x00, 0x04, 0x00, 0x00);
		  ackbuf[2] = (count & 0xFF00) >> 8;	//fill in the count (top number first)
		  ackbuf[3] = (count & 0x00FF);	//fill in the lower part of the count
		  if (tftp_debug)
		    printf ("Sending ack # %04d (length: %d)\n", count, len);
		  if (((count - 1) % ackfreq) == 0)
		    {
		      if (sendto
			  (sock, ackbuf, len, 0, (struct sockaddr *) &server,
			   sizeof (server)) != len)
			{
			  if (tftp_debug)
			    printf ("Mismatch in number of sent bytes\n");
			  return;
			}
		      if (tftp_debug)
			printf ("The client has sent an ACK for packet %d\n",
				count);
		    }		//check for ackfreq
		  else if (count == 1)
		    {
		      if (sendto
			  (sock, ackbuf, len, 0, (struct sockaddr *) &server,
			   sizeof (server)) != len)
			{
			  if (tftp_debug)
			    printf ("Mismatch in number of sent bytes\n");
			  return;
			}
		      if (tftp_debug)
			printf ("The Client has sent an ACK for packet 1\n");
		    }
		  break;
		}		//end of else
	    }
	}
      if (j == RETRIES)
	{
	  if (tftp_debug)
	    printf ("Data recieve Timeout. Aborting transfer\n");
	  fclose (fp);
	  return;
	}
    }
  while (fwrite (filebuf, 1, n - 4, fp) == n - 4);	/* if it doesn't write the file the length of the packet received less 4 then it didn't work */
  fclose (fp);
  sync ();
  if (tftp_debug)
    printf
      ("fclose and sync unsuccessful. File failed to recieve properly\n");
  return;

done:
  fclose (fp);
  sync ();
  if (tftp_debug)
    printf ("fclose and sync successful. File received successfully\n");
  return;
}

int	req_packet (int opcode, char *filename, char *mode, char buf[])
{
	int len;
	len =
	sprintf (buf, "%c%c%s%c%s%c", 0x00, opcode, filename, 0x00, mode, 0x00);
	if (len == 0)
	{
	  printf ("Error in creating the request packet\n");	/*could not print to the client buffer */
	  exit (ERROR);
	}

	return len;
}

int tftp_get_file(char *hostip,char *filename ,char *buf){

	char  mode[12] = "octet";
	int port = 69;
	char opcode =1;
	int sock, server_len, len, opt;	//,n;
	//struct hostent *host;		/*for host information */
	 struct sockaddr_in server;	//, client; /*the address structure for both the server and client */
	/*Create the socket, a -1 will show us an error */

	if ((sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf ("Client: Socket could not be created");
		return 0;
	}
	 printf ("tftp_get_file\n");
	
	/*set the address values for the server */
	memset (&server, 0, sizeof (server));	/*Clear the structure */
	server.sin_family = AF_INET;	/*address family for TCP and UDP */
	
	server.sin_addr.s_addr=inet_addr(hostip);
	//server.sin_addr.s_addr = htonl (INADDR_ANY); /*use any address */
	server.sin_port = htons (port);	/*pick a free port */

	server_len = sizeof (server);	/*get the length of the server address */

	//memset (buf, 0, BUFSIZ);	/*clear the buffer */
	/* this is the first request message */
	len = req_packet (opcode, filename, mode, buf);
	printf ("the len is %d\n",len);
	//printf ("the server.sin_addr is %s\n",server.sin_addr.s_addr);
	if (sendto (sock, buf, len, 0, (struct sockaddr *) &server, server_len) != len)
	{
		perror ("Client: sendto has returend an error");
		exit (ERROR);
	}
	strncpy (mode, "octet", sizeof (mode) - 1);
	tget (filename, server, mode, sock);
	 

}

int main(void){
#if 1
	char hostip[128] ;	
	char filename[128];
	char buf[1024];
	strcpy(hostip,DEFAULT_TFTP_HOST);
	strcpy(filename,"hello");
#endif
	printf ("main\n");
	tftp_get_file( hostip, filename,  buf);
	return 0;

}
