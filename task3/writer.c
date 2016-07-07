
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>


int main(){
	char buffer[10] = "0123456789";
	int f;

	if ( (f = open("/dev/testmodu", O_WRONLY)) == -1){
		printf("NO OPEN\n");
		return 1;
	}
    printf("OPENED\n");
	int i;
	for (i = 0; i < 30; i++){

        printf("READY to WRITE\n");
		if (write(f, &i, sizeof(i)) )
			printf("%d: WRITED\n", i);
        else
        printf("NOT WRITE\n");
	}
	printf("EXITED\n");

	close(f);
}