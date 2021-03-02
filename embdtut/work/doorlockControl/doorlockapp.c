#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/times.h>
#include <signal.h>
#include <sys/ioctl.h> // for ioctl
#include <fcntl.h> // for O_RDWR
#define INPUT_DEVICE_LIST "/dev/input/event"
#define MAX_BUFF 200
/** 
 * headers
 */

struct doorlockdata{
	int threadStop;
	int curIndex;
	int pass[6];
};
struct doorlockdata doorData;
/** 
 * global var
 */

int stopEvent =0;
void handle_kill_signal(){
	stopEvent =1;
	signal(SIGINT,SIG_DFL); // 
	signal(SIGTERM,SIG_DFL); // 
	signal(SIGHUP,SIG_DFL); // 
}
/** 
 * signal handler
 */

void alarmLedOn(int bEnable){ // 
	char strCmd[80];
	int result;
	if(bEnable){
		sprintf(strCmd,"./ledtest 0x01");
		result = system(strCmd);
	}
	else{
		sprintf(strCmd,"./ledtest 0x00");
		result = system(strCmd);
	}
}
/** 
 * ledtest system LED control
 */


void setStrTextLCD(int line, char* str){
	char strCmd[80];
	int result;
	sprintf(strCmd,"./textlcdtest %d '%s'",line,str);
	result = system(strCmd);
}
/** 
 * textlcdtest
 */

void setColorLED(int red,int green,int blue){
	char strCmd[80];
	int result;

	sprintf(strCmd,"./colorledtest %d %d %d",red,green,blue);
	result = system(strCmd);
}
/** 
 * colorledtest
 */

void buzzerOn(int bEnable){
	char strCmd[80];
	int result;
	if(bEnable){
		sprintf(strCmd,"./buzzertest 5");
		result = system(strCmd);
	}
	else{
		sprintf(strCmd,"./buzzertest 0");
		result = system(strCmd);
	}
}
/** 
 * buzzertest
 */

int fndNum[6]={
	0,0,0,0,0,0
};
/** 
 * default passwork
 */

void updateFnd(void){
	char strCmd[80];
	int result;

	sprintf(strCmd,"./fndtest '%d%d%d%d%d%d'",fndNum[0],fndNum[1],fndNum[2],fndNum[3],fndNum[4],fndNum[5]);
	result = system(strCmd);
}

void clearFnd(void){
	int i;
	for(i=0;i<6;i++){
		fndNum[i]=0;	
	}
	updateFnd();
	
}

void setnumberFnd(int nth, int num){
	fndNum[nth] = num;
	updateFnd();
}

/** 
 * fndtest
 */

int setnumAndInc(struct doorlockdata* pdoorData, int num){
	int result = 0;
	int i;
	setnumberFnd(pdoorData->curIndex,num);

	pdoorData->curIndex++;
	if(pdoorData->curIndex >= 6){
		result = 1;
		pdoorData->curIndex =0;
		sleep(1);
		for(i=0;i<6;i++){
			if(fndNum[i] != pdoorData->pass[i]){
				clearFnd();
				return -1;
			}
		}
		clearFnd();
		return result;
	}
	return result;
}
/** 
 * until input 6
 */

void init(struct doorlockdata* pdoorData){
	pdoorData->threadStop=0;
	pdoorData->curIndex=0;
	pdoorData->pass[0] = 2;
	pdoorData->pass[1] = 2;
	pdoorData->pass[2] = 2;
	pdoorData->pass[3] = 3;
	pdoorData->pass[4] = 3;
	pdoorData->pass[5] = 3;
	updateFnd();
	alarmLedOn(0);
	buzzerOn(0);
	setStrTextLCD(1, "init PWD 222333");
	setStrTextLCD(2,"door locked");
	setColorLED(0,0,100);
}
/** 
 * 222333
 */

void whenButtonDetect(struct doorlockdata* pdoorData, int btnNo){
	int result = setnumAndInc(pdoorData,btnNo);

	if(result== -1){
		buzzerOn(1);
		setColorLED(100,0,0);
		alarmLedOn(0);
		setStrTextLCD(1,"password fail");
		setStrTextLCD(2,"door locked ");
	}
	else if(result == 1){
		buzzerOn(1);
		setColorLED(0,100,0);
		alarmLedOn(1);
		setStrTextLCD(1,"password success");
		setStrTextLCD(2,"door opened ");
		sleep(3);

		buzzerOn(0);
		alarmLedOn(0);
		setColorLED(0,0,100);
		setStrTextLCD(1,"input password");
		setStrTextLCD(2,"door locked ");

	}
}
/** 
 *
 */

int main(int argc, char *argv[]){
	struct sigaction action;
	int fp;

	char inputfileName[MAX_BUFF+1];

	int readSize,inputIndex;

	struct input_event stEvent;
	struct doorlockdata* pdoorData = &doorData;
	// add signal
	action.sa_handler = handle_kill_signal;// signal enroll
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGINT,&action,NULL); // ctrl +c
	sigaction(SIGTERM, &action,NULL);
	sigaction(SIGHUP,&action,NULL);

	init(pdoorData);
	/** 
	 * exception
	 */

	if(argc != 2){
		printf("Usage: buttontest <input Number>\n");
		printf("firstly read the '/proc/bus/input/devices' file to check <input Number>\n");
		printf(">cat /proc/bus/input/device\n");
		return 1;
	}
	inputIndex = atoi(argv[1]);
	sprintf(inputfileName,"%s%d",INPUT_DEVICE_LIST,inputIndex);
	printf("read button event:%s\n",inputfileName);
	fp = open(inputfileName,O_RDONLY);
	if(-1 == fp){
		printf("%s file read error.\n",inputfileName);
		return 1;
	}

	/** 
	 * button input ready
	 */

	while(!stopEvent){
		readSize = read(fp,&stEvent,sizeof(stEvent));
		if(readSize != sizeof(stEvent)){
			continue;
		}
		// printf("type")

		if(stEvent.type == EV_KEY){
			printf("EV_KEY(");
			if(stEvent.value){
				printf("pressed\n");
				switch(stEvent.code){
					case KEY_HOME:
						printf("home key):");
						whenButtonDetect(pdoorData,0);
						break;
					case KEY_BACK:
						printf("back key):");
						whenButtonDetect(pdoorData,1);
						break;
					case KEY_SEARCH:
						printf("Search key):");
						whenButtonDetect(pdoorData,2);
						break;
					case KEY_MENU:
						printf("menu key):");
						whenButtonDetect(pdoorData,3);
						break;
					case KEY_VOLUMEUP:
						printf("Volume up key):");
						whenButtonDetect(pdoorData,4);
						break;
					case KEY_VOLUMEDOWN:
						printf("volume down key):");
						whenButtonDetect(pdoorData,5);
						break;
				}
			}
			else{
				printf("released\n");
			}
		}
		else { // EV_SYN
			// do nothing
		}
	}
	close(fp);
	return 0;
}
/** 
 *
 */
