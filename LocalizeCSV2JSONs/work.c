#include <stdio.h>
#include <stdlib.h>

#include "Header.h"

#pragma warning(disable:4996)

#define MAX_STR 8000
#define MAX_LANG 20
static char str[MAX_STR];

static int langCount;
static FILE* fp;
FILE* fpOut[MAX_LANG];

static int IsOpenCSV(const char* _filename)
{
	fp = fopen(_filename, "r");

	if (fp == NULL)
	{
		printf("Can't open file\n");
		return 0;
	}

	fgets(str, MAX_STR, fp);
	
	if (str[0] == '#') return 1;
	printf("File format not valid\n");
	fclose(fp);
	return 0;
}

static void SetLangCount()
{
	int i;

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
}

static void CreateOutputFiles()
{
	int i, j, n;
	char name[256];
	char c;

	n = 0;
	for (i = 1; str[i] != 0; i++)
	{
		if (str[i] != '#') continue;
		for (j = 0; (c = str[++i]) != ','; j++)
		{
			name[j] = c;
			if (((unsigned char)c) < ' ') break;
		}
		name[j++] = '.';
		name[j++] = 'j';
		name[j++] = 's';
		name[j++] = 'o';
		name[j++] = 'n';
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

}

static void ScanString()
{
	int i, curLang;
	char c;

	while (fgets(str, MAX_STR, fp))
	{
		if (str[0] == '#') continue;

		i = 0;
		while (str[i] != ',') i++;
		str[i++] = 0;
		for (curLang = 0; curLang < langCount; curLang++)
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
			if (c == '\"' && str[i] == ',') i++;
			fprintf(fpOut[curLang], "{ \"key\": \"%s\", \"value\": \"%s\" },\n", str, value);
		}
	}

}

static void CloseAll()
{
	int i;

	for (i = 0; i < langCount; i++)
	{
		if (fpOut[i] != NULL)
		{
			fputs("{ \"key\": \"core_empty\", \"value\": \"\" }\n] }", fpOut[i]);
			fclose(fpOut[i]);
		}
	}
	fclose(fp);
}

void work(const char* _filename)
{
	if (IsOpenCSV(_filename)==0)
	{		
		return;
	}
	
	SetLangCount();

	CreateOutputFiles();

	ScanString();

	CloseAll();
}