#include "wii.h"

struct wii_data wii;

u16 wii_wpad_map[WII_WPAD_BUTTONS] = { WPAD_BUTTON_2, // from wiiuse/wapd.h
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

char *wii_wpad_str[WII_WPAD_BUTTONS] = { "BUTTON_2", // wii_wpad_enum
                                         "BUTTON_1",
                                         "BUTTON_B",
                                         "BUTTON_A",
                                         "BUTTON_MINUS",
                                         "BUTTON_HOME",
                                         "BUTTON_LEFT",
                                         "BUTTON_RIGHT",
                                         "BUTTON_DOWN",
                                         "BUTTON_UP",
                                         "BUTTON_PLUS" };

u16 wii_wpad_key88[WII_WPAD_N][WII_WPAD_BUTTONS] =
{ { KEY88_INVALID,     // 1 WII_WPAD_2
    KEY88_INVALID,     // 1 WII_WPAD_1
    KEY88_INVALID,     // 1 WII_WPAD_B
    KEY88_INVALID,     // 1 WII_WPAD_A
    KEY88_INVALID,     // 1 WII_WPAD_MINUS
    KEY88_INVALID,     // 1 WII_WPAD_HOME
    KEY88_INVALID,     // 1 WII_WPAD_PLUS
    KEY88_INVALID,     // 1 WII_WPAD_LEFT
    KEY88_INVALID,     // 1 WII_WPAD_RIGHT
    KEY88_INVALID,     // 1 WII_WPAD_DOWN
    KEY88_INVALID },   // 1 WII_WPAD_UP
  { KEY88_INVALID,     // 2 WII_WPAD_2
    KEY88_INVALID,     // 2 WII_WPAD_1
    KEY88_INVALID,     // 2 WII_WPAD_B
    KEY88_INVALID,     // 2 WII_WPAD_A
    KEY88_INVALID,     // 2 WII_WPAD_MINUS
    KEY88_INVALID,     // 2 WII_WPAD_HOME
    KEY88_INVALID,     // 2 WII_WPAD_PLUS
    KEY88_INVALID,     // 2 WII_WPAD_LEFT
    KEY88_INVALID,     // 2 WII_WPAD_RIGHT
    KEY88_INVALID,     // 2 WII_WPAD_DOWN
    KEY88_INVALID } }; // 2 WII_WPAD_UP

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

void wii_apply_rc_string(char *config)
{
    char *option = (char *) malloc(strlen(config));
    char *joytype = (char *) malloc(strlen(config));
    unsigned int i, number, value;

    sscanf(config, "%s %u %s %u", joytype, &number, option, &value);

    // support for at most WII_WPAD_N controllers
    if (number < WII_WPAD_N)
    {
        if (strcasecmp("WPAD", joytype) == 0)
        {
            for (i = 0; i < WII_WPAD_BUTTONS; i++)
            {
                if (strcasecmp(option, wii_wpad_str[i]) == 0)
                {
                    wii_wpad_key88[number - 1][i] = value;
                    break;
                }
            }
        }
    }

    free(option);
    free(joytype);
}

void wii_read_conf_file(const char *filepath)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    if ((fp = fopen(filepath, "r")))
    {
        while ((read = __getline(&line, &len, fp)) != -1)
        {
            // skip empty lines and comments
            if ((read > 0) && (line[0] != '#'))
                wii_apply_rc_string(line);
        }

        if (line)
            free(line);
        fclose(fp);
    }
}

void wii_disk_insert(const char *filepath)
{
    char *file = &(rindex(filepath, '/')[1]); // first char after last '/'
    char *rc = (char *) malloc(strlen(WII_PATH_RC) + strlen(file) + 4);
    struct stat st;

    strcpy(rc, WII_PATH_RC);
    strcat(rc, file);
    strcat(rc, ".rc");

    if (stat(rc, &st) == 0)
        wii_read_conf_file(rc);
    else
        wii_read_conf_file(WII_PATH_RC_DEFAULT);

    free(rc);
}

void wii_apply_wpad(u8 index)
{
    u8 i;

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
    u32 ans, i;

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
