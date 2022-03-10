#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#define LOCKFILE "/var/run/lab1_daemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

sigset_t mask;

void daemonize(const char *cmd) {
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask(0);  // 1

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        puts("getrlimit error");
        exit(1);
    }

    // 2
    if ((pid = fork()) < 0) {
        puts("Error during fork");
        exit(1);
    } else if (pid != 0) {
        exit(0);
    }

    // 3
    if (setsid() == -1) {
        puts("setsid error setsid");
        exit(1);
    }

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        puts("sigaction failed to ignore sighup");
        exit(1);
    }
    
    // 4
    if (chdir("/") < 0) {  
        puts("chdir / failed");
        exit(1);
    }

    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }

    // 5
    for (i = 0; i < rl.rlim_max; i++) {  
        close(i);
    }

    // 6
    fd0 = open("dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != -0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "File descriptors problems %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
    syslog(LOG_WARNING, "Daemon started!");
}

int already_running(void) {
    syslog(LOG_ERR, "Check if already running");

    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);

    if (fd < 0) {
        syslog(LOG_ERR, "Cant open lockfile %s: %s", LOCKFILE,
               strerror(errno));
        exit(1);
    }

    syslog(LOG_WARNING, "Lock file opened");

    if (flock(fd, LOCK_EX | LOCK_NB) != 0) {
        if (errno == EWOULDBLOCK) {
            syslog(LOG_ERR, "Daemon is already running %s: %s", LOCKFILE,
                   strerror(errno));
            close(fd);
            exit(1);
        }

        syslog(LOG_ERR, "Invalid FD");
        exit(1);
    }

    ftruncate(fd, 0);
    sputs(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);

    syslog(LOG_WARNING, "PID writed");

    return 0;
}

void log_time() {
    time_t raw_time;
    struct tm *time_info;
    char buf[70];

    time(&raw_time);
    time_info = localtime(&raw_time);
    syslog(LOG_INFO, (asctime(time_info)));
}

void *thr_fn(void *arg) {
    int signo;
    int code_error = 0;
    while (!code_error) {
        code_error = sigwait(&mask, &signo);
        switch (signo) {
            case SIGHUP:
                syslog(LOG_INFO, "--- SIGHUP");
                break;
            case SIGTERM:
                syslog(LOG_INFO, "--- SIGTERM - KILL APP");
                return 0;
                break;
            default:
                syslog(LOG_INFO, "--- UNKNOWN SIGNAL - %d", signo);
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    int err;
    pthread_t tid;
    char *cmd;
    struct sigaction sa;

    if ((cmd = strrchr(argv[0], '/')) == NULL) {
        cmd = argv[0];
    } else
        cmd++;

    daemonize(cmd);

    if (already_running() != 0) {
        syslog(LOG_ERR, "Daemon is already running\n");
        exit(1);
    }

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        syslog(LOG_ERR, "sigaction failed SIG_DFL on SIGHUP\n");
        exit(1);
    }

    sigfillset(&mask);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0) {
        syslog(LOG_ERR, "pthread_sigmask failed SIG_BLOCK\n");
        exit(1);
    }

    err = pthread_create(&tid, NULL, thr_fn, 0);
    if (err != 0) {
        syslog(LOG_ERR, "Thread creation failed\n");
        exit(1);
    }

    for (int i = 0; i < 100; ++i) {
        log_time();
        sleep(3);
    }

    pthread_join(tid, NULL);

    exit(0);
}
