#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "nk/signals.h"

static volatile bool g_child_done = false;
static volatile bool g_terminate = false;

static void sighandler(int sig)
{
    switch (sig) {
        case SIGTERM:
        case SIGINT:
        case SIGHUP:
            g_terminate = true;
            break;
        case SIGCHLD:
            g_child_done = true;
            break;
    }
}

static void free_children(void)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

static void mark_tunnel_down(const char *tag)
{
    static const char false_text[] = "false\n";
    char target[1024];
    size_t written = snprintf(target, sizeof target, "/etc/openvpn/state/%s-ISUP", tag);
    if (written >= sizeof target) {
        printf("unable to write to -ISUP file, path would be too long\n");
        exit(EXIT_FAILURE);
    }
    FILE *f = fopen(target, "w+");
    if (!f) {
        printf("unable to open -ISUP file\n");
        exit(EXIT_FAILURE);
    }
    written = fwrite(false_text, 1, sizeof false_text, f);
    if (written < sizeof false_text) {
        printf("failed to write false_text: %zu < %zu\n", written, sizeof false_text);
        exit(EXIT_FAILURE);
    }
    if (fclose(f)) {
        printf("failed to close false_text\n");
        exit(EXIT_FAILURE);
    }
    // It doesn't really matter whether the disk is sync'ed; the buffer
    // cache is good enough.
}

static void reload_firewall(void)
{
    char * const fwargs[] = { "/sbin/fw", "reload", NULL };
    g_child_done = false;
    g_terminate = false;
    int child_pid = fork();
    switch (child_pid) {
        case 0: {
            execv("/sbin/fw", fwargs);
            exit(EXIT_FAILURE);
        }
        case -1: {
            printf("fork failed\n");
            exit(EXIT_FAILURE);
        }
        default: {
            // parent
            while (!g_child_done && !g_terminate) {
                sleep(60 * 1000);
            }
            break;
        }
    }
}

// 0       1   2               3...
// program tag OpenVPN_program ARGS
int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("usage: %s <TAG> <PATH_TO_OPENVPN> <ARGS...>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *tag = argv[1];

    if (faccessat(AT_FDCWD, argv[2], R_OK|X_OK, AT_EACCESS)) {
        printf("cannot find openvpn program at %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    disable_signal(SIGPIPE);
    disable_signal(SIGUSR1);
    disable_signal(SIGUSR2);
    disable_signal(SIGTSTP);
    disable_signal(SIGTTIN);

    hook_signal(SIGCHLD, sighandler, SA_NOCLDSTOP);
    hook_signal(SIGHUP, sighandler, 0);
    hook_signal(SIGINT, sighandler, 0);
    hook_signal(SIGTERM, sighandler, 0);

    int child_pid = fork();

    switch (child_pid) {
        case 0: {
            execv(argv[2], argv + 2);
            exit(EXIT_FAILURE);
        }
        case -1: {
            printf("fork failed\n");
            exit(EXIT_FAILURE);
        }
        default: {
            // parent
            while (!g_child_done && !g_terminate) {
                sleep(60 * 1000);
            }
            if (g_terminate) {
                kill(child_pid, SIGTERM);
                sleep(1000);
                kill(child_pid, SIGKILL);
            }
            free_children();
            mark_tunnel_down(tag);
            reload_firewall();
            break;
        }
    }

    return 0;
}

