/*
 *  Copyright (C) 2020 CS416 Rutgers CS
 *	Tiny File System
 *	File:	tfs.c
 *
 */

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <libgen.h>
#include <limits.h>

#include "block.h"
#include "tfs.h"

char diskfile_path[PATH_MAX];

// Declare your in-memory data structures here
struct superblock* superblock = NULL;
bitmap_t dataBlockBitmap = NULL;
bitmap_t inodeBitmap = NULL;

/* 
 * Get available inode number from bitmap
 */
int get_avail_ino() {

	// Step 1: Read inode bitmap from disk
	
	// Step 2: Traverse inode bitmap to find an available slot

	// Step 3: Update inode bitmap and write to disk 

	return 0;
}

/* 
 * Get available data block number from bitmap
 */
int get_avail_blkno() {

	// Step 1: Read data block bitmap from disk
	
	// Step 2: Traverse data block bitmap to find an available slot

	// Step 3: Update data block bitmap and write to disk 

	return 0;
}

/* 
 * inode operations
 */
int readi(uint16_t ino, struct inode *inode) {

  	// Step 1: Get the inode's on-disk block number
	int inodeStartBlock = superblock -> i_start_blk;

	int blockOffset = ino / (BLOCK_SIZE / sizeof(struct inode));
	int blockNumber = inodeStartBlock + blockOffset;

 	// Step 2: Get offset of the inode in the inode on-disk block
	int inodeOffsetNumber = ino % (BLOCK_SIZE / sizeof(struct inode));
	int inodeOffset = inodeOffsetNumber * sizeof(struct inode);

  	// Step 3: Read the block from disk and then copy into inode structure
	void* buffer = malloc(BLOCK_SIZE);
	bio_read(blockNumber, buffer);

	memcpy(inode, buffer + inodeOffset, sizeof(struct inode));

	free(buffer);

	return 0;
}

int writei(uint16_t ino, struct inode *inode) {

	// Step 1: Get the block number where this inode resides on disk
	int inodeStartBlock = superblock -> i_start_blk;

	int blockOffset = ino / (BLOCK_SIZE / sizeof(struct inode));
	int blockNumber = inodeStartBlock + blockOffset;
	
	// Step 2: Get the offset in the block where this inode resides on disk
	int inodeOffsetNumber = ino % (BLOCK_SIZE / sizeof(struct inode));
	int inodeOffset = inodeOffsetNumber * sizeof(struct inode);

	// Step 3: Write inode to disk
	void* buffer = malloc(BLOCK_SIZE);
	bio_read(blockNumber, buffer);

	memcpy(buffer + inodeOffset, inode, sizeof(struct inode));
	bio_write(blockNumber, buffer);

	free(buffer);

	return 0;
}


/* 
 * directory operations
 */
int dir_find(uint16_t ino, const char *fname, size_t name_len, struct dirent *dirent) {

  	// Step 1: Call readi() to get the inode using ino (inode number of current directory)
	struct inode* inode = malloc(sizeof(struct inode));
	readi(ino, inode);

	int i;
	for (i = 0; i < 16; i++) {
		// Step 2: Get data block of current directory from inode
		int dataBlockNumber = inode -> direct_ptr[i];
		//If the data block has not yet been initialized
		if (dataBlockNumber == 0)
			continue;

		void* dataBlock = malloc(BLOCK_SIZE);
		bio_read(dataBlockNumber, dataBlock);

		// Step 3: Read directory's data block and check each directory entry.
		//If the name matches, then copy directory entry to dirent structure
		int maxDirent = BLOCK_SIZE / sizeof(struct dirent);
		struct dirent* currentDirent = malloc(sizeof(struct dirent));

		int j;
		for (j = 0; j < maxDirent; j++) {
			memcpy(currentDirent, dataBlock + j * sizeof(struct dirent), sizeof(struct dirent));

			if (currentDirent -> valid == 1 && currentDirent -> len == name_len) {
				//Check if name matches
				char* direntName = malloc(currentDirent -> len);
				memcpy(direntName, currentDirent -> name, currentDirent -> len);

				if (strcmp(fname, direntName) == 0) {
					dirent = currentDirent;

					free(direntName);
					free(currentDirent);
					free(dataBlock);
					free(inode);

					return 1;
				}

				free(direntName);
			}
		}

		free(currentDirent);
		free(dataBlock);
	}

	free(inode);

	return 0;
}

int dir_add(struct inode dir_inode, uint16_t f_ino, const char *fname, size_t name_len) {

	// Step 1: Read dir_inode's data block and check each directory entry of dir_inode
	
	// Step 2: Check if fname (directory name) is already used in other entries

	// Step 3: Add directory entry in dir_inode's data block and write to disk

	// Allocate a new data block for this directory if it does not exist

	// Update directory inode

	// Write directory entry

	int i;
	for (i = 0; i < 16; i++) {
		int dataBlockNumber = dir_inode.direct_ptr[i];

		if (dataBlockNumber == 0) {
			int maxDirent = BLOCK_SIZE / sizeof(struct dirent);
			struct dirent* newDirents = malloc(sizeof(struct dirent) * maxDirent);

			newDirents -> ino = f_ino;
			memcpy(newDirents -> name, fname, name_len);
			newDirents -> len = name_len;
			newDirents -> valid = 1;

			int i;
			for (i = 1; i < maxDirent; i++)
				(newDirents + i * sizeof(struct dirent)) -> valid = 0;

			int newDataBlockNum = get_avail_blkno();
			bio_write(newDataBlockNum, newDirents);

			dir_inode.direct_ptr[i] = newDataBlockNum;

			writei(dir_inode.ino, &dir_inode);

			free(newDirents);

			return 1;
		}

		void* dataBlock = malloc(BLOCK_SIZE);
		bio_read(dataBlockNumber, dataBlock);

		int maxDirent = BLOCK_SIZE / sizeof(struct dirent);
		struct dirent* currentDirent = malloc(sizeof(struct dirent));

		int i;
		for (i = 0; i < maxDirent; i++) {
			memcpy(currentDirent, dataBlock + i * sizeof(struct dirent), sizeof(struct dirent));

			if (currentDirent -> valid == 0) {
				currentDirent -> ino = f_ino;
				memcpy(currentDirent -> name, fname, name_len);
				currentDirent -> len = name_len;
				currentDirent -> valid = 1;

				memcpy(dataBlock + i * sizeof(struct dirent), currentDirent, sizeof(struct dirent));
				bio_write(dataBlockNumber, dataBlock);

				free(currentDirent);
				free(dataBlock);

				return 1;
			}
		}

		free(currentDirent);
		free(dataBlock);
	}

	return 0;
}

int dir_remove(struct inode dir_inode, const char *fname, size_t name_len) {

	// Step 1: Read dir_inode's data block and checks each directory entry of dir_inode
	
	// Step 2: Check if fname exist

	// Step 3: If exist, then remove it from dir_inode's data block and write to disk

	int i;
	for (i = 0; i < 16; i++) {
		int dataBlockNumber = dir_inode.direct_ptr[i];
		if (dataBlockNumber == 0)
			continue;

		void* dataBlock = malloc(BLOCK_SIZE);
		bio_read(dataBlockNumber, dataBlock);

		int maxDirent = BLOCK_SIZE / sizeof(struct dirent);
		struct dirent* currentDirent = malloc(sizeof(struct dirent));

		int j;
		for (j = 0; j < maxDirent; j++) {
			memcpy(currentDirent, dataBlock + j * sizeof(struct dirent), sizeof(struct dirent));

			if (currentDirent -> valid == 1 && currentDirent -> len == name_len) {
				char* direntName = malloc(currentDirent -> len);
				memcpy(direntName, currentDirent -> name, currentDirent -> len);

				if (strcmp(fname, direntName) == 0) {
					currentDirent -> valid = 0;

					memcpy(dataBlock + j * sizeof(struct dirent), currentDirent, sizeof(struct dirent));
					bio_write(dataBlockNumber, dataBlock);

					free(direntName);
					free(currentDirent);
					free(dataBlock);

					return 1;
				}

				free(direntName);
			}
		}

		free(currentDirent);
		free(dataBlock);
	}

	return 0;
}

/* 
 * namei operation
 */
int get_node_by_path(const char *path, uint16_t ino, struct inode *inode) {
	
	// Step 1: Resolve the path name, walk through path, and finally, find its inode.
	// Note: You could either implement it in a iterative way or recursive way
	char* pathName = strtok(path + 1, "/");
	int inodeNumber = ino;
	struct dirent* dirent = malloc(sizeof(struct dirent));

	do {
		dir_find(inodeNumber, pathName, strlen(pathName), dirent);

		inodeNumber = dirent -> ino;
		pathName = strtok(NULL, "/");
	} while (pathName != NULL);

	readi(inodeNumber, inode);

	free(dirent);

	return 0;
}

/* 
 * Make file system
 */
int tfs_mkfs() {
	int counter = 1;

	// Call dev_init() to initialize (Create) Diskfile
	dev_init(diskfile_path);

	// write superblock information
	superblock = malloc(sizeof(struct superblock));
	superblock -> magic_num = MAGIC_NUM;
	superblock -> max_inum = MAX_INUM;
	superblock -> max_dnum = MAX_DNUM;

	// initialize inode bitmap
	int numBytes = MAX_INUM / 8;
	inodeBitmap = malloc(numBytes);

	int i;
	for (i = 0; i < numBytes; i++)
		*(inodeBitmap + i) = 0;

	superblock -> i_bitmap_blk = counter;
	counter += (numBytes / BLOCK_SIZE) + 1;

	// initialize data block bitmap
	numBytes = MAX_DNUM / 8;
	dataBlockBitmap = malloc(numBytes);

	for (i = 0; i < numBytes; i++)
		*(dataBlockBitmap + i) = 0;

	superblock -> d_bitmap_blk = counter;
	counter += (numBytes / BLOCK_SIZE) + 1;

	superblock -> i_start_blk = counter;

	int inodePerBlock = BLOCK_SIZE / sizeof(struct inode);
	int numInodeBlocks = (MAX_INUM / inodePerBlock) + 1;
	if (MAX_INUM % inodePerBlock)
		numInodeBlocks--;

	counter += numInodeBlocks + 1;

	superblock -> d_start_blk = counter;

	bio_write(0, superblock);

	// update bitmap information for root directory
	set_bitmap(inodeBitmap, 0);

	for (i = 0; i < superblock -> d_bitmap_blk; i++)
		set_bitmap(dataBlockBitmap, i);

	bio_write(superblock -> i_bitmap_blk, inodeBitmap);
	bio_write(superblock -> d_bitmap_blk, dataBlockBitmap);

	// update inode for root directory

	//Have to create all inodes?

	return 0;
}


/* 
 * FUSE file operations
 */
static void *tfs_init(struct fuse_conn_info *conn) {

	// Step 1a: If disk file is not found, call mkfs

  	// Step 1b: If disk file is found, just initialize in-memory data structures
  	// and read superblock from disk

	return NULL;
}

static void tfs_destroy(void *userdata) {

	// Step 1: De-allocate in-memory data structures

	// Step 2: Close diskfile

}

static int tfs_getattr(const char *path, struct stat *stbuf) {

	// Step 1: call get_node_by_path() to get inode from path

	// Step 2: fill attribute of file into stbuf from inode

		stbuf->st_mode   = S_IFDIR | 0755;
		stbuf->st_nlink  = 2;
		time(&stbuf->st_mtime);

	return 0;
}

static int tfs_opendir(const char *path, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path

	// Step 2: If not find, return -1

    return 0;
}

static int tfs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path

	// Step 2: Read directory entries from its data blocks, and copy them to filler

	return 0;
}


static int tfs_mkdir(const char *path, mode_t mode) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name

	// Step 2: Call get_node_by_path() to get inode of parent directory

	// Step 3: Call get_avail_ino() to get an available inode number

	// Step 4: Call dir_add() to add directory entry of target directory to parent directory

	// Step 5: Update inode for target directory

	// Step 6: Call writei() to write inode to disk
	

	return 0;
}

static int tfs_rmdir(const char *path) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name

	// Step 2: Call get_node_by_path() to get inode of target directory

	// Step 3: Clear data block bitmap of target directory

	// Step 4: Clear inode bitmap and its data block

	// Step 5: Call get_node_by_path() to get inode of parent directory

	// Step 6: Call dir_remove() to remove directory entry of target directory in its parent directory

	return 0;
}

static int tfs_releasedir(const char *path, struct fuse_file_info *fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target file name

	// Step 2: Call get_node_by_path() to get inode of parent directory

	// Step 3: Call get_avail_ino() to get an available inode number

	// Step 4: Call dir_add() to add directory entry of target file to parent directory

	// Step 5: Update inode for target file

	// Step 6: Call writei() to write inode to disk

	return 0;
}

static int tfs_open(const char *path, struct fuse_file_info *fi) {

	// Step 1: Call get_node_by_path() to get inode from path

	// Step 2: If not find, return -1

	return 0;
}

static int tfs_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {

	// Step 1: You could call get_node_by_path() to get inode from path

	// Step 2: Based on size and offset, read its data blocks from disk

	// Step 3: copy the correct amount of data from offset to buffer

	// Note: this function should return the amount of bytes you copied to buffer
	return 0;
}

static int tfs_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	// Step 1: You could call get_node_by_path() to get inode from path

	// Step 2: Based on size and offset, read its data blocks from disk

	// Step 3: Write the correct amount of data from offset to disk

	// Step 4: Update the inode info and write it to disk

	// Note: this function should return the amount of bytes you write to disk
	return size;
}

static int tfs_unlink(const char *path) {

	// Step 1: Use dirname() and basename() to separate parent directory path and target file name

	// Step 2: Call get_node_by_path() to get inode of target file

	// Step 3: Clear data block bitmap of target file

	// Step 4: Clear inode bitmap and its data block

	// Step 5: Call get_node_by_path() to get inode of parent directory

	// Step 6: Call dir_remove() to remove directory entry of target file in its parent directory

	return 0;
}

static int tfs_truncate(const char *path, off_t size) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_release(const char *path, struct fuse_file_info *fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
	return 0;
}

static int tfs_flush(const char * path, struct fuse_file_info * fi) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_utimens(const char *path, const struct timespec tv[2]) {
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}


static struct fuse_operations tfs_ope = {
	.init		= tfs_init,
	.destroy	= tfs_destroy,

	.getattr	= tfs_getattr,
	.readdir	= tfs_readdir,
	.opendir	= tfs_opendir,
	.releasedir	= tfs_releasedir,
	.mkdir		= tfs_mkdir,
	.rmdir		= tfs_rmdir,

	.create		= tfs_create,
	.open		= tfs_open,
	.read 		= tfs_read,
	.write		= tfs_write,
	.unlink		= tfs_unlink,

	.truncate   = tfs_truncate,
	.flush      = tfs_flush,
	.utimens    = tfs_utimens,
	.release	= tfs_release
};


int main(int argc, char *argv[]) {
	int fuse_stat;

	getcwd(diskfile_path, PATH_MAX);
	strcat(diskfile_path, "/DISKFILE");

	fuse_stat = fuse_main(argc, argv, &tfs_ope, NULL);

	return fuse_stat;
}

