#include <signal.h>
#include <string.h>
#include <errno.h>

#include <mutex>
#include <condition_variable>

#include "core/sigint_waiter.h"

namespace owd {
namespace core {

namespace {

static void handle_signal(int);

static std::condition_variable cond;
static std::mutex mu;

static void handle_signal(int signum) {
    if (signum != SIGINT)
        return;

    cond.notify_one();
}

} // namespace

int SigintWaiter::setup() {
    struct sigaction action;
    memset(&action, 0, sizeof(action));

    action.sa_handler = handle_signal;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    return 0;
}

void SigintWaiter::wait_interrupt() {
    std::unique_lock<std::mutex> lock(mu);
    cond.wait(lock);
}

} // namespace core
} // namespace owd
