#include "stdafx.h"
#include "BitBoard.h"
#include <stdlib.h>

void printBit(uint64 in) {
	char buf[1000];
	itoa(in, buf, 2);
	printf("%s,", buf);
	printf("\n");
}
