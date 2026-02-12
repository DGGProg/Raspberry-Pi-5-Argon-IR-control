#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>

#ifndef KEY_SETTINGS
#define KEY_SETTINGS 0x1a1
#endif

static int last_code = -1;
static long last_time = 0;
static int repeat_mode = 0;

/* ---------- tiempo en ms ---------- */

long now_ms()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}

/* ---------- mapping IR ---------- */

int map_scan_to_key(int scan)
{
    switch(scan)
    {
        case 0xca: return KEY_UP;
        case 0xd2: return KEY_DOWN;
        case 0x99: return KEY_LEFT;
        case 0xc1: return KEY_RIGHT;
        case 0xce: return KEY_ENTER;
        case 0xcb: return KEY_HOMEPAGE;
        case 0x90: return KEY_BACK;
        case 0x9d: return KEY_SETTINGS;
        case 0x80: return KEY_VOLUMEUP;
        case 0x81: return KEY_VOLUMEDOWN;
    }
    return -1;
}

/* ---------- emitir evento ---------- */

void emit(int fd, int type, int code, int val)
{
    struct input_event ie;
    memset(&ie, 0, sizeof(ie));

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    ie.time.tv_sec  = ts.tv_sec;
    ie.time.tv_usec = ts.tv_nsec / 1000;

    ie.type = type;
    ie.code = code;
    ie.value = val;

    write(fd, &ie, sizeof(ie));
}

/* ---------- main ---------- */

int main()
{
    printf("Argon IR Mapper iniciado\n");

    /* ---- abrir IR ---- */

    int evfd = open("/dev/input/event2", O_RDONLY);
    if(evfd < 0)
    {
        perror("event2");
        return 1;
    }

    /* modo nonblock */
    fcntl(evfd, F_SETFL, O_NONBLOCK);

    /* ---- abrir uinput ---- */

    int uifd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(uifd < 0)
    {
        perror("uinput");
        return 1;
    }

    /* ---- declarar capacidades ---- */

    ioctl(uifd, UI_SET_EVBIT, EV_KEY);
    ioctl(uifd, UI_SET_EVBIT, EV_SYN);
    ioctl(uifd, UI_SET_EVBIT, EV_REP);

    ioctl(uifd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);

    ioctl(uifd, UI_SET_KEYBIT, KEY_UP);
    ioctl(uifd, UI_SET_KEYBIT, KEY_DOWN);
    ioctl(uifd, UI_SET_KEYBIT, KEY_LEFT);
    ioctl(uifd, UI_SET_KEYBIT, KEY_RIGHT);
    ioctl(uifd, UI_SET_KEYBIT, KEY_ENTER);
    ioctl(uifd, UI_SET_KEYBIT, KEY_HOMEPAGE);
    ioctl(uifd, UI_SET_KEYBIT, KEY_BACK);
    ioctl(uifd, UI_SET_KEYBIT, KEY_SETTINGS);
    ioctl(uifd, UI_SET_KEYBIT, KEY_VOLUMEUP);
    ioctl(uifd, UI_SET_KEYBIT, KEY_VOLUMEDOWN);

    /* opcionales pero ayudan a Android */
    ioctl(uifd, UI_SET_KEYBIT, KEY_MENU);
    ioctl(uifd, UI_SET_KEYBIT, KEY_POWER);

    /* ---- crear dispositivo virtual ---- */

    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));

    snprintf(usetup.name, UINPUT_MAX_NAME_SIZE, "Argon Android Remote");

    usetup.id.bustype = BUS_USB;
    usetup.id.vendor  = 0x1234;
    usetup.id.product = 0x5678;

    ioctl(uifd, UI_DEV_SETUP, &usetup);
    ioctl(uifd, UI_DEV_CREATE);

    sleep(1);

    struct input_event ev;

    /* ---- loop principal ---- */

    while(1)
    {
        ssize_t n = read(evfd, &ev, sizeof(ev));

        if(n <= 0)
        {
            usleep(2000);
            continue;
        }

        if(ev.type == EV_MSC && ev.code == MSC_SCAN)
        {
            int sc = ev.value;

            /* NEC manda repeat fake */
            if(sc == 0xffffffff)
                continue;

            long t = now_ms();
            long diff = t - last_time;

            /* filtro anti burst */
            if(diff < 80)
                continue;

            int key = map_scan_to_key(sc);
            if(key < 0)
                continue;

            if(sc != last_code)
            {
                repeat_mode = 0;
            }
            else
            {
                if(!repeat_mode && diff < 350)
                    continue;

                if(repeat_mode && diff < 120)
                    continue;

                repeat_mode = 1;
            }

            last_code = sc;
            last_time = t;

            /* emitir press */
            emit(uifd, EV_KEY, key, 1);
            emit(uifd, EV_SYN, SYN_REPORT, 0);

            /* emitir release */
            emit(uifd, EV_KEY, key, 0);
            emit(uifd, EV_SYN, SYN_REPORT, 0);
        }
    }

    return 0;
}
