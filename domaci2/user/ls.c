#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fs.h"


char*
fmtname(char *path)
{
	static char buf[DIRSIZ+1];
	char *p;

	// Find first character after last slash.
	for(p=path+strlen(path); p >= path && *p != '/'; p--)
		;
	p++;

	// Return blank-padded name.
	if(strlen(p) >= DIRSIZ)
		return p;
	memmove(buf, p, strlen(p));
	memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
	return buf;
}

void
ls(char *path)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		fprintf(2, "ls: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "ls: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){
	case T_FILE:
		printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
		break;

	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
			printf("ls: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf("ls: cannot stat %s\n", buf);
				continue;
			}
			printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
		}
		break;
	}
	close(fd);
}


//new

void
lookAndEncr(char *path)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		fprintf(2, "ls: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "ls: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){
	case T_FILE:
		printf("fajl %s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
		break;
	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
			printf("ls: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf("ls: cannot stat %s\n", buf);
				continue;
			}
			int childfd;
			if((childfd = open(buf,2)) < 0){
				printf("Failed to encrypt, file: %s is dir\n",fmtname(buf));
				continue;
			}
			int successful = encr(childfd);
			if(successful == -1)printf("Failed to encrypt file: %s [key not set]\n", fmtname(buf));
			if(successful == -2)printf("Failed to encrypt file: %s [wrong type - dev_type]\n", fmtname(buf));
			if(successful == -3)printf("Failed to encrypt file: %s [file already encrypted]\n", fmtname(buf));
			if(successful ==  0)printf("Successfully encrypted file: %s\n",fmtname(buf));
			close(childfd);
		}
		break;
	}
	close(fd);
}

void
lookAndDecr(char *path)
{
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if((fd = open(path, 0)) < 0){
		fprintf(2, "ls: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "ls: cannot stat %s\n", path);
		close(fd);
		return;
	}

	switch(st.type){
	case T_FILE:
		printf("fajl %s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
		break;
	case T_DIR:
		if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
			printf("ls: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf+strlen(buf);
		*p++ = '/';
		while(read(fd, &de, sizeof(de)) == sizeof(de)){
			if(de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if(stat(buf, &st) < 0){
				printf("ls: cannot stat %s\n", buf);
				continue;
			}
			int childfd;
			if((childfd = open(buf,2)) < 0){
				printf("Failed to encrypt, file: %s is dir\n",fmtname(buf));
				continue;
			}
			int successful = decr(childfd);
			if(successful == -1)printf("Failed to decrypt file: %s [key not set]\n", fmtname(buf));
			if(successful == -2)printf("Failed to decrypt file: %s [wrong type - dev_type]\n", fmtname(buf));
			if(successful == -3)printf("Failed to encrypt file: %s [file not encrypted]\n", fmtname(buf));
			if(successful ==  0)printf("Successfully decrypted file: %s\n",fmtname(buf));
			close(childfd);
		}
		break;
	}
	close(fd);
}

//new



int
main(int argc, char *argv[])
{
	int i;

	//printf("argc je %d\n",argc);
	if(argc < 2){
		ls(".");
		exit();
	}
	for(i=1; i<argc; i++){
		//printf("argv %d - %s\n",i,argv[i]);
		if(!strcmp(argv[1],"--encrypt-all")){
			lookAndEncr(".");
		}
		else
		if(!strcmp(argv[1],"--decrypt-all")){
			lookAndDecr(".");
		}
		else 
		ls(argv[i]);
	}

	exit();
}
