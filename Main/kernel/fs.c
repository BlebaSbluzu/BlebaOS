#include "fs.h"
#include "console.h"

typedef struct {
    int used;
    char name[FS_MAX_NAME];
    char data[FS_MAX_DATA];
} file_t;

static file_t files[FS_MAX_FILES];

static int str_equal_fs(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }

    return *a == *b;
}

static void str_copy_limit(char* dest, const char* src, int max) {
    int i = 0;

    while (src[i] && i < max - 1) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

static int find_file(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].used && str_equal_fs(files[i].name, name)) {
            return i;
        }
    }

    return -1;
}

static int find_free_slot(void) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!files[i].used) {
            return i;
        }
    }

    return -1;
}

void fs_init(void) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        files[i].used = 0;
        files[i].name[0] = '\0';
        files[i].data[0] = '\0';
    }

    console_write("In-memory filesystem initialized.\n");
}

int fs_create(const char* name) {
    if (!name || name[0] == '\0') {
        return 0;
    }

    if (find_file(name) != -1) {
        return 0;
    }

    int slot = find_free_slot();

    if (slot == -1) {
        return 0;
    }

    files[slot].used = 1;
    str_copy_limit(files[slot].name, name, FS_MAX_NAME);
    files[slot].data[0] = '\0';

    return 1;
}

int fs_delete(const char* name) {
    int index = find_file(name);

    if (index == -1) {
        return 0;
    }

    files[index].used = 0;
    files[index].name[0] = '\0';
    files[index].data[0] = '\0';

    return 1;
}

int fs_write(const char* name, const char* data) {
    int index = find_file(name);

    if (index == -1) {
        return 0;
    }

    str_copy_limit(files[index].data, data, FS_MAX_DATA);
    return 1;
}

const char* fs_read(const char* name) {
    int index = find_file(name);

    if (index == -1) {
        return NULL;
    }

    return files[index].data;
}

void fs_list(void) {
    int count = 0;

    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].used) {
            console_write(files[i].name);
            console_write("\n");
            count++;
        }
    }

    if (count == 0) {
        console_write("No files.\n");
    }
}

int fs_exists(const char* name) {
    return find_file(name) != -1;
}

int fs_count(void) {
    int count = 0;

    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (files[i].used) {
            count++;
        }
    }

    return count;
}