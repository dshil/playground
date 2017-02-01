import unittest

class Helper(unittest.TestCase):
    def get_highest_product_nlogn(self, data):
        if len(data) < 3:
            raise IndexError('Get highest product of 3 requires at least 3 items')

        data = sorted(data, reverse=True, key=lambda num: abs(num))
        return data[0] * data[1] * data[2]

    def test_get_max_profit_nlogn(self):
        self.assertEqual(6, self.get_highest_product_nlogn([1, 2, 3]))

        self.assertEqual(300,
                        self.get_highest_product_nlogn([-10, -10, 1, 3, 2]))

unittest.main()
