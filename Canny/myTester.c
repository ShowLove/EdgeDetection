#include <stdlib.h>
#include <string.h>
#include <stdio.h>                          /* Sobel.c */
#include <math.h>

int main()
{
	int ones = 0xFFFF;

	int number = 0377;

	printf("%04x\n",ones);
	printf("%04x\n",number);
	printf("%04x\n",number&ones);

	return 0;
}