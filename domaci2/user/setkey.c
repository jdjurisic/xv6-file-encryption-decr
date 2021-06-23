#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{

	char key[5];
	int successful = 0;
	
	if(argc <= 1 || !(strcmp(argv[1],"--help")) || !(strcmp(argv[1],"-h")))
	{
		printf("Use this program to set the current active key.\nAfterwards you can use encr decr with current key.\n\nUsage: setkey [OPTION] ...[KEY]\nCommand line options:\n-h, --help: Show help prompt.\n-s, --secret: Enter the key via STDIN. Hide the key when entering it.\n");
	}
	else 
	if(!(strcmp(argv[1],"--secret")) || !(strcmp(argv[1],"-s")))
	{
		printf("Enter key:");
		setecho(0);
		gets(key,5);
		setecho(1);
		successful = setkey(atoi(key)); // -1 failed, 0 ok
	}
	else 
	{
		successful = setkey(atoi(argv[1]));
	}


	if(successful == -1)printf("Key error.\n");
	exit();

}
