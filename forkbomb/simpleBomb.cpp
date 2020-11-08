#include <unistd.h>
#include <stdio.h>

void bomb(){
	int n = fork();	
	bomb();
}

int main()
{
	bomb();
	return 0;
}
