#ifndef OWD_CORE_SIGINT_WAITER_H_
#define OWD_CORE_SIGINT_WAITER_H_

namespace owd {
namespace core {

class SigintWaiter {
public:
    int setup();
    void wait_interrupt();
};

} // namespace core
} // namespace owd

#endif // OWD_CORE_SIGINT_WAITER_H_
