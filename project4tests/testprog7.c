#include<stdbool.h>
#include<stdio.h>
int main (int argc, char *argv[]) {
	char word[10];
	while (true) {
		int ret = scanf("%9s", word);
		if (ret == EOF) {
			break;
		}
	}
	return 0;
}
