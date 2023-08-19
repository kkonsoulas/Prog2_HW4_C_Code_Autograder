#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
int main (int argc, char *argv[]) {
	int sig;
	int i,j;
	for (i=1; i<argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");
	fflush(stdout);
	scanf("%d", &sig);
	kill(getpid(), sig);
	return 0;
}
