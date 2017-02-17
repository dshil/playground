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
    d1 = [1]
    for i, v in enumerate(data[:len(data) - 1]):
        if i == 0:
            d1.append(v)
        else:
            d1.append(d1[i] * v)

    pr = 1
    i = len(d1) - 1
    while i >= 0:
        if i == (len(d1) - 1):
            pr *= data[i]
            i -= 1
        else:
            d1[i] *= pr
            pr *= data[i]
            i -= 1

    return d1


if __name__=='__main__':
    from timeit import Timer
    t_slow = Timer("get_products_slow([1, 2, 6, 5, 9])", "from __main__ import get_products_slow")
    t_fast = Timer("get_products_v2([1, 2, 6, 5, 9])", "from __main__ import get_products_v2")
    t_greedy = Timer("get_products_greedy([1, 2, 6, 5, 9])", "from __main__ import get_products_greedy")
    print('slow = {0}, fast = {1}, greedy = {2}'.format(
                                                        t_slow.timeit(),
                                                        t_fast.timeit(),
                                                        t_greedy.timeit()))

