//
// File descriptors
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"

struct devsw devsw[NDEV];
struct {
	struct spinlock lock;
	struct file file[NFILE];
} ftable;

void
fileinit(void)
{
	initlock(&ftable.lock, "ftable");
}

// Allocate a file structure.
struct file*
filealloc(void)
{
	struct file *f;

	acquire(&ftable.lock);
	for(f = ftable.file; f < ftable.file + NFILE; f++){
		if(f->ref == 0){
			f->ref = 1;
			release(&ftable.lock);
			return f;
		}
	}
	release(&ftable.lock);
	return 0;
}

// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
	acquire(&ftable.lock);
	if(f->ref < 1)
		panic("filedup");
	f->ref++;
	release(&ftable.lock);
	return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
	struct file ff;

	acquire(&ftable.lock);
	if(f->ref < 1)
		panic("fileclose");
	if(--f->ref > 0){
		release(&ftable.lock);
		return;
	}
	ff = *f;
	f->ref = 0;
	f->type = FD_NONE;
	release(&ftable.lock);

	if(ff.type == FD_PIPE)
		pipeclose(ff.pipe, ff.writable);
	else if(ff.type == FD_INODE){
		begin_op();
		iput(ff.ip);
		end_op();
	}
}

// Get metadata about file f.
int
filestat(struct file *f, struct stat *st)
{
	if(f->type == FD_INODE){
		ilock(f->ip);
		stati(f->ip, st);
		iunlock(f->ip);
		return 0;
	}
	return -1;
}

extern int caesarsKey;

// Read from file f.
int
fileread(struct file *f, char *addr, int n, int encFlag)
{
	int r;
	//cprintf("major je %d",f->ip->major);

	if(f->readable == 0)
		return -1;
	if(f->type == FD_PIPE)
		return piperead(f->pipe, addr, n);
	if(f->type == FD_INODE){
		ilock(f->ip);
		if((r = readi(f->ip, addr, f->off, n)) > 0)
			f->off += r;
		iunlock(f->ip);


		if(encFlag == 1){
			for(int i = 0; i < r; i++){
				addr[i] += caesarsKey;
			}
		}
		else if(encFlag == -1 || (f->ip->major == 1 && f->ip->type == 2)){ // T_FILE 2
			for(int i = 0; i < r; i++){
				addr[i] -= caesarsKey;
			}
		}

		// if(encFlag != 0)
		// {
		// begin_op();
		// ilock(f->ip);
		// if(encFlag == 1 && (f->off == f->ip->size)){
		// 	f->ip->major = 1;
		// 	cprintf("offset -> %d , size -> %d",f->off,f->ip->size);
		// }
		// if(encFlag == -1)f->ip->major = 0;
		// iupdate(f->ip); // https://www.cse.iitb.ac.in/~mythili/teaching/cs347_autumn2016/notes/10-xv6-filesystem.pdf
		// iunlock(f->ip);
		// end_op();
		// } --- ovo ne radi, duplira se ispis

		return r;
	}
	panic("fileread");
}

// Write to file f.
int
filewrite(struct file *f, char *addr, int n)
{
	int r;

	if(f->writable == 0)
		return -1;
	if(f->type == FD_PIPE)
		return pipewrite(f->pipe, addr, n);
	if(f->type == FD_INODE){
		// write a few blocks at a time to avoid exceeding
		// the maximum log transaction size, including
		// i-node, indirect block, allocation blocks,
		// and 2 blocks of slop for non-aligned writes.
		// this really belongs lower down, since writei()
		// might be writing a device like the console.
		int max = ((MAXOPBLOCKS-1-1-2) / 2) * 512;
		int i = 0;
		while(i < n){			
			int n1 = n - i;
			if(n1 > max)
				n1 = max;

			if(f->ip->major == 1 && f->ip->type == 2)
			for(int j = 0;j<n1;j++){
				addr[j] += caesarsKey;
			}

			begin_op();
			ilock(f->ip);
			if ((r = writei(f->ip, addr + i, f->off, n1)) > 0)
				f->off += r;
			iunlock(f->ip);
			end_op();

			if(r < 0)
				break;
			if(r != n1)
				panic("short filewrite");
			i += r;
		}
		return i == n ? n : -1;
	}
	panic("filewrite");
}

