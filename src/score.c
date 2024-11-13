#include "score.h"
#include "ff.h"

FATFS fs_storage;

void mount_sd()
{
    FATFS *fs = &fs_storage;
    if (fs->id != 0) {
        print_error(FR_DISK_ERR, "Already mounted.");
        return;
    }
    f_mount(fs, "", 1);
}

void save_score(int score) {
    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */
    fr = f_open(&fil, SCORE_PATH, FA_WRITE);
    if (fr) {
        print_error(fr, SCORE_PATH);
        return;
    }

    UINT write_len;
    f_write(&fil, &score, sizeof(int), &write_len);
    printf("BYTES WRITTEN: %d\n\r", write_len);

    f_close(&fil);
}

int get_score() {
    FIL fil;        /* File object */
    char line[100]; /* Line buffer */
    FRESULT fr;     /* FatFs return code */

    /* Open a text file */
    fr = f_open(&fil, SCORE_PATH, FA_READ);
    if (fr) {
        print_error(fr, SCORE_PATH);
        return;
    }

    UINT br;
    int score;
    f_read(&fil, &score, sizeof(int), &br);
    printf("BYTES READ: %d\n\r", br);
    fclose(&fil);

    return score;
}