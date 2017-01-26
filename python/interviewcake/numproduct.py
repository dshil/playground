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

if __name__=='__main__':
    from timeit import Timer
    t_slow = Timer("get_products_slow([1, 0, 0, 0, 0, 0, 0, 0, 0])", "from __main__ import get_products_slow")
    t_fast = Timer("get_products_v2([1, 0, 0, 0, 0, 0, 0, 0, 0])", "from __main__ import get_products_v2")
    print('slow = {0}, fast = {1}'.format(t_slow.timeit(), t_fast.timeit()))

