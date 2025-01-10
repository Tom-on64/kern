#ifndef _FAT32_H
#define _FAT32_H

#include "stdint.h"
#include "defs.h"
#include "disk.h"

// Macros
#define FAT32_PTR(_p)			((struct fat32_ctx*)(_p))
#define FAT32_DIR_PTR(_p)		((struct fat32_fd*)(_p))
#define FAT32_FULL_CLUST(_hi, _lo) 	(((uint32_t)(_hi) << 16) | (_lo))
#define FAT32_INODE_GEN(_dir, _i)	((_dir) * 100 + (_i))

// Long File Name (LFN)
#define LFN_MAX		20
#define LFN_MAX_CHARS	256

// Attributes
#define FAT32_ATTR_READ_ONLY	(1 << 0)
#define FAT32_ATTR_HIDDEN	(1 << 1)
#define FAT32_ATTR_SYSTEM	(1 << 2)
#define FAT32_ATTR_VOLUME_ID	(1 << 3)
#define FAT32_ATTR_DIRECTORY	(1 << 4)
#define FAT32_ATTR_ARCHIVE	(1 << 5)
#define FAT32_ATTR_LFN					\
	(FAT32_ATTR_READ_ONLY | FAT32_ATTR_HIDDEN 	\
	 FAT32_ATTR_SYSTEM    | FAT32_ATTR_VOLUME_ID)

/* On-disk structures */ 

// FAT32 Bios Parameter Block & Extended BPB
struct fat32_bpb {
	uint8_t bootjmp[3];
	char oemName[8];
	uint16_t bytesPerSect;
	uint8_t sectsPerClust;
	uint16_t reservedSects;
	uint8_t fatCount;
	uint16_t rootEntryCount;
	uint16_t totalSects_16;
	uint8_t mediaType;
	uint16_t sectsPerFat_16;
	uint16_t sectsPerTrack;
	uint16_t headCount;
	uint32_t hiddenSects;
	uint32_t sectCount_32;
	// Extended
	uint32_t sectsPerFat_32;
	uint16_t flags;
	uint16_t fatVersion;
	uint32_t rootClust;
	uint16_t fsinfoSect;
	uint16_t backupBootsect;
	uint8_t _reserved[12];
	uint8_t driveNum;
	uint8_t winNTFlags;
	uint8_t signature;
	uint32_t volumeID;
	char volumeLabel[11];
	char systemID[8];
	uint8_t bootcode[420];
	uint16_t bootsig;
} __packed;

// FAT32 Directory Entry
struct fat32_dentry {
	char name[8];
	char ext[3];
	uint8_t attr;
	uint8_t userattr;
	char undelete;
	uint16_t createTime;
	uint16_t createDate;
	uint16_t accessDate;
	uint16_t clustHigh;
	uint16_t modifyTime;
	uint16_t modifyDate;
	uint16_t clustLow;
	uint32_t size;
} __packed;

// FAT32 Long File Name entry
struct fat32_lfn {
	uint8_t order;
	uint8_t name1[10];
	uint8_t attr;
	uint8_t type;
	uint8_t checksum;
	uint8_t name2[12];
	uint16_t zero;
	uint8_t name3[4];
} __packed;

/* In memory structures */
struct fat32_ctx {
	size_t offsetBase;
	size_t offsetFats;
	size_t offsetClusts;

	struct fat32_bpb bpb;
};

struct fat32_fd {
	uint32_t ptr;
	uint8_t index;
	uint32_t dirStart;
	uint32_t dirCurrent;
	struct fat32_dentry dentry;
};

// Handlers
int 	fat32_init(struct partition* p);
int	fat32_open(char* filename, int flags, ...);	/* We won't use the va_args */


void*	fat32_fopen(const char* path, const char* mode);
void	fat32_fclose(void* file);
int	fat32_fgetc(void* file);
char*	fat32_fgets(char* s, size_t n, void* file);
int	fat32_fread(const void* buf, size_t size, size_t count, void* file);
int	fat32_fseek(void* file, size_t offset, int whence);
long	fat32_ftell(void* file);

#endif
