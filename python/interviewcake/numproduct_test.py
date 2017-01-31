import timeit
import unittest

class Helper(unittest.TestCase):
    def get_products_slow(self, data):
        if len(data) == 0:
            return data

        res = []

        for i, n in enumerate(data):
            p = 1
            for j, v in enumerate(data):
                if j == i:
                    continue
                p *= v
            res.append(p)

        return res

    def get_products_v2(self, data):
        if len(data) == 0:
            return data

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

    def get_products_greedy(self, data):
        if len(data) == 0:
            return data

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

    def test_get_products_of_all_ints(self):
        self.assertEqual([84, 12, 28, 21], self.get_products_slow([1, 7, 3, 4]))
        self.assertEqual([0, 0, 6], self.get_products_slow([2, 3, 0]))
        self.assertEqual([0, 0, 0, 0, 0], self.get_products_slow([101, 2, 0, 0, 5]))
        self.assertEqual([1, 1, 1], self.get_products_slow([1, 1, 1]))
        self.assertEqual([1], self.get_products_slow([3]))
        self.assertEqual([2, 1], self.get_products_slow([1, 2]))
        self.assertEqual([6, 3, 2], self.get_products_slow([1, 2, 3]))

    def test_get_products_v2(self):
        self.assertEqual([84, 12, 28, 21], self.get_products_v2([1, 7, 3, 4]))
        self.assertEqual([0, 0, 6], self.get_products_v2([2, 3, 0]))
        self.assertEqual([0, 0, 0, 0, 0], self.get_products_v2([101, 2, 0, 0, 5]))
        self.assertEqual([1, 1, 1], self.get_products_v2([1, 1, 1]))
        self.assertEqual([2, 10, 20], self.get_products_v2([10, 2, 1]))
        self.assertEqual([1], self.get_products_v2([39]))
        self.assertEqual([2, 1], self.get_products_v2([1, 2]))
        self.assertEqual([6, 3, 2], self.get_products_v2([1, 2, 3]))

    def test_get_products_greedy(self):
        self.assertEqual([540, 270, 90, 108, 60], self.get_products_greedy([1, 2, 6, 5, 9]))
        self.assertEqual([84, 12, 28, 21], self.get_products_greedy([1, 7, 3, 4]))
        self.assertEqual([0, 0, 6], self.get_products_greedy([2, 3, 0]))
        self.assertEqual([0, 0, 0, 0, 0], self.get_products_greedy([101, 2, 0, 0, 5]))
        self.assertEqual([1, 1, 1], self.get_products_greedy([1, 1, 1]))
        self.assertEqual([2, 10, 20], self.get_products_greedy([10, 2, 1]))
        self.assertEqual([1], self.get_products_greedy([10]))
        self.assertEqual([2, 1], self.get_products_greedy([1, 2]))
        self.assertEqual([6, 3, 2], self.get_products_greedy([1, 2, 3]))
        self.assertEqual([], self.get_products_greedy([]))

unittest.main()

