#include <stdio.h>
#include <string.h>

#define ALBHABETH_LENGTH 256

void do_delta(int delta[], char search[], char searchLen){
	int i;
	for(i=0; i < ALBHABETH_LENGTH; i++){
		delta[i] = searchLen;
	}

	for(i=0; i <= searchLen-1 ; i++){
		delta[(int)search[i]]=searchLen-i-1;
	}
}

int find(char s[], char search[]){
	int i, sLen = strlen(s), searchLen = strlen(search);
	int delta[ALBHABETH_LENGTH];

	do_delta(delta,search,searchLen);

	if(searchLen == 0)	return 0;

	i = searchLen - 1;
	while(i < sLen){
		int j = searchLen-1;
		
		while(j >= 0 && s[i] == search[j]){
			j--;
			i--;
		}
		
		if( j < 0)	return i+1;
		
	i+=delta[s[i]];
	}
}