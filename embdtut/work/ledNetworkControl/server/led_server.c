#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
/* headers and declaration */

#define LISTENING_PORT	50001 // server port
//thread
void *listening_thread(void *arg);
void *client_thread(void *arg);
void error_handing(char *message);

#define	CMD_NONE	0
#define CMD_CONTROL	1
#define MAX_CLIENT_NUM	10 // maximum client number that can be attached to a server
/* declaration */

struct led_control {
// present led information state

	// bit 0 => first led, bit 2 => second led, bit 7 => 7th led.  
	// set(1) => on, reset(0) => off
	int ledStatus;
	int newCmdFlag; // flag which informs a new coming commands.
	pthread_mutex_t* mutex; 	
};
struct server_info {
// private information of server

	struct led_control ledctl;
	int threadStop;
	int serverlistenSock; // listen socket
};

struct info_for_client {
// information of connected clients

	struct server_info* pInfo;
	int ownSockIndex;
	int clientSock[MAX_CLIENT_NUM];
};

struct server_info gInfo;
struct info_for_client clientInfo;
/* declaration of necessary global variable  */
void alarmLedControl(unsigned char data) {
	char strCmd[80];
	int result;
	sprintf(strCmd,"./ledtest 0x%02x",data);
	result = system(strCmd);
}
/* 
  Using system function, 
  making led control function 
  that is calling the 'ledtest' program made previously 
  */

void handle_kill_signal() {
	gInfo.threadStop = 1;
	if( gInfo.serverlistenSock != -1)
		close(gInfo.serverlistenSock);
	signal(SIGINT, SIG_DFL); // SIG_DFL => do the default action of this signal
	alarmLedControl(0x00); // all off
}
/* when stopping with "Ctrl+C", close the socket and exit the program. */
void init(struct server_info* data){
	alarmLedControl(0x00); // LED off
	data->ledctl.ledStatus = 0x00;
	data->ledctl.newCmdFlag = CMD_NONE;
	data->threadStop = 0;
	data->serverlistenSock = 01;
	data->ledctl.mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(data->ledctl.mutex, NULL); //mutex init
}
/* variables and LED initialization  */

int main(int argc, char *argv[]){
	struct server_info* pInfo = &gInfo;
	pthread_t pthreadListening; // accept client
	struct sigaction action;
	int *pThreadReturn;
	init(pInfo);

	// create listening thread
	if(pthread_create(&pthreadListening, NULL, listening_thread,pInfo)== -1){
		error_handing("pthread_create() error\n");
		return -1;
	}
	// Add signal handler to terminate server
	action.sa_handler = handle_kill_signal;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGINT, &action, NULL); // ctrl +c
	// main thread loop
	while(!pInfo->threadStop){
		if(pInfo->ledctl.newCmdFlag == CMD_CONTROL) { // if new command cin,
			// run the command
			pInfo->ledctl.newCmdFlag = CMD_NONE;
			alarmLedControl(pInfo->ledctl.ledStatus);
		}
		usleep(100000); // check every 100ms (100,000 us)
	}
	printf("LED control server closed.\n");
	return 0;
}
/* 
 generating and executing the listening Thread, 
 execute the command by checking the new commands from Client every 100ms
 */

//control Client acception manage
void *listening_thread(void *arg){
	pthread_detach(pthread_self()); 
	// release resources independently of this thread.(when thread ends)

	struct server_info* pInfo = (struct server_info*)arg; 
	// data passed when creating a thread.
	
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;

	pthread_t pthreadClient; // thread receive data

	struct info_for_client* pclientInfo = &clientInfo;
	int i;
/*
 * initialize the initial varible with the listening thread
 */
	pclientInfo->pInfo = pInfo;
	for(i=0;i<MAX_CLIENT_NUM;i++){
		pclientInfo->clientSock[i] = -1;
	}
	pclientInfo->ownSockIndex = 0;
	/* 
	 * initialize client related information
	 */


	// get server socket
	serv_sock = socket(PF_INET,SOCK_STREAM,0); //tcp => 0
	if(serv_sock == -1) {  
		error_handing("socket() error\n");  
	}
	/*  create server socket  */
	
	// socket option - reuse binded address
	int option =1;
	setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));

	memset(&serv_adr,0,sizeof(serv_adr));

	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // get ip address
	serv_adr.sin_port = htons(LISTENING_PORT); // get port num

	// bind .  for the connection request of control board
	if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1) {
		error_handing("bind() error\n");
	}
	/**  
	 * Bind the server socket 
	 * created with the server IP and port information.
	 */
	
	// listen
	if(listen(serv_sock,10)==-1){
		error_handing("listen() error\n");
	}
	clnt_adr_sz = sizeof(clnt_adr);
	/** 
	 * wating for connection request from client
	 */
	while(!pInfo->threadStop){
		printf("ready to accep client...\n");
		// accept when requesting client connection
		clnt_sock = accept(serv_sock,
			(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		printf("clnt_sock : %d\n",clnt_sock);
		if(clnt_sock <0){
			error_handing("accept() error");
			continue;
		}
	
		// check if the maximum number of clients has been exceeded
		if(pclientInfo->ownSockIndex >= MAX_CLIENT_NUM){
			close(clnt_sock);
			continue;
		}
		// Storing clients socket information 
		// that accepts the client information structure
		printf("accept() success \n");
		pclientInfo->clientSock[pclientInfo->ownSockIndex] = clnt_sock;
		pclientInfo->ownSockIndex++;

		// accept and create object and create thread
		if(pthread_create(
		&pthreadClient,NULL,client_thread,(void*)pclientInfo)==-1){
			error_handing
			("acceptClient_thread -- pthread_create() error\n");
			close(clnt_sock);
		}

		printf("Connected Client IP : %s\n",
			inet_ntoa(clnt_adr.sin_addr));
		printf("Client Port Num : %d\n\n",ntohs(clnt_adr.sin_port));
	}
	/** 
	 * 
	 */
	close(serv_sock);
	for(i=0;i<MAX_CLIENT_NUM;i++){
		if(pclientInfo->clientSock[i] != -1){ 
		// closing the currently connected client socket 
		// when it is closed
			close(pclientInfo->clientSock[i]);
			pclientInfo->clientSock[i] = -1;
		}
	}
	return 0;
}
/** 
 * Exit when pinfo->threadStop stop flag  appears.
 */
#define	RX_DATA_MAX	2048

/************************************************
msg => 3 byte
msg[0] => 0xFE	header
msg[1] => command
msg[2] => data
************************************************/

#define PKT_INDEX_HEADER	0
#define PKT_INDEX_CMD	1
#define PKT_INDEX_DATA	2

#define PKT_LEN	3
#define PKT_HEADER_VALUE	0xFE

#define PKT_CMD_SET_LED	0x10
#define PKT_CMD_GET_LED	0x11
#define PKT_CMD_GET_LED_RES	0x91

/** 
 * Command data protocol definition
 */
void *client_thread(void *arg){
	int clnt_sock;
	struct info_for_client* pclientInfo = arg;
	int clnt_sock_index = pclientInfo->ownSockIndex-1;
	clnt_sock = pclientInfo->clientSock[clnt_sock_index];

	char rcv_buf[RX_DATA_MAX];
	int readBufSize,writeBufSize;

	pthread_detach(pthread_self());

	while(!pclientInfo->pInfo->threadStop){
		readBufSize = read(clnt_sock,rcv_buf,RX_DATA_MAX);

		if(readBufSize>0){
			// Check message protocol format
			if( (readBufSize != PKT_LEN) || 
				(rcv_buf[PKT_INDEX_HEADER] != PKT_HEADER_VALUE)){
					continue;
			}
			// success
			if(rcv_buf[PKT_INDEX_CMD] == PKT_CMD_SET_LED){
				// LED set command
				pclientInfo->pInfo->ledctl.ledStatus = 
					rcv_buf[PKT_INDEX_DATA];
				pclientInfo->pInfo->ledctl.newCmdFlag =
					CMD_CONTROL;
			}
			else if(rcv_buf[PKT_INDEX_CMD]==PKT_CMD_GET_LED){
				// LED get command
				
				// echo TX
				rcv_buf[PKT_INDEX_CMD]=PKT_CMD_GET_LED_RES;
				rcv_buf[PKT_INDEX_DATA]=
				pclientInfo->pInfo->ledctl.ledStatus;
				writeBufSize = 
				write(clnt_sock,rcv_buf,PKT_LEN);
				printf("Send data len: %d\n",writeBufSize);
				if(writeBufSize<0){
					printf("echo write() error\n");
					break;
				}

			}
		}
		else if(readBufSize==0){
			printf("readBufSize: 0\n");
			break;
		}
		else { // readBufSize <0
			printf("client(%d) write error.\n",clnt_sock);
			break;
		}
	}
	printf("close clnt_sock.:%d\n\n",clnt_sock);
	close(clnt_sock);
	pclientInfo->clientSock[clnt_sock_index] = -1;

	return 0;
}

/** 
 * the created client thread performs data transfer/receive based on the 
 * message protocol.
 */

void error_handing(char *message){
	fputs(message,stderr);
	fputc('\n',stderr);
}

/** 
 * error message handler
 */


