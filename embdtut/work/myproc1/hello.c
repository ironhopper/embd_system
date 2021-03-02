#include <stdio.h>
extern void bye(void);

static void hello(void){
	bye();
	printf("Hello World.\n");
}
