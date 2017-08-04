#include "wii.h"

struct wii_data wii;

void wii_init(void)
{
    strcpy(wii.path, WII_BASE_PATH);
    fatMountSimple("sd", &__io_wiisd);
}
