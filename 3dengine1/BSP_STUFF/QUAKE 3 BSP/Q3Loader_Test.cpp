#include "Q3Loader.h"
#include "Q3Loader.cpp"
#include <conio.h>

#include <cstdlib>

int main(int pArgc, char** pArgv)
{
	TMapQ3	lMap;
	
	readMap("data/final.bsp", lMap);

	FILE*	lFile = fopen("final_debug.txt", "w+");	
	debugInformations(lMap, lFile);
	fclose(lFile);

	freeMap(lMap);
	getch();
	
	return 0;
};
