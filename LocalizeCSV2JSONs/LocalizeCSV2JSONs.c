#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "Header.h"

int main(int argc, char **argv)
{
	int i;

	setlocale(LC_CTYPE, "ukr");

	printf("Convert localization table to json files\n");

	for (i = 1; i < argc; i++)
	{
		printf("%d: %s\n", i, argv[i]);
		work(argv[i]);
	}

	printf("Convert end\n");
}
