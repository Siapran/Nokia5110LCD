#include "stdio.h"

int main(int argc, char const *argv[])
{
	printf("%x\n", 0xFF & ~(1 << 3));
	return 0;
}