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
	char* buffer = (char*) malloc(sizeof(char) * BLOCK_SIZE);
	char* ibuffer = (char*) malloc(sizeof(char) * (MAX_INUM / 8));
	bio_read(1, (void*) buffer);
	int i = 0;
	for(i = 0; i < (MAX_INUM / 8); i++)
	{
		ibuffer[i] = buffer[i];
	}
	bitmap_t bit = (bitmap_t) ibuffer;
	// Step 2: Traverse inode bitmap to find an available slot
	for(i = 0; i < MAX_INUM; i++)
	{
		if(get_bitmap(bit, i) == 0)
		{
			set_bitmap(bit, i);
			inodeBitmap = bit;
			bio_write(1, (void*) bit);
			free(buffer);
			free(ibuffer);
			return i;
		}
	}
	free(buffer);
	free(ibuffer);
	return -1;
}

/* 
 * Get available data block number from bitmap
 */
int get_avail_blkno() {

	// Step 1: Read data block bitmap from disk
	char* buffer = (char*) malloc(sizeof(char) * BLOCK_SIZE);
	char* dbuffer = (char*) malloc(sizeof(char) * (MAX_DNUM / 8));
	bio_read(2, (void*) buffer);
	int i = 0;
	for(i = 0; i < (MAX_DNUM / 8); i++)
	{
		dbuffer[i] = buffer[i];
	}
	bitmap_t bit = (bitmap_t) dbuffer;
	// Step 2: Traverse data block bitmap to find an available slot
	for(i = 0; i < MAX_DNUM; i++)
	{
		if(get_bitmap(bit, i) == 0)
		{
			set_bitmap(bit, i);
			dataBlockBitmap = bit;
			bio_write(2, (void*) bit);
			free(buffer);
			free(dbuffer);
			return i;
		}
	}
	free(buffer);
	free(dbuffer);
	return -1;
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
	printf("Entering dir_find, trying to find %s\n", fname);
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
		int direntPerBlock = BLOCK_SIZE / sizeof(struct dirent);
		struct dirent* currentDirent = malloc(sizeof(struct dirent));

		int j;
		for (j = 0; j < direntPerBlock; j++) {
			memcpy(currentDirent, dataBlock + j * sizeof(struct dirent), sizeof(struct dirent));
			
			if (currentDirent -> valid == 1 && currentDirent -> len == name_len) {
				//Check if name matches
				char* direntName = malloc(currentDirent -> len);
				memcpy(direntName, currentDirent -> name, currentDirent -> len);

				if (strncmp(fname, direntName, name_len) == 0) {
					dirent = currentDirent;

					free(direntName);
					free(currentDirent);
					free(dataBlock);
					free(inode);

					printf("Found!\n");

					return 1;
				}

				free(direntName);
			}
		}

		free(currentDirent);
		free(dataBlock);
	}

	free(inode);

	printf("Leaving dir_find\n");

	return 0;
}

int dir_add(struct inode dir_inode, uint16_t f_ino, const char *fname, size_t name_len) {
	printf("Entering dir_add\n");
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
			int direntPerBlock = BLOCK_SIZE / sizeof(struct dirent);
			struct dirent* newDirents = malloc(sizeof(struct dirent) * direntPerBlock);
			memset(newDirents, 0, sizeof(struct dirent) * direntPerBlock);

			newDirents -> ino = f_ino;
			memcpy(newDirents -> name, fname, name_len);
			newDirents -> len = name_len;
			newDirents -> valid = 1;

			int newDataBlockNum = get_avail_blkno();
			bio_write(newDataBlockNum, newDirents);

			struct inode* dirInode = malloc(sizeof(struct inode));
			*dirInode = dir_inode;

			(dirInode -> direct_ptr)[i] = newDataBlockNum;
			writei(dirInode -> ino, dirInode);

			free(dirInode);
			free(newDirents);

			printf("Leaving dir_add\n");

			return 1;
		}

		void* dataBlock = malloc(BLOCK_SIZE);
		bio_read(dataBlockNumber, dataBlock);

		int direntPerBlock = BLOCK_SIZE / sizeof(struct dirent);
		struct dirent* currentDirent = malloc(sizeof(struct dirent));

		int i;
		for (i = 0; i < direntPerBlock; i++) {
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

		int direntPerBlock = BLOCK_SIZE / sizeof(struct dirent);
		struct dirent* currentDirent = malloc(sizeof(struct dirent));

		int j;
		for (j = 0; j < direntPerBlock; j++) {
			memcpy(currentDirent, dataBlock + j * sizeof(struct dirent), sizeof(struct dirent));

			if (currentDirent -> valid == 1 && currentDirent -> len == name_len) {
				char* direntName = malloc(currentDirent -> len);
				memcpy(direntName, currentDirent -> name, currentDirent -> len);

				if (strncmp(fname, direntName, name_len) == 0) {
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
	printf("Entering get_node_by_path\n");
	// Step 1: Resolve the path name, walk through path, and finally, find its inode.
	// Note: You could either implement it in a iterative way or recursive way
	if (strlen(path) == 1) {
		readi(0, inode);

		return 0;
	}

	char* pathCopy = malloc(strlen(path));
	strcpy(pathCopy, path);

	char* pathName = strtok(pathCopy + 1, "/");

	int inodeNumber = ino;
	struct dirent* dirent = malloc(sizeof(struct dirent));

	do {
		int found = dir_find(inodeNumber, pathName, strlen(pathName), dirent);
		if (found == 0)
			//There was no directory/file with that name
			return -1;

		inodeNumber = dirent -> ino;
		pathName = strtok(NULL, "/");
	} while (pathName != NULL);

	readi(inodeNumber, inode);

	free(pathCopy);
	free(dirent);

	printf("Leaving get_node_by_path\n");

	return 0;
}

/* 
 * Make file system
 */
int tfs_mkfs() {
	printf("Entering tfs_mkfs\n");
	// Call dev_init() to initialize (Create) Diskfile
	dev_init(diskfile_path);
	
	// write superblock information
	superblock = malloc(sizeof(struct superblock));
	superblock -> magic_num = MAGIC_NUM;
	superblock -> max_inum = MAX_INUM;
	superblock -> max_dnum = MAX_DNUM;
	superblock -> i_bitmap_blk = 1;
	superblock -> d_bitmap_blk = 2;
	superblock -> i_start_blk = 3;

	// initialize inode bitmap
	int numBytes = MAX_INUM / 8;
	inodeBitmap = malloc(numBytes);

	int i;
	for (i = 0; i < numBytes; i++)
		*(inodeBitmap + i) = 0;

	// initialize data block bitmap
	numBytes = MAX_DNUM / 8;
	dataBlockBitmap = malloc(numBytes);

	for (i = 0; i < numBytes; i++)
		*(dataBlockBitmap + i) = 0;

	int inodePerBlock = BLOCK_SIZE / sizeof(struct inode);
	int numInodeBlocks = (MAX_INUM / inodePerBlock) + 1;
	if (MAX_INUM % inodePerBlock)
		numInodeBlocks--;

	superblock -> d_start_blk = (superblock -> i_start_blk) + numInodeBlocks + 1;

	bio_write(0, superblock);

	// update bitmap information for root directory
	set_bitmap(inodeBitmap, 0);

	for (i = 0; i < superblock -> d_start_blk; i++)
		set_bitmap(dataBlockBitmap, i);

	bio_write(superblock -> i_bitmap_blk, inodeBitmap);
	bio_write(superblock -> d_bitmap_blk, dataBlockBitmap);

	//Have to initialize all inodes?
	for (i = 0; i < numInodeBlocks; i++) {
		void* inodeDataBlock = malloc(BLOCK_SIZE);
		memset(inodeDataBlock, 0, BLOCK_SIZE);
		bio_write((superblock -> i_start_blk) + i, inodeDataBlock);

		free(inodeDataBlock);
	}

	// update inode for root directory
	struct inode* root = malloc(sizeof(struct inode));

	root -> ino = 0;
	root -> valid = 1;
	root ->	size = 0;
	//For Type,
	//0 == Directory
	//1 == File
	root ->	type = 0;
	root -> link = 2;
	root -> direct_ptr[16] = 0;
	root -> indirect_ptr[8] = 0;

	struct stat* stat = malloc(sizeof(struct stat));
	stat -> st_mode = S_IFDIR | 0755;
	stat -> st_nlink = 2;
	stat -> st_uid = getuid();
	stat -> st_gid = getgid();
	stat -> st_atime = time(NULL);
	stat -> st_mtime = time(NULL);

	root -> vstat = *stat;

	writei(0, root);

	free(root);
	free(stat);

	printf("Leaving tfs_mkfs\n");

	return 0;
}


/* 
 * FUSE file operations
 */
static void *tfs_init(struct fuse_conn_info *conn) {
	printf("Entering tfs_init\n");
	int open = dev_open(diskfile_path);
	if (open != 0) {
		tfs_mkfs();

		printf("Leaving tfs_init\n");

		return NULL;
	}

	printf("Open successful\n");
	// Step 1b: If disk file is found, just initialize in-memory data structures
	// and read superblock from disk
	void* buffer = malloc(BLOCK_SIZE);
	bio_read(0, buffer);

	struct superblock* sblock = malloc(sizeof(struct superblock));
	memcpy(sblock, buffer, sizeof(struct superblock));

	if (sblock -> magic_num != MAGIC_NUM) {
		printf("Have to make file system anyways\n");
		free(buffer);
		free(sblock);

		tfs_mkfs();

		return NULL;
	}

	superblock = malloc(sizeof(struct superblock));
	superblock = sblock;
	free(sblock);

	bio_read(1, buffer);
	inodeBitmap = malloc(MAX_INUM / 8);
	memcpy(inodeBitmap, buffer, MAX_INUM / 8);

	bio_read(2, buffer);
	dataBlockBitmap = malloc(MAX_DNUM / 8);
	memcpy(dataBlockBitmap, buffer, MAX_DNUM / 8);

	free(buffer);

	return NULL;
}

static void tfs_destroy(void *userdata) {
	printf("Entering tfs_destroy\n");
	// Step 1: De-allocate in-memory data structures
	free(superblock);
	free(inodeBitmap);
	free(dataBlockBitmap);

	// Step 2: Close diskfile
	dev_close();
}

static int tfs_getattr(const char *path, struct stat *stbuf) {
	printf("Entering tfs_getattr\n");
	// Step 1: call get_node_by_path() to get inode from path
	struct inode* inode = malloc(sizeof(struct inode));
	int found = get_node_by_path(path, 0, inode);
	if (found == -1) {
		free(inode);

		return -ENOENT;
	}

	/*
	printf("Inode number: %d\n", inode -> ino);
	printf("Valid: %d\n", inode -> valid);
	printf("Type: %d\n", inode -> type);
	printf("Link: %d\n", inode -> link);
	struct inode* test = malloc(sizeof(struct inode));
	readi(0, test);
	printf("Inode number: %d\n", test -> ino);
	printf("Valid: %d\n", test -> valid);
	printf("Type: %d\n", test -> type);
	printf("Link: %d\n", test -> link);
	printf("Dirent number: %d\n", test -> direct_ptr[0]);
	struct dirent* test2 = malloc(sizeof(struct dirent));
	void* buffer = malloc(BLOCK_SIZE);
	bio_read(test -> direct_ptr[0], buffer);
	memcpy(test2, buffer, sizeof(struct dirent));
	printf("Inode number: %d\n", test2 -> ino);
	printf("Valid: %d\n", test2 -> valid);
	printf("Length: %d\n", test2 -> len);
	printf("Name: %s\n", test2 -> name);
	*/

	// Step 2: fill attribute of file into stbuf from inode
	stbuf -> st_ino = inode -> vstat.st_ino;
	stbuf -> st_mode = inode -> vstat.st_mode;
	stbuf -> st_nlink = inode -> vstat.st_nlink;
	stbuf -> st_uid = inode -> vstat.st_uid;
	stbuf -> st_gid = inode -> vstat.st_gid;
	stbuf -> st_size = inode -> vstat.st_size;
	stbuf -> st_atime = inode -> vstat.st_atime;
	stbuf -> st_mtime = inode -> vstat.st_mtime;

	//free(inode);	

	printf("Leaving tfs_getattr\n");

	return 0;
}

static int tfs_opendir(const char *path, struct fuse_file_info *fi) {
	printf("Entering tfs_opendir\n");
	// Step 1: Call get_node_by_path() to get inode from path
	struct inode* inode = malloc(sizeof(struct inode));
	int found = get_node_by_path(path, 0, inode);

	// Step 2: If not find, return -1
	free(inode);
	if (found == -1)
		return -ENOENT;
	else
    	return 0;
}

static int tfs_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	printf("Entering tfs_readdir\n");
	// Step 1: Call get_node_by_path() to get inode from path
	struct inode* inode = malloc(sizeof(struct inode));
	int found = get_node_by_path(path, 0, inode);
	if (found == -1) {
		free(inode);

		return -ENOENT;
	}

	// Step 2: Read directory entries from its data blocks, and copy them to filler
	int i;
	for (i = 0; i < 16; i++) {
		int dataBlockNumber = inode -> direct_ptr[i];
		if (dataBlockNumber == 0)
			continue;

		void* dataBlock = malloc(BLOCK_SIZE);
		bio_read(dataBlockNumber, dataBlock);

		int direntPerBlock = BLOCK_SIZE / sizeof(struct dirent);
		struct dirent* currentDirent = malloc(sizeof(struct dirent));

		int j;
		for (j = 0; j < direntPerBlock; j++) {
			memcpy(currentDirent, dataBlock + j * sizeof(struct dirent), sizeof(struct dirent));

			if (currentDirent -> valid == 1) {
				char* direntName = malloc(currentDirent -> len);
				memcpy(direntName, currentDirent -> name, currentDirent -> len);

				filler(buffer, direntName, NULL, 0);

				free(direntName);
			}
		}

		free(currentDirent);
		free(dataBlock);
	}

	free(inode);

	return 0;
}


static int tfs_mkdir(const char *path, mode_t mode) {
	printf("Entering tfs_mkdir\n");
	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name
	int final = 0;

	int i;
	for (i = 2; i < strlen(path); i++) {
		if (*(path + i) == '/')
			final = i;
	}

	char* dirName;
	if (final == 0) {
		dirName = malloc(1);
		strncpy(dirName, path, 1);
		*(dirName + 1) = '\0';
	} else {
		dirName = malloc(final);
		strncpy(dirName, path, final);
		*(dirName + final) = '\0';
	}

	char* baseName = malloc(strlen(path) - final - 1);
	strncpy(baseName, path + final + 1, strlen(path) - final - 1);
	*(baseName + strlen(path) - final - 1) = '\0';

	// Step 2: Call get_node_by_path() to get inode of parent directory
	struct inode* dirInode = malloc(sizeof(struct inode));

	if (final == 0)
		readi(0, dirInode);
	else
		get_node_by_path(dirName, 0, dirInode);

	struct dirent *newDirent = NULL;
	int found = dir_find(dirInode -> ino, baseName, strlen(path) - final - 1, newDirent);
	if (found == 1) {
		free(dirInode);

		return -ENOENT;
	}

	// Step 3: Call get_avail_ino() to get an available inode number
	int newInodeNumber = get_avail_ino();

	// Step 4: Call dir_add() to add directory entry of target directory to parent directory
	dir_add(*dirInode, newInodeNumber, baseName, strlen(path) - final - 1);

	// Step 5: Update inode for target directory
	struct inode* newInode = malloc(sizeof(struct inode));

	newInode -> ino = newInodeNumber;
	newInode -> valid = 1;
	newInode ->	size = 0;
	newInode ->	type = 0;
	newInode -> link = 2;
	newInode -> direct_ptr[16] = 0;
	newInode -> indirect_ptr[8] = 0;

	struct stat* stat = malloc(sizeof(struct stat));
	stat -> st_mode = S_IFDIR | mode;
	stat -> st_nlink = 2;
	stat -> st_uid = getuid();
	stat -> st_gid = getgid();
	stat -> st_size = 0;
	stat -> st_atime = time(NULL);
	stat -> st_mtime = time(NULL);

	newInode -> vstat = *stat;

	// Step 6: Call writei() to write inode to disk
	writei(newInodeNumber, newInode);

	free(dirInode);
	free(newInode);
	free(stat);
	free(dirName);
	free(baseName);

	return 0;
}

static int tfs_rmdir(const char *path) {
	printf("Entering tfs_rmdir\n");
	// Step 1: Use dirname() and basename() to separate parent directory path and target directory name
	int final = 0;

	int i;
	for (i = 2; i < strlen(path); i++) {
		if (*(path + i) == '/')
			final = i;
	}

	char* dirName;
	if (final == 0) {
		dirName = malloc(1);
		strncpy(dirName, path, 1);
		*(dirName + 1) = '\0';
	} else {
		dirName = malloc(final);
		strncpy(dirName, path, final);
		*(dirName + final) = '\0';
	}

	char* baseName = malloc(strlen(path) - final - 1);
	strncpy(baseName, path + final + 1, strlen(path) - final - 1);
	*(baseName + strlen(path) - final - 1) = '\0';

	// Step 2: Call get_node_by_path() to get inode of target directory
	struct inode* targetInode = malloc(sizeof(struct inode));
	int found = get_node_by_path(path, 0, targetInode);
	if (found == -1) {
		free(targetInode);

		return -ENOENT;
	}

	// Step 3: Clear data block bitmap of target directory
	for (i = 0; i < 16; i++) {
		int dataBlockNumber = targetInode -> direct_ptr[i];
		if (dataBlockNumber == 0)
			continue;

		unset_bitmap(dataBlockBitmap, dataBlockNumber);
	}

	bio_write(superblock -> d_bitmap_blk, dataBlockBitmap);

	// Step 4: Clear inode bitmap and its data block
	unset_bitmap(inodeBitmap, targetInode -> ino);
	bio_write(superblock -> i_bitmap_blk, inodeBitmap);

	targetInode -> valid = 0;
	writei(targetInode -> ino, targetInode);
	free(targetInode);

	// Step 5: Call get_node_by_path() to get inode of parent directory
	struct inode* parentInode = malloc(sizeof(struct inode));
	get_node_by_path(dirName, 0, parentInode);

	// Step 6: Call dir_remove() to remove directory entry of target directory in its parent directory
	dir_remove(*parentInode, baseName, strlen(path) - final - 1);

	free(dirName);
	free(baseName);
	free(parentInode);

	return 0;
}

static int tfs_releasedir(const char *path, struct fuse_file_info *fi) {
	printf("Entering tfs_releasedir\n");
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
	printf("Entering tfs_create\n");
	// Step 1: Use dirname() and basename() to separate parent directory path and target file name
	int final = 0;

	int i;
	for (i = 2; i < strlen(path); i++) {
		if (*(path + i) == '/')
			final = i;
	}

	char* dirName;
	if (final == 0) {
		dirName = malloc(1);
		strncpy(dirName, path, 1);
		*(dirName + 1) = '\0';
	} else {
		dirName = malloc(final);
		strncpy(dirName, path, final);
		*(dirName + final) = '\0';
	}

	char* baseName = malloc(strlen(path) - final - 1);
	strncpy(baseName, path + final + 1, strlen(path) - final - 1);
	*(baseName + strlen(path) - final - 1) = '\0';

	// Step 2: Call get_node_by_path() to get inode of parent directory
	struct inode* dirInode = malloc(sizeof(struct inode));

	if (final == 0)
		readi(0, dirInode);
	else
		get_node_by_path(dirName, 0, dirInode);
	
	struct dirent *newDirent = NULL;
	int found = dir_find(dirInode -> ino, baseName, strlen(path) - final - 1, newDirent);
	if (found == 1) {
		free(dirName);
		free(baseName);
		free(dirInode);

		return -ENOENT;
	}

	// Step 3: Call get_avail_ino() to get an available inode number
	int newInodeNumber = get_avail_ino();

	// Step 4: Call dir_add() to add directory entry of target file to parent directory
	dir_add(*dirInode, newInodeNumber, baseName, strlen(path) - final - 1);

	// Step 5: Update inode for target file
	struct inode* newInode = malloc(sizeof(struct inode));

	newInode -> ino = newInodeNumber;
	newInode -> valid = 1;
	newInode ->	size = 0;
	newInode ->	type = 1;
	newInode -> link = 1;
	newInode -> direct_ptr[16] = 0;
	newInode -> indirect_ptr[8] = 0;

	struct stat* stat = malloc(sizeof(struct stat));
	stat -> st_ino = newInode -> ino;
	stat -> st_mode = S_IFREG | mode;
	stat -> st_nlink = 1;
	stat -> st_uid = getuid();
	stat -> st_gid = getgid();
	stat -> st_size = 0;
	stat -> st_atime = time(NULL);
	stat -> st_mtime = time(NULL);

	newInode -> vstat = *stat;

	// Step 6: Call writei() to write inode to disk
	writei(newInodeNumber, newInode);

	free(dirInode);
	free(newInode);
	free(stat);
	free(dirName);
	free(baseName);

	printf("Leaving tfs_create\n");

	return 0;
}

static int tfs_open(const char *path, struct fuse_file_info *fi) {
	printf("Entering tfs_open\n");
	// Step 1: Call get_node_by_path() to get inode from path
	struct inode* inode = malloc(sizeof(struct inode));
	int found = get_node_by_path(path, 0, inode);

	// Step 2: If not find, return -1
	free(inode);
	if (found == -1)
		return -ENOENT;
	else
    	return 0;
}

static int tfs_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	printf("Entering tfs_read\n");
	// Step 1: You could call get_node_by_path() to get inode from path
	struct inode* inode = malloc(sizeof(struct inode));
	int found = get_node_by_path(path, 0, inode);
	if (found == -1) {
		free(inode);

		return -ENOENT;
	}

	// Step 2: Based on size and offset, read its data blocks from disk
	int currentBlock = offset / BLOCK_SIZE;
	int blockOffset = offset % BLOCK_SIZE;

	int amountCopiedLeft = size;
	while (currentBlock < 16 && amountCopiedLeft > 0) {
		int dataBlockNumber = inode -> direct_ptr[currentBlock];

		void* dataBlock = malloc(BLOCK_SIZE);
		bio_read(dataBlockNumber, dataBlock);

		// Step 3: copy the correct amount of data from offset to buffer
		int copyAmount;
		if (blockOffset != 0) {
			copyAmount = BLOCK_SIZE - blockOffset;
			if (copyAmount > amountCopiedLeft)
				copyAmount = amountCopiedLeft;

			memcpy(buffer + size - amountCopiedLeft, dataBlock + blockOffset, copyAmount);

			blockOffset = 0;
		} else {
			if (amountCopiedLeft > BLOCK_SIZE)
				copyAmount = BLOCK_SIZE;
			else
				copyAmount = amountCopiedLeft;

			memcpy(buffer + size - amountCopiedLeft, dataBlock, copyAmount);
		}

		amountCopiedLeft -= copyAmount;
		currentBlock++;

		free(dataBlock);
	}

	free(inode);
	// Note: this function should return the amount of bytes you copied to buffer
	return size - amountCopiedLeft;
}

static int tfs_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	printf("Entering tfs_write\n");
	// Step 1: You could call get_node_by_path() to get inode from path
	struct inode* inode = malloc(sizeof(struct inode));
	int found = get_node_by_path(path, 0, inode);
	if (found == -1) {
		free(inode);

		return -ENOENT;
	}

	// Step 2: Based on size and offset, read its data blocks from disk
	int currentBlock = offset / BLOCK_SIZE;
	int blockOffset = offset % BLOCK_SIZE;

	int amountCopiedLeft = size;
	while (currentBlock < 16 && amountCopiedLeft > 0) {
		int dataBlockNumber = inode -> direct_ptr[currentBlock];

		void* dataBlock = malloc(BLOCK_SIZE);
		bio_read(dataBlockNumber, dataBlock);

		// Step 3: copy the correct amount of data from offset to disk
		int copyAmount;
		if (blockOffset != 0) {
			copyAmount = BLOCK_SIZE - blockOffset;
			if (copyAmount > amountCopiedLeft)
				copyAmount = amountCopiedLeft;

			memcpy(dataBlock + blockOffset, buffer + size - amountCopiedLeft, copyAmount);
			bio_write(dataBlockNumber, dataBlock);

			blockOffset = 0;
		} else {
			if (amountCopiedLeft > BLOCK_SIZE)
				copyAmount = BLOCK_SIZE;
			else
				copyAmount = amountCopiedLeft;

			memcpy(dataBlock, buffer + size - amountCopiedLeft, copyAmount);
			bio_write(dataBlockNumber, dataBlock);
		}

		amountCopiedLeft -= copyAmount;
		currentBlock++;

		free(dataBlock);
	}

	// Step 4: Update the inode info and write it to disk
	inode -> size += size - amountCopiedLeft;
	(inode -> vstat).st_size += size - amountCopiedLeft;

	writei(inode -> ino, inode);
	free(inode);
	// Note: this function should return the amount of bytes you write to disk
	return size - amountCopiedLeft;
}

static int tfs_unlink(const char *path) {
	printf("Entering tfs_unlink\n");
	// Step 1: Use dirname() and basename() to separate parent directory path and target file name
	int final = 0;

	int i;
	for (i = 2; i < strlen(path); i++) {
		if (*(path + i) == '/')
			final = i;
	}

	char* dirName;
	if (final == 0) {
		dirName = malloc(1);
		strncpy(dirName, path, 1);
		*(dirName + 1) = '\0';
	} else {
		dirName = malloc(final);
		strncpy(dirName, path, final);
		*(dirName + final) = '\0';
	}

	char* baseName = malloc(strlen(path) - final - 1);
	strncpy(baseName, path + final + 1, strlen(path) - final - 1);
	*(baseName + strlen(path) - final - 1) = '\0';

	// Step 2: Call get_node_by_path() to get inode of target file
	struct inode* targetInode = malloc(sizeof(struct inode));
	int found = get_node_by_path(path, 0, targetInode);
	if (found == -1) {
		free(targetInode);

		return -ENOENT;
	}

	// Step 3: Clear data block bitmap of target file
	for (i = 0; i < 16; i++) {
		int dataBlockNumber = targetInode -> direct_ptr[i];
		if (dataBlockNumber == 0)
			continue;

		unset_bitmap(dataBlockBitmap, dataBlockNumber);
	}

	// Step 4: Clear inode bitmap and its data block
	unset_bitmap(inodeBitmap, targetInode -> ino);
	bio_write(superblock -> i_bitmap_blk, inodeBitmap);

	targetInode -> valid = 0;
	writei(targetInode -> ino, targetInode);
	free(targetInode);

	// Step 5: Call get_node_by_path() to get inode of parent directory
	struct inode* parentInode = malloc(sizeof(struct inode));
	get_node_by_path(dirName, 0, parentInode);

	// Step 6: Call dir_remove() to remove directory entry of target file in its parent directory
	dir_remove(*parentInode, baseName, strlen(path) - final - 1);

	free(dirName);
	free(baseName);
	free(parentInode);

	return 0;
}

static int tfs_truncate(const char *path, off_t size) {
	printf("Entering tfs_truncate\n");
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_release(const char *path, struct fuse_file_info *fi) {
	printf("Entering tfs_release\n");
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
	printf("Leaving tfs_release\n");
	return 0;
}

static int tfs_flush(const char * path, struct fuse_file_info * fi) {
	printf("Entering tfs_flush\n");
	// For this project, you don't need to fill this function
	// But DO NOT DELETE IT!
    return 0;
}

static int tfs_utimens(const char *path, const struct timespec tv[2]) {
	printf("Entering tfs_utimens\n");
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

