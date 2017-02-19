import unittest

class Helper(unittest.TestCase):
    def setUp(self):
        self._cache = {}

    def get_exchange_number(self, amount_left, index, denominations):
        key = str((amount_left, index))
        if key in self._cache:
            return self._cache[key]

        if len(denominations) == 0:
            raise IndexError("Getting the exchange requires at least one  \
                              denomination")

        if amount_left < 0: return 0
        if amount_left == 0: return 1
        if index == len(denominations): return 0

        curr_coin = denominations[index]

        e_num = 0
        while amount_left >= 0:
            e_num += self.get_exchange_number(amount_left, index+1,
                                              denominations)
            amount_left -= curr_coin

        self._cache[key] = e_num

        return e_num

    def test_get_number_for_denomination(self):
         self.assertEqual(4, self.get_exchange_number(4, 0, [1, 2, 3]))

    def test_get_number_for_denomination_none(self):
         self.assertEqual(0, self.get_exchange_number(4, 0, [5, 6, 7]))

unittest.main()
