#include "mutex.h"

void lock_init(struct lock *lock)
{
}

void lock(struct lock *lock)
{
}

void unlock(struct lock *lock)
{
}

void condition_init(struct condition *cv)
{
}

void wait(struct condition *cv, struct lock *lock)
{
}

void notify_one(struct condition *cv)
{
}

void notify_all(struct condition *cv)
{
}
