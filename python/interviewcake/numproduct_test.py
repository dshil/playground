import timeit
import unittest

class Helper(unittest.TestCase):
    def get_products(self, data):
        res = []

        for i, n in enumerate(data):
            p = 1
            for j, v in enumerate(data):
                if j == i:
                    continue
                p *= v
            res.append(p)

        return res

    def test_get_products_of_all_ints(self):
        self.assertEqual([84, 12, 28, 21], self.get_products([1, 7, 3, 4]))

unittest.main()
