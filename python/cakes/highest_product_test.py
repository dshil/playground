import unittest

class Helper(unittest.TestCase):
    def get_highest_product_nlogn(self, data):
        if len(data) < 3:
            raise IndexError('Get highest product of 3 requires at least 3 items')

        data = sorted(data, reverse=True, key=lambda num: abs(num))
        return data[0] * data[1] * data[2]

    def get_highest_product_greedy(self, data):
        """This algorithm is more easy adopted for different variations of
        this problem:
            - what if we wanted the highest product of 4 items? O(n)
            - what if we wanted the highest product of k items? O(kn)
        """
        if len(data) < 3:
            raise IndexError('Get highest product of 3 requires at least 3 items')

        for i, v in enumerate(data):
            if i > 2:
                min_index = i - 1
                min_num = data[i-1]
                for n, num in enumerate([data[i-1], data[i-2], data[i-3]]):
                    if abs(num) < abs(min_num):
                        min_index = n
                        min_num = num

                if abs(min_num) < abs(v):
                    data[i-3], data[min_index] = data[min_index], data[i-3]
                else:
                    data[i-3], data[i] = data[i], data[i-3]

        l = len(data)
        return data[l-1] * data[l-2] * data[l-3]

    def get_highest_product_greedy_v2(self, data):
        res = 1
        lo = data[0] * data[1]
        hi = data[0] * data[1]

        for i, _ in enumerate(data):
            if i > 1:
                res = max(res, hi * data[i])
                res = max(res, lo * data[i])

                lo = min(lo, data[i] * data[i-1])
                lo = min(lo, data[i] * data[i-2])

                hi = max(hi, data[i] * data[i-1])
                hi = max(hi, data[i] * data[i-2])

        return res

    def test_get_max_profit_nlogn(self):
        self.assertEqual(6, self.get_highest_product_nlogn([1, 2, 3]))
        self.assertEqual(300,
                        self.get_highest_product_nlogn([-10, -10, 1, 3, 2]))

    def test_get_max_profit_greedy(self):
        self.assertEqual(6, self.get_highest_product_greedy([1, 2, 3]))
        self.assertEqual(24, self.get_highest_product_greedy([3, 1, 2, 4]))
        self.assertEqual(64, self.get_highest_product_greedy([4, 1, 4, 4]))
        self.assertEqual(300,
                        self.get_highest_product_greedy([-10, -10, 1, 3, 2]))
        self.assertEqual(5000,
                        self.get_highest_product_greedy([1, 10, -5, 1, -100]))

    def test_get_max_profit_greedy_v2(self):
        self.assertEqual(6, self.get_highest_product_greedy_v2([1, 2, 3]))
        self.assertEqual(24, self.get_highest_product_greedy_v2([3, 1, 2, 4]))
        self.assertEqual(64, self.get_highest_product_greedy_v2([4, 1, 4, 4]))
        self.assertEqual(300,
                        self.get_highest_product_greedy_v2([-10, -10, 1, 3, 2]))
        self.assertEqual(5000,
                        self.get_highest_product_greedy_v2([1, 10, -5, 1, -100]))

unittest.main()
