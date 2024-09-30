#include <complex.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 0;
    }
    char *filename = argv[1];

    int f = open(filename, O_RDONLY);
    if (f == -1) {
        int err = errno;
        fprintf(stderr, "error %d\n", err);
        return 0;
    }

    // if (ioctl(f, EVIOCGRAB, 1) == -1) {
    //     int err = errno;
    //     fprintf(stderr, "error %d\n", err);
    //     return 0;
    // }

    struct input_event event;
    while (read(f, &event, sizeof(event))) {
        printf("%ld.%06ld %x %x %x\n", event.time.tv_sec, event.time.tv_usec, event.type,
               event.code, event.value);
    }

    close(f);

    return 0;
}
