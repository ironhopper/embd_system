#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>

/**  
 * header definition
 */
#define SERVER_PORT	50001
void *read_thread(void *arg);
void GetLEDStatus(int consocket);
void SendSetLED(int consocket,unsigned char ledctrValue);
int gThreadStop =0;

/** 
 * Port, variable, and function definitions
 */
int main(int argc,char *argv[]){
	pthread_t pthreadread; // read thread
	int socketClient;

	// check port info
	if(argc != 2){ // server ip
		printf("Usage: led_client <server ip>\n");
		printf("ex)./led_client 192.168.10.38\n");
		exit(1);
	}
	// argument ip format check
	struct sockaddr_in serverAddr;
	if(inet_pton(AF_INET,argv[1],&(serverAddr.sin_addr)) <0) { // wrong format
		// get ip address from string
		printf("IP format:%s is wrong.\n",argv[1]);
		printf("Usage: led_client <server ip>\n");
		printf("ex)./led_client 192.168.10.38\n");
		exit(1);
	}
	/**  
	 * check the input format, if the format is incorrect, output the example.
	 */
	socketClient = socket(PF_INET,SOCK_STREAM, 0); // tcp => 0 generating socket
	if(socketClient == -1){
		printf("socket() error");
		return -1;
	}
	serverAddr.sin_family = AF_INET;
	printf("Connecting IP: %s\n", inet_ntoa(serverAddr.sin_addr));
	serverAddr.sin_port = htons(SERVER_PORT); // input port
	/* Set all bits of the padding field to 0  */
	memset(serverAddr.sin_zero,'\0',sizeof(serverAddr.sin_zero));
	// request connection to server
	if(connect(socketClient,(struct sockaddr*)&serverAddr,sizeof(serverAddr))==-1){
		printf("connect() error\n");
		close(socketClient);
		return 0;
	}
	else{
		printf("connect() success\n");
	}
	/**  
	 * connect to server after creating socket
	 */

	// create read waiting thread
	if(pthread_create(&pthreadread,NULL,read_thread,&socketClient)== -1){
		printf("read_thread() error\n");
		return -1;
	}

	/**  
	 * gen
	 */
	int setValue;
	char inputCmd;
	int scanRelt;
	// main thread loop
	while(!gThreadStop){
		printf("\neXit:x, Setled:s,Getled:g  select menu:");
		scanRelt = scanf("%c",&inputCmd);
		printf("\nscanRelt:%d, inputCmd:%c\n",scanRelt,inputCmd);
		while(getchar()!='\n');
		switch(inputCmd){
			case 's':
				printf("\ninput the Led Set value(hexvalue):");
				scanRelt = scanf("%x",&setValue);
				while(getchar()!= '\n');
				SendSetLED(socketClient,(unsigned char)setValue);
				printf("\nSetValue:0x%02X\n",setValue);
				break;
			case 'g':
				GetLEDStatus(socketClient);
				usleep(200000);
				break;
			case 'x':
				close(socketClient);
				gThreadStop=1;
				printf("bye ~~\n");
				return 0;
			default:
				printf("wrong menu input:%c\n",inputCmd);
				continue;
		}
	}
	return 0;
}

/**  
 * scanf
 */

#define RX_DATA_MAX 2048
/************************************************************
 msg => 3 byte
 msg[0] => 0xFE	 header
 msg[1] => command
 msg[2] => data
************************************************************/

#define PKT_INDEX_HEADER 0
#define PKT_INDEX_CMD 1
#define PKT_INDEX_DATA 2

#define PKT_LEN 3
#define PKT_HEADER_VALUE 0xFE

#define PKT_CMD_SET_LED 0x10
#define PKT_CMD_GET_LED 0x11
#define PKT_CMD_GET_LED_RES 0x91


/** 
 * protocol
 */
void *read_thread(void *arg){
	int socketClient = *(int*)arg;

	unsigned char rcv_buf[RX_DATA_MAX];
	int readBufSize;

	pthread_detach(pthread_self());

	while(!gThreadStop){
		readBufSize = read(socketClient, rcv_buf, RX_DATA_MAX);
		if(readBufSize>0){
			if(readBufSize<PKT_LEN){
				printf("packet is short:%d\n",readBufSize);
				continue;
			}
			printf("[%02X][%02X][%02X]\n",rcv_buf[0],rcv_buf[1],rcv_buf[2]);
			if((readBufSize != PKT_LEN)|| (rcv_buf[PKT_INDEX_HEADER] != PKT_HEADER_VALUE)){
				continue;
			}
			//success
			if(rcv_buf[PKT_INDEX_CMD] == PKT_CMD_GET_LED_RES){
				printf("LED status:0x%02X\n",rcv_buf[PKT_INDEX_DATA]);
			}
		}
		else if(readBufSize == 0){
			printf("readBufSize: 0\n");
		}
		else { // readBufSize <0
			printf("client(%d) read error.\n", socketClient);
			break;
		}
	}
	printf("close clnt_sock.:%d\n",socketClient);
	close(socketClient);

	return 0;
}

/** 
 * Read ~~~ PKT_CMD_GET_LED_RES
 */

void SendSetLED(int consocket, unsigned char ledctrValue){
	unsigned char tx_buf[PKT_LEN];
	int writeBufSize;
	tx_buf[PKT_INDEX_HEADER] = PKT_HEADER_VALUE;
	tx_buf[PKT_INDEX_CMD] = PKT_CMD_SET_LED;
	tx_buf[PKT_INDEX_DATA] = ledctrValue;

	writeBufSize = write(consocket,tx_buf,PKT_LEN);
	if(writeBufSize<0){
		printf("network error\n");
		close(consocket);
		gThreadStop= 1;
	}
	else if(writeBufSize != PKT_LEN){
		printf("Write error\n");
	}
}

/** 
 * Set LED function
 */

void GetLEDStatus(int consocket){
	unsigned char tx_buf[PKT_LEN];
	int writeBufSize;
	tx_buf[PKT_INDEX_HEADER] = PKT_HEADER_VALUE;
	tx_buf[PKT_INDEX_CMD] = PKT_CMD_GET_LED;
	tx_buf[PKT_INDEX_DATA] = 0;
	writeBufSize = write(consocket,tx_buf,PKT_LEN);
	if(writeBufSize<0){
		printf("network error\n");
		close(consocket);
		gThreadStop = 1;
	}
	else if(writeBufSize != PKT_LEN){
		printf("write error\n");
	}
}
/** 
 * present LED status command
 */
