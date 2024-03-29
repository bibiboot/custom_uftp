#include "nack_timer.h"

void alarm_handler(int signal){
    if (signal != SIGALRM) {
        DBG("Caught wrong signal: %d", signal);
        exit(1);
        return;
    }

    if ((globals.nackl).num_members != 0) {
        send_nack_packet();
        // Register another alarm
        register_signal();
    }
}

void register_signal(){

    struct sigaction sa;
    sigset_t mask;
    // Intercept and ignore SIGALRM
    sa.sa_handler = &alarm_handler;
    // Remove the handler after first signal
    sa.sa_flags = SA_RESETHAND;
    sigfillset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
    // Get the current signal mask
    sigprocmask(0, NULL, &mask);
    // Unblock SIGALRM
    sigdelset(&mask, SIGALRM);

    // Wait with this mask
    // 10000 Microseconds
    ualarm(globals.config.nack_timer, 0);
    sigsuspend(&mask);
}
