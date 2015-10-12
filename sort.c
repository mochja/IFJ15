#include <stdio.h>
#include <string.h>



char* sort(char* s){
	int gap = strlen(s)/2;	
	int tmp,i,j;

	while(gap > 0){
		for(i = gap; i < strlen(s); i++){
			j=i;
			tmp=s[i];
			while( s[j-gap]>tmp && j>=gap ){
				s[i] = s[j-gap];
				j -= gap;
			}
			s[j] = tmp;
		}

	   gap = (gap==2) ? 1 : (int)(gap/2.2);	
	}

	return s;
}
