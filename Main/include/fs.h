#ifndef FS_H
#define FS_H


#include "common.h"

#define FS_MAX_FILES 16
#define FS_MAX_NAME  32
#define FS_MAX_DATA  256

void fs_init(void);

int fs_create(const char* name);
int fs_delete(const char* name);
int fs_write(const char* name, const char* data);
const char* fs_read(const char* name);
void fs_list(void);

int fs_exists(const char* name);
int fs_count(void);

#endif