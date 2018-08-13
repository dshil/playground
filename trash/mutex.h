#ifndef MUTEX_H
#define MUTEX_H

struct lock {
};

void lock_init(struct lock *lock);
void lock(struct lock *lock);
void unlock(struct lock *lock);

struct condition {
};

void condition_init(struct condition *cv);
void wait(struct condition *cv, struct lock *lock);
void notify_one(struct condition *cv);
void notify_all(struct condition *cv);

#endif // MUTEX_H
