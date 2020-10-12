#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>

static const char g_sProgramName[] = "bzip2_exec",
                  g_sProgramDescription[] = "converts each file to bz2 in directories",
                  g_sProgramVersion[] = "1.0",
                  g_sProgramBuildDate[] = __DATE__;

int PrintHelpInformation(const char *sExecFile)
{
	return printf("%s, a %s. Version %s, %s\n\n\tusage: %s [bzip2 args, by default -9] [directory to work, by default current directory]\n\n", g_sProgramName, g_sProgramDescription, g_sProgramVersion, g_sProgramBuildDate, sExecFile);
}

int ReadDir(const char *sDirPath, const char *sIgnore, const char *sCompression)
{
	DIR *pDir = opendir(sDirPath);

	if(pDir)
	{
		struct dirent *pEnt;

		while((pEnt = readdir(pDir)))
		{
			char iElementType = pEnt->d_type;

			const char *sElementName = pEnt->d_name;

			char sNewDir[1024];

			int iLength = strlen(sDirPath);

			if(iLength)
			{
				sprintf(sNewDir, sDirPath[iLength - 1] == '/' ? "%s%s" : "%s/%s", sDirPath, sElementName);
			}
			else
			{
				sprintf(sNewDir, "./%s", sElementName);
			}

			if(iElementType & (1 << 2))		// Is dir.
			{
				if(strcmp(sElementName, ".") && strcmp(sElementName, ".."))
				{
					ReadDir(sNewDir, "", sCompression);
				}
			}
			else if(iElementType & (1 << 3) && strcmp(sElementName, sIgnore))		// Is file.
			{
				/* I was too lazy to do it at a low level BZip2 API :P */

				char sSysCommand[256];

				sprintf(sSysCommand, "/bin/bzip2 %s \"%s\"\n", sCompression, sNewDir);

				if(!system(sSysCommand))
				{
					printf("%s -> %s.bz2\n", sElementName, sElementName);
				}
			}
		}

		closedir(pDir);
	}
	else
	{
		/* Could not open directory. */

		char sErrorMessage[256];

		sprintf(sErrorMessage, "Failed the open \"%s\"\n", sDirPath);
		perror(sErrorMessage);

		return 1;
	}

	return 0;
}

int main(int iArgs, const char **psArgs)
{
	bool bIsArgsLessThanTwo = iArgs > 1;

	int  iResult;		// aka EAX

	if(bIsArgsLessThanTwo && (!strcmp(psArgs[1], "-h") || !strcmp(psArgs[1], "-V") || !strcmp(psArgs[1], "--help") || !strcmp(psArgs[1], "--version")))
	{
		iResult = PrintHelpInformation(psArgs[0]);
	}
	else
	{
		const char *sIgnore = strrchr(psArgs[0], '/');

		sIgnore = sIgnore ? &sIgnore[1] : psArgs[0];

		printf("Ignore \"%s\"\n", sIgnore);

		iResult = ReadDir(iArgs > 2 && psArgs[2] ? psArgs[2] : "./", sIgnore, bIsArgsLessThanTwo && psArgs[1] ? psArgs[1] : "-9");
	}

	return iResult;
}