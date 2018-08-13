#include "atomic.h"

atomic_t atomic_add(atomic_t *dst, int src)
{
	*dst += src;
}
