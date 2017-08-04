#ifndef WII_H
#define WII_H

#include <string.h>

#include <fat.h>
#include <sys/dir.h>
#include <sdcard/wiisd_io.h>

#define WII_BASE_PATH "sd:/wii88/"

extern struct wii_data
{
    char path[261]; // Maximum filename size (255) plus SD dir path size (5).
} wii;

void wii_init(void);

#endif
