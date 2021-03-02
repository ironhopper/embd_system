#include <stdio.h>
#define ABC 123
#define hello 456
extern void bye(void);

void hello(){
	printf("\thello!\n");
	printf("\tthis is latest update!! 1.2?\n");
}
int main(void){
	bye();
	hello();
	return(0);
}
