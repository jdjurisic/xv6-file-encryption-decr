#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fcntl.h"

void helpMenu(){
	printf("Use this program to create big file filled with a-z characters.\nDefault filename long.txt.\nDefault blocks 150.\nUsage: blockwriter [OPTION] ...\n\nCommand line options:\n-h, --help: Show help prompt.\n-b, --blocks: Number of blocks to write.\n-o, --output-file: Set output filename.\n");
}

int
main(int argc, char *argv[])
{

	int blockNo = 150;
	char *name = "long.txt";
	
	int i = 0;
	char *alphabet = "abcdefghijklmnopqrstuvwxyz";
	int filedescriptor;
	char data[512];

	if(!(argc <= 1))
	{

		while(i < argc)
		{

		//printf("i - %d - argv[i] - %s\n",i,argv[i]);

		if(!(strcmp(argv[i],"--help")) || !(strcmp(argv[i],"-h")))
		{
		helpMenu();
		}
		else 
		if(!(strcmp(argv[i],"--output-file")) || !(strcmp(argv[i],"-o")))
		{		
			name = argv[i+1];
			i++;
		}
		else
		if(!(strcmp(argv[i],"--blocks")) || !(strcmp(argv[i],"-b")))
		{
			blockNo = atoi(argv[i+1]);
			i++;
			if(blockNo > 11 + 128 + 128 * 128 || blockNo <= 0) // increase when double indirect is done
				{
				printf("Block limit exceeded or lower than allowed(1).\n");
				exit();
				}	
		}

		i++;

		}
	}
	
	//memset(data, 'a', sizeof(data));
	for(i = 0; i < sizeof(data); i++){
		data[i] = alphabet[i%26];
	}

	filedescriptor = open(name, O_CREATE | O_RDWR);
	for(int j = 0; j < blockNo; j++)
	{
		printf("Writing block %d\n",j);
		write(filedescriptor, data, sizeof(data));
	}	

	close(filedescriptor);
	exit();
}
