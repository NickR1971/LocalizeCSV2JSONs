#include <stdio.h>
#include <stdlib.h>

#include "Header.h"

#pragma warning(disable:4996)

#define MAX_STR 8000
#define MAX_LANG 20
static char str[MAX_STR];

static int langCount;
FILE* fpOut[MAX_LANG];

void work(const char* _filename)
{
	FILE* fp;
	int i, j, n;
	char name[256];
	char c;

	fp = fopen(_filename, "r");

	if (fp == NULL)
	{
		printf("Can't open file\n");
		return;
	}

	fgets(str, MAX_STR, fp);

	if (str[0] == '#')
	{
		langCount = 0;
		for (i = 1; str[i] != 0; i++)
		{
			if (str[i] == '#') langCount++;
		}

		printf("Detected %d languages\n", langCount);

		if (langCount > MAX_LANG)
		{
			printf("Count language too much! Only %d created!\n", MAX_LANG);
			langCount = MAX_LANG;
		}
		n = 0;
		for (i = 1; str[i] != 0; i++)
		{
			if (str[i] != '#') continue;
			for (j = 0; (c = str[++i]) != ','; j++)
			{
				name[j] = c;
				if (((unsigned char)c) < ' ') break;
			}
			name[j] = 0;
			printf("File %d: %s", n, name);
			fpOut[n] = fopen(name, "w");
			if (fpOut[n] == NULL) printf(" failed");
			else
			{
				fputs("{ \"loc\": [\n", fpOut[n]);
			}
			printf("\n");
			n++;
			if (n == langCount) break;
		}

		while (fgets(str, MAX_STR, fp))
		{
			i = 0;
			while (str[i] != ',') i++;
			str[i++] = 0;
			for (j = 0; j < langCount; j++)
			{
				char* value;

				if (str[i] == '\"')
				{
					c = '\"';
					i++;
				}
				else c = ',';
				value = &str[i];
l_next:
				while (str[i] != c)
				{
					if (((unsigned char)str[i]) < ' ') break;
					i++;
				}
				if (c == '\"')
				{
					if (str[i + 1] == c)
					{
						str[i] = '\\';
						i += 2;
						goto l_next; // replace "" to \" and continue string converting
					}
				}
				str[i++] = 0;
				if (c == '\"' && str[i]==',') i++;
				fprintf(fpOut[j],"{ \"key\": \"%s\", \"value\": \"%s\" },\n", str, value);
			}
		}

		for (i = 0; i < n; i++)
		{
			if (fpOut[i] != NULL)
			{
				fputs("{ \"key\": \"core_empty\", \"value\": \"\" }\n] }", fpOut[i]);
				fclose(fpOut[i]);
			}
		}
	}
	else printf("File format not valid\n");
	fclose(fp);
}