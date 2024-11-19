#include "store.h"
#include "ff.h"
#include <stdio.h>

FATFS fs_storage;

struct store current_storage;

void print_error(FRESULT fr, const char *msg);

void mount_sd()
{
    FATFS *fs = &fs_storage;
    if (fs->id != 0) {
        print_error(FR_DISK_ERR, "Already mounted.");
        return;
    }
    f_mount(fs, "", 1);
}

void save_store(struct store storage) {
    FIL fil;        /* File object */
    FRESULT fr;     /* FatFs return code */
    fr = f_open(&fil, SCORE_PATH, FA_WRITE);
    if (fr) {
        print_error(fr, SCORE_PATH);
        return;
    }

    UINT write_len;
    f_write(&fil, &storage, sizeof(struct store), &write_len);
    printf("BYTES WRITTEN: %d\n\r", write_len);

    f_close(&fil);
}

struct store get_store() {
    FIL fil;        /* File object */
    FRESULT fr;     /* FatFs return code */

    struct store storage = {0, 0};

    /* Open a text file */
    fr = f_open(&fil, SCORE_PATH, FA_READ);
    if (fr) {
        print_error(fr, SCORE_PATH);
        return storage;
    }

    UINT br;

    f_read(&fil, &storage, sizeof(struct store), &br);
    printf("BYTES READ: %d\n\r", br);
    f_close(&fil);

    return storage;
}

void save_highscore(int highscore) {
    current_storage = get_store();
    current_storage.highscore = highscore;
    save_store(current_storage);
}

int get_highscore() {
    current_storage = get_store();
    return current_storage.highscore;
}

void save_seed(int seed) {
    current_storage = get_store();
    current_storage.seed = seed;
    save_store(current_storage);
}

int get_seed() {
    current_storage = get_store();
    return current_storage.seed;
}