#include "mutex.h"
#include "wdlock.h"
#include "atomic.h"
#include "stdargs.h"

static int _wdlock_lock(struct wdlock *l, struct wdlock_ctx *ctx);
static void wdlock_unlock_all(struct wdlock *l);

void wdlock_ctx_init(struct wdlock_ctx *ctx)
{
	static atomic_t next;

	ctx->timestamp = atomic_add(&next, 1) + 1;
	ctx->locks = NULL;
}

void wdlock_init(struct wdlock *lock)
{
	lock_init(&lock->lock);
	condition_init(&lock->cv);
	lock->owner = NULL;
}

int wdlock_lock(struct wdlock *l, struct wdlock_ctx *ctx)
{
	lock(&l->lock);
	const int ret = _wdlock_lock(l, ctx);
	unlock(&l->lock);

	return ret;
}

void wdlock_unlock(struct wdlock_ctx *ctx)
{
	wdlock_unlock_all(ctx->locks);
	ctx->locks = NULL;
}

static int _wdlock_lock(struct wdlock *l, struct wdlock_ctx *ctx)
{
	if (l->owner) {
		if (l->owner->timestamp < ctx->timestamp)
			return 0;

		if (l->owner == ctx)
			return 1;

		while(l->owner)
			wait(&l->cv, &l->lock);
	}

	l->owner = ctx;
	l->next = NULL;

	if (ctx->locks) {
		struct wdlock *it = ctx->locks;
		while(it->next)
			it = it->next;
		it->next = l;
	} else {
		ctx->locks = l;
	}

	return 1;
}

static void wdlock_unlock_all(struct wdlock *l)
{
	if (l == NULL)
		return;

	wdlock_unlock_all(l->next);

	lock(&l->lock);

	notify_all(&l->cv);
	l->next = NULL;
	l->owner = NULL;

	unlock(&l->lock);
}
