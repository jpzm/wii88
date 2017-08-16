#ifndef WII_H
#define WII_H

#include <string.h>

#include <fat.h>
#include <sys/dir.h>
#include <sdcard/wiisd_io.h>
#include <wiiuse/wpad.h>

#include "quasi88.h"
#include "keyboard.h"
#include "event.h"

#define WII_PATH                "sd:/wii88/"
#define WII_PATH_MAX_SIZE       260
#define WII_WPAD_N              2     // number of WPAD controllers supported
#define WII_WPAD_BUTTONS        11    // number of buttons on WPAD
#define WII_WPAD_ACTIVE         1
#define WII_WPAD_INACTIVE       0

extern u16 wpad_button_map[WII_WPAD_BUTTONS];

enum wii_wpad_enum
{ WII_WPAD_2,
  WII_WPAD_1,
  WII_WPAD_B,
  WII_WPAD_A,
  WII_WPAD_MINUS,
  WII_WPAD_HOME,
  WII_WPAD_LEFT,
  WII_WPAD_RIGHT,
  WII_WPAD_DOWN,
  WII_WPAD_UP,
  WII_WPAD_PLUS };

struct wii_button
{
    u16 id;
    u8 apply;
    u16 state;
};

struct wii_cursor
{
    int valid;
    u8 apply;
    float x;
    float y;
};

struct wii_wpad
{
    u8 state;
    struct wii_button button[WII_WPAD_BUTTONS];
    struct wii_cursor cursor;
    WPADData *data;
};

extern struct wii_data
{
    char path[WII_PATH_MAX_SIZE + 1]; // plus '\0'
    struct wii_wpad wpad[WII_WPAD_N];
} wii;

void wii_init(void);

void wii_input(void);

void wii_read_wpad(u8 index);

void wii_apply_wpad(u8 index);

#endif
