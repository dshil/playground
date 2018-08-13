#include "wdlock.h"

static int
try_lock(struct wdlock locks[], const int locks_num, struct wdlock_ctx *ctx);

int main(int argc, char *argv[])
{
	struct wdlock_ctx ctx;
	struct wdlock lockA;
	struct wdlock lockB;

	wdlock_ctx_init(&ctx);
	wdlock_init(&lockA);
	wdlock_init(&lockB);

	struct wdlock locks[] = {lockA, lockB};

	for (;;) {
		if (!try_lock(locks, sizeof(locks)/sizeof(locks[0]), &ctx)) {
			wdlock_unlock(&ctx);
			continue;
		}
		break;
	}

	wdlock_unlock(&ctx);
}

static int
try_lock(struct wdlock locks[], const int locks_num, struct wdlock_ctx *ctx)
{
	for (int n = 0; n < locks_num; ++n) {
		if (!wdlock_lock(&locks[n], ctx)) {
			return 0;
		}
	}
}
