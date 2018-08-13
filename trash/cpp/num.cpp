#define MAX(x, y, r)                                \
do {                                                \
  int _x = x;                                       \
  int _y = y;                                       \
  int max = _x;                                     \
  if (_y > max)                                     \
      max = _y;                                     \
  r = max;                                          \
} while (0)

int power(int x, unsigned p) {
    int ret = 1;
    for (int n = 0; n < p; n++)
        ret *= x;

    return ret;
}

unsigned gcd(unsigned a, unsigned b)
{
    if (a == 0)
        return b;
    if (b == 0)
        return a;

    return (a > b) ? gcd(b, a % b) : gcd(a, b % a);
}

void rotate_r(int a[], unsigned size, int shift)
{
    if (size == 0)
        return;

    for (int n = 0; n < shift; n++) {
        int last = a[size-1];

        for (int i = size - 1; i > 0; i--)
            a[i] = a[i-1];
        a[0] = last;

        print_array(a, size);
    }
}

void rotate_l(int a[], unsigned size, int shift)
{
    if (size == 0)
        return;

    for (int n = 0; n < shift; n++) {
        int first = a[0];

        for (int i = 0; i < size - 1; ++i)
            a[i] = a[i+1];
        a[size-1] = first;
    }
}

void print_array(int a[], int sz)
{
    for (int n = 0; n < sz; n++)
        printf("%d ", a[n]);
    printf("\n");
}
