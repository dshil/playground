def get_products_slow(data):
    res = []

    for i, n in enumerate(data):
        p = 1
        for j, v in enumerate(data):
            if j == i:
                continue
            p *= v
        res.append(p)

    return res

def get_products_v2(data):
    pr = 1
    zr_cnt = 0
    zr_index = 0
    res = []
    for i, v in enumerate(data):
        if v == 0:
            zr_index = i
            zr_cnt += 1
            if zr_cnt > 1:
                break
        else:
            pr *= v

    if zr_cnt > 1:
        for i,v in enumerate(data):
            res.append(0)
    elif zr_cnt == 1:
        for i, v in enumerate(data):
            if i == zr_index:
                res.append(pr)
            else:
                res.append(0)
    else:
        for i, v in enumerate(data):
            res.append(pr / v)

    return res

def get_products_greedy(data):
    d1 = []
    for i, v in enumerate(data[:len(data) - 1]):
        if len(d1) == 0:
            d1.append(v)
        else:
            d1.append(d1[len(d1)-1] * v)

    k = 1
    for i, v in reversed(list(enumerate(data[2:]))):
        k *= v
        d1[i] = d1[i] * k

    if len(data) == 1:
        d1.insert(0, data[0] * k)
    else:
        d1.insert(0, data[1] * k)

    return d1

if __name__=='__main__':
    from timeit import Timer
    t_slow = Timer("get_products_slow([1, 2, 6, 5, 9])", "from __main__ import get_products_slow")
    t_fast = Timer("get_products_v2([1, 2, 6, 5, 9])", "from __main__ import get_products_v2")
    t_greedy = Timer("get_products_greedy([1, 2, 6, 5, 9])", "from __main__ import get_products_greedy")

    print('slow = {0}, fast = {1}, greedy = {2}'.format(t_slow.timeit(),
                                                        t_fast.timeit(),
                                                        t_greedy.timeit()))

