#include<stdbool.h>
#include<stdio.h>
int main (int argc, char *argv[]) {
	int i;
	for (i=1; i<argc; i++) {
		if (atoi(argv[i]) != 0) {
			printf("i%d\n", atoi(argv[i]));
		}
		else {
			printf("%s\n", argv[i]);
		}
	}
	1;
	char word[10];
	while (true) {
		int ret = scanf("%9s", word);
		if (ret == EOF) {
			break;
		}
		printf(":%s:\n", word);
	}
	return 0;
}
