#include <unistd.h>
#include <stdio.h>

int main()
{
	int n = fork();
	if(n > 1){
		printf("I am Parent\n");
	}
	else{
		printf("I am Child\n");
	}
	return 0;
}
