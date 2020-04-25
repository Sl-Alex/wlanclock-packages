#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <linux/input.h>
#include "libevdev/libevdev.h"
#include <stdint.h>
#include <unistd.h>

#include <signal.h>
#include <time.h>

#include "evdev.h"
#include "ubus_client.h"

static volatile int timer_expired = 0;
static int dx = 0;
static int dy = 0;
static int btn = 0;
static int dbtn = 0;

static int ddx = 0;
static int ddy = 0;

static void
print_abs_bits(struct libevdev *dev, int axis)
{
    const struct input_absinfo *abs;

    if (!libevdev_has_event_code(dev, EV_ABS, axis))
        return;

    abs = libevdev_get_abs_info(dev, axis);

    printf("    Value   %6d\n", abs->value);
    printf("    Min %6d\n", abs->minimum);
    printf("    Max %6d\n", abs->maximum);
    if (abs->fuzz)
        printf("    Fuzz    %6d\n", abs->fuzz);
    if (abs->flat)
        printf("    Flat    %6d\n", abs->flat);
    if (abs->resolution)
        printf("    Resolution  %6d\n", abs->resolution);
}

static void
print_code_bits(struct libevdev *dev, unsigned int type, unsigned int max)
{
    unsigned int i;
    for (i = 0; i <= max; i++) {
        if (!libevdev_has_event_code(dev, type, i))
            continue;

        printf("    Event code %i (%s)\n", i, libevdev_event_code_get_name(type, i));
        if (type == EV_ABS)
            print_abs_bits(dev, i);
    }
}

static void
print_info(struct libevdev *dev)
{
    printf("Input device ID: bus %#x vendor %#x product %#x\n",
            libevdev_get_id_bustype(dev),
            libevdev_get_id_vendor(dev),
            libevdev_get_id_product(dev));
    printf("Evdev version: %x\n", libevdev_get_driver_version(dev));
    printf("Input device name: \"%s\"\n", libevdev_get_name(dev));
    printf("Phys location: %s\n", libevdev_get_phys(dev));
    printf("Uniq identifier: %s\n", libevdev_get_uniq(dev));

    unsigned int i;
    printf("Supported events:\n");

    for (i = 0; i <= EV_MAX; i++) {
        if (libevdev_has_event_type(dev, i))
            printf("  Event type %d (%s)\n", i, libevdev_event_type_get_name(i));
        switch(i) {
            case EV_KEY:
                print_code_bits(dev, EV_KEY, KEY_MAX);
                break;
            case EV_REL:
                print_code_bits(dev, EV_REL, REL_MAX);
                break;
            case EV_ABS:
                print_code_bits(dev, EV_ABS, ABS_MAX);
                break;
            case EV_LED:
                print_code_bits(dev, EV_LED, LED_MAX);
                break;
        }
    }
}

void parse_gesture(int dx, int dy, int btn)
{
    const int thr = 10;
    const float prop = 1.5f;
    int adx, ady;
    adx = (dx > 0) ? dx : -dx;
    ady = (dy > 0) ? dy : -dy;
    if ((adx < thr) && (ady < thr) && dbtn)
    {
        printf("Single click\n");
    }
    else if ((adx > thr) && (((float)adx/(float)ady) > prop))
    {
        printf("%s %s\n", dbtn ? "Drag" : "Swipe", (dx > 0) ? "right" : "left");
        if (dbtn == 0)
        {
            ubus_client_send((dx > 0) ? 1 : 0);
        }
    }
    else if ((ady > thr) && (((float)ady/(float)adx) > prop))
    {
        printf("%s %s\n", dbtn ? "Drag" : "Swipe", (dy > 0) ? "down" : "up");
        if (dbtn == 0)
        {
            ubus_client_send((dy > 0) ? 3 : 2);
        }
    }
    else
    {
        printf("Can't recognize gesture\n");
    }
    printf("\n");
}

static int
print_event(struct input_event *ev)
{
    if (ev->type == EV_SYN)
        printf("Event: time %ld.%06ld, dx = %06d, dy = %06d%s\n",
                ev->time.tv_sec,
                ev->time.tv_usec,
                dx,dy,
                btn ? ", PRESSED" : "");
    else if ((ev->type == EV_REL) && (ev->code == REL_X))
    {
        dx += ev->value;
        ddx += ev->value;
    }
    else if ((ev->type == EV_REL) && (ev->code == REL_Y))
    {
        dy += ev->value;
        ddy += ev->value;
    }
    else if ((ev->type == EV_KEY) && (ev->code == BTN_LEFT))
    {
        btn = ev->value;
        dbtn = 1;
    }
    return 0;
}

static int
print_sync_event(struct input_event *ev)
{
    printf("SYNC: ");
    print_event(ev);
    return 0;
}

timer_t timer;

void sig_alarm_handler(int sig, siginfo_t *info, void *ptr)
{
    timer_expired = 1;
}

void start_timer(uint32_t timeout)
{
    static struct itimerspec its;
    static struct timespec   ts;
    if (0 != clock_gettime(CLOCK_BOOTTIME, &ts))
    {
        perror("Can't get time\n");
        return;
    }
    int64_t timeout_ns = ts.tv_nsec + timeout * 1000000;
    
    its.it_value.tv_sec     = ts.tv_sec + timeout_ns / 1000000000;
    its.it_value.tv_nsec    = timeout_ns % 1000000000;

    if (0>timer_settime(timer, TIMER_ABSTIME, &its, NULL) )
    {
        perror("Can't start timer\n");
    }
}

int
evdev_loop(touchpad_config_t * cfg)
{
    struct libevdev *dev = NULL;
    int fd;
    int rc = 1;

    fd = open((char *)cfg->device, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open device %s\n", cfg->device);
        return 1;
    }

    rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
        return 1;
    }

    static struct sigaction sa;
    sa.sa_sigaction = sig_alarm_handler;
    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGALRM, &sa, 0);

    static struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
    sev.sigev_value.sival_ptr = NULL;
    if (0 > timer_create(CLOCK_BOOTTIME, &sev, &timer))
    {
        perror("Can't create timer\n");
    }

    print_info(dev);

    timer_expired = 1;

    do {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL|LIBEVDEV_READ_FLAG_BLOCKING, &ev);
        if (rc == LIBEVDEV_READ_STATUS_SYNC) {
            while (rc == LIBEVDEV_READ_STATUS_SYNC) {
                print_sync_event(&ev);
                rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_SYNC, &ev);
            }
        } else if (rc == LIBEVDEV_READ_STATUS_SUCCESS)
            print_event(&ev);

        if (timer_expired)
        {
            if ((ddx != 0) || (ddy != 0) || (btn != 0))
            {
                ddx = 0;
                ddy = 0;
                timer_expired = 0;
                start_timer(cfg->timeout);
            }
            else
            {
                if ((dx != 0) || (dy != 0) || (dbtn != 0))
                {
                    parse_gesture(dx, dy, dbtn);
                    dx = 0;
                    dy = 0;
                    dbtn = 0;
                }
            }
        }
    } while (rc == LIBEVDEV_READ_STATUS_SYNC || rc == LIBEVDEV_READ_STATUS_SUCCESS || rc == -EAGAIN || rc == -EINTR);

    if (rc != LIBEVDEV_READ_STATUS_SUCCESS && rc != -EAGAIN)
        fprintf(stderr, "Failed to handle events: %s\n", strerror(-rc));

    libevdev_free(dev);
    close(fd);

    return 0;
}
