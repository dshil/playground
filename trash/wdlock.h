#ifndef WDLOCK_H
#define WDLOCK_H

#include "mutex.h"

struct wdlock_ctx;

/*
 * The stupid wait-die lock implementation based on the following resources:
 * https://elixir.bootlin.com/linux/latest/source/include/linux/ww_mutex.h
 * https://lwn.net/Articles/548909/
 *
 */
struct wdlock {
	struct wdlock *next;

	const struct wdlock_ctx *owner;

	struct lock lock;
	struct condition cv;
};

struct wdlock_ctx {
	unsigned long long timestamp;
	struct wdlock *locks;
};

void wdlock_ctx_init(struct wdlock_ctx *ctx);
void wdlock_init(struct wdlock *lock);
int wdlock_lock(struct wdlock *l, struct wdlock_ctx *ctx);
void wdlock_unlock(struct wdlock_ctx *ctx);

#endif // WDLOCK_H
