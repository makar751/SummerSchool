#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
 
#define SERVER "10.42.0.242"
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data
 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(void)
{
    struct sockaddr_in si_other;
    int s, j, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];

    long            resu, sen,rec; 
    struct timespec spec;

    FILE *f = fopen("results.xls", "w");
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
     
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 
    for(j=0;j<50000;j++)
    {
         
        //send the message
        if (sendto(s, "1", 1 , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
        clock_gettime(CLOCK_REALTIME, &spec);
        sen = spec.tv_nsec;
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        {
            die("recvfrom()");
        }
        clock_gettime(CLOCK_REALTIME, &spec);
        rec = spec.tv_nsec;
        resu=rec-sen;
        if (resu>0)
            fprintf(f, "%d;\n", rec-sen);
    }
    fclose(f);
    close(s);
    return 0;
}