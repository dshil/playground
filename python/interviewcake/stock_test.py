import unittest


class Helper(unittest.TestCase):
    def get_max_profit(self, data):
        if len(data) < 2:
            raise IndexError('Getting some profit requires at least 2 prices')

        min_price = data[0]
        max_profit = 0
        for i, v in enumerate(data):
            if i == 0:
                continue
            profit = data[i] - min_price
            max_profit = max(max_profit, profit)

            min_price = min(min_price, data[i])

        if max_profit == 0:
            # there are two corner cases:
            #  - [4, 4, 4] - the same value for the whole day.
            #  - [4, 3, 2, 1] - the value goes down for the whole day.
            # for these cases return the difference between two last prices.
            return data[len(data) - 1] - data[len(data) - 2]

        return max_profit

    def test_get_max_profit(self):
        self.assertEqual(6, self.get_max_profit([10, 7, 5, 8, 11, 9]))
        self.assertEqual(-2, self.get_max_profit([4, 3, 1]))
        self.assertEqual(0, self.get_max_profit([1, 1, 1]))
        self.assertEqual(4, self.get_max_profit([10, 9, 8, 7, 11]))

unittest.main()
