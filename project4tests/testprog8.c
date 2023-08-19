#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
int main (int argc, char *argv[]) {
	int maxlen = atoi(argv[1]);
	char *word = (char *) calloc(maxlen+1, 1) ;
	char *doubleword = (char *)calloc(2*maxlen+1, 1);
	char format[15];
	sprintf(format, "%%%ds", maxlen);
	scanf(format, word);
	strcpy(doubleword, word);
	strcat(doubleword, word);
	printf("%s\n", doubleword);
	free(word);
	free(doubleword);
	return 0;
}
