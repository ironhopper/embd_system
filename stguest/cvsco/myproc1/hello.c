#include <stdio.h>
extern void bye(void);

static void hello(void){
	bye();
	printf("Hello World11.\n");
	printf("Hello World22.\n");
}
