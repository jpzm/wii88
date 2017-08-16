#include "wii.h"

struct wii_data wii;

u16 wii_wpad_map[WII_WPAD_BUTTONS] = { WPAD_BUTTON_2,
                                       WPAD_BUTTON_1,
                                       WPAD_BUTTON_B,
                                       WPAD_BUTTON_A,
                                       WPAD_BUTTON_MINUS,
                                       WPAD_BUTTON_HOME,
                                       WPAD_BUTTON_LEFT,
                                       WPAD_BUTTON_RIGHT,
                                       WPAD_BUTTON_DOWN,
                                       WPAD_BUTTON_UP,
                                       WPAD_BUTTON_PLUS };

u16 wii_wpad_key88[WII_WPAD_N][WII_WPAD_BUTTONS] =
{ { KEY88_SPACE,      // 1 WII_WPAD_2
    KEY88_Z,          // 1 WII_WPAD_1
    KEY88_X,          // 1 WII_WPAD_B
    NULL,             // 1 WII_WPAD_A
    KEY88_STOP,       // 1 WII_WPAD_MINUS
    KEY88_F12,        // 1 WII_WPAD_HOME
    KEY88_KP_2,       // 1 WII_WPAD_LEFT
    KEY88_KP_8,       // 1 WII_WPAD_RIGHT
    KEY88_KP_6,       // 1 WII_WPAD_DOWN
    KEY88_KP_4,       // 1 WII_WPAD_UP
    KEY88_RETURN },   // 1 WII_WPAD_PLUS

  { KEY88_TAB,        // 2 WII_WPAD_2
    NULL,             // 2 WII_WPAD_1
    NULL,             // 2 WII_WPAD_B
    NULL,             // 2 WII_WPAD_A
    KEY88_STOP,       // 2 WII_WPAD_MINUS
    KEY88_F12,        // 2 WII_WPAD_HOME
    NULL,             // 2 WII_WPAD_LEFT
    NULL,             // 2 WII_WPAD_RIGHT
    KEY88_G,          // 2 WII_WPAD_DOWN
    KEY88_D,          // 2 WII_WPAD_UP
    KEY88_RETURN } }; // 2 WII_WPAD_PLUS

void wii_init(void)
{
    u32 i, j;

    strcpy(wii.path, WII_PATH);
    fatMountSimple("sd", &__io_wiisd);

    for (j = 0; j < WII_WPAD_N; j++)
    {
        wii.wpad[j].cursor.x = 0;
        wii.wpad[j].cursor.y = 0;
        wii.wpad[j].cursor.apply = FALSE;

        for (i = 0; i < WII_WPAD_BUTTONS; i++)
        {
            wii.wpad[j].data = NULL;
            wii.wpad[j].button[i].id = wii_wpad_map[i];
            wii.wpad[j].button[i].apply = FALSE;
            wii.wpad[j].button[i].state = FALSE;
        }
    }

    WPAD_Init();
}

void wii_input(void)
{
    WPAD_ScanPads();

    wii_read_wpad(0);
    wii_read_wpad(1);

    wii_apply_wpad(0);
    wii_apply_wpad(1);
}

void wii_apply_wpad(u8 index)
{
    u8 i, state;

    if (wii.wpad[index].button[WII_WPAD_HOME].apply)
        quasi88_exit(EXIT_SUCCESS);

    /*
     * Verify if the WPAD is pointed to the screen.
     */
    if (wii.wpad[index].cursor.valid)
    {
        /*
         * WPAD as mouse.
         */
        Sint16 x, y;

        if (wii.wpad[index].button[WII_WPAD_A].apply)
        {
            quasi88_mouse(KEY88_MOUSE_L,
                          wii.wpad[index].button[WII_WPAD_A].state);
            wii.wpad[index].button[WII_WPAD_A].apply = 0;
        }

        if (wii.wpad[index].button[WII_WPAD_B].apply)
        {
            quasi88_mouse(KEY88_MOUSE_R,
                          wii.wpad[index].button[WII_WPAD_B].state);
            wii.wpad[index].button[WII_WPAD_B].apply = 0;
        }

        if (wii.wpad[index].cursor.apply)
        {
            x = wii.wpad[index].cursor.x;
            y = wii.wpad[index].cursor.y;
		    quasi88_mouse_moved_abs(x, y);
            wii.wpad[index].cursor.apply = FALSE;
        }

        if (wii.wpad[index].button[WII_WPAD_PLUS].apply)
        {
            quasi88_key(KEY88_F12,
                        wii.wpad[index].button[WII_WPAD_PLUS].state);
            wii.wpad[index].button[WII_WPAD_PLUS].apply = 0;
        }

        if (wii.wpad[index].button[WII_WPAD_MINUS].apply)
        {
            quasi88_key(KEY88_ESC,
                        wii.wpad[index].button[WII_WPAD_MINUS].state);
            wii.wpad[index].button[WII_WPAD_MINUS].apply = 0;
        }
    }
    else
    {
        /*
         * WPAD as controller.
         */
        for (i = 0; i < WII_WPAD_BUTTONS; i++)
        {
            if (wii.wpad[index].button[i].apply)
            {
                if (wii_wpad_key88[index][i])
                    quasi88_key(wii_wpad_key88[index][i],
                                wii.wpad[index].button[i].state);
                wii.wpad[index].button[i].apply = 0;
            }
        }
    }
}

void wii_read_wpad(u8 index)
{
    u32 ans, type, i;

    wii.wpad[index].data = WPAD_Data(index);

    ans = wii.wpad[index].data->btns_h;
    for (i = 0; i < WII_WPAD_BUTTONS; i++)
    {
        if (ans & wii.wpad[index].button[i].id)
        {
            if (wii.wpad[index].button[i].state == FALSE)
                wii.wpad[index].button[i].apply = TRUE;
            wii.wpad[index].button[i].state = TRUE;
        }
        else
        {
            if (wii.wpad[index].button[i].state == TRUE)
                wii.wpad[index].button[i].apply = TRUE;
            wii.wpad[index].button[i].state = FALSE;
        }
    }

    wii.wpad[index].cursor.valid = wii.wpad[index].data->ir.valid;

    if ((wii.wpad[index].cursor.x != wii.wpad[index].data->ir.x) ||
        (wii.wpad[index].cursor.y != wii.wpad[index].data->ir.y))
    {
        wii.wpad[index].cursor.x = wii.wpad[index].data->ir.x;
        wii.wpad[index].cursor.y = wii.wpad[index].data->ir.y;
        wii.wpad[index].cursor.apply = TRUE;
    }
}
