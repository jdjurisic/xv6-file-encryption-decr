#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"


void helpMenu()
{
printf("Use this program to encrypt files written on the disk.\nUsage: encr [OPTION] ... [FILE]...\n\nCommand line options:\n-h, --help: Show menu prompt.\n-a, --encrypt-all: Encrypt all filed in CWD with current key.\n");
}



int
main(int argc, char *argv[])
{

	int successful = 0;
	int fd;

	if(argc <= 1){
		helpMenu();
		exit();
	}

	if(!(strcmp(argv[1],"--help")) || !(strcmp(argv[1],"-h")))
	{
	helpMenu();
	}
	else 
	if(!(strcmp(argv[1],"--encrypt-all")) || !(strcmp(argv[1],"-a")))
	{		
		char *argl[3];
		argl[0] = "ls";
 		argl[1] = "--encrypt-all";
 		argl[2] = 0;
		exec("/bin/ls",argl); // exec function explained -> https://www.ics.uci.edu/~aburtsev/cs5460/lectures/lecture02-xv6-unix/lecture02-xv6-unix.pdf
	}
	else
	{
		if((fd = open(argv[1],2)) < 0){
			printf("Failed to encrypt, file: %s is dir\n",argv[1]);
			exit();
		}
		else successful = encr(fd);
		if(successful == -1)printf("Failed to encrypt file: %s [key not set]\n", argv[1]);
		if(successful == -2)printf("Failed to encrypt file: %s [wrong type - dev_type]\n", argv[1]);
		if(successful == -3)printf("Failed to encrypt file: %s [file already encrypted]\n", argv[1]);
		if(successful ==  0)printf("Successfully encrypted file: %s\n",argv[1]);
	}
	

	exit();
}
