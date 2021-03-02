#include <stdio.h>
#define ABC 123
#define hello 456
extern void bye(void);

void hello(){
	printf("\thello!\n");
}
int main(void){
	bye();
	hello();
	return(0);
}
