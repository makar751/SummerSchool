#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>

#define BUF_SIZE 10


int main(){
	char buffer[10];
	int f;

	if ( (f = open("/dev/testmodu", O_RDONLY)) == -1){
		printf("NO OPEN\n");
		return 1;
	}

	char b;

	while (read(f, &b, sizeof(b))){
		printf("B = %s\n", &b);
	}

	close(f);
}