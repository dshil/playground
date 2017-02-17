import unittest

class Helper(unittest.TestCase):
    def get_denominations(self, den):
        if den <= 0:
            raise IndexError("Getting the denominations requires the value to \
                              be at least 1")
        table = []

        for i in range(den+1):
            if i == 0:
                table.append(None)
                continue

            if i == 1:
                table.append([[1]])
                continue

            if i == 2:
                table.append([[1, 1]])
                continue

            lo = 1
            hi = i - 1

            cell = []
            while lo <= hi:
                    cell.append([lo, hi])
                    lo += 1
                    hi -= 1

            for v in table[i-1]:
                copy_v = list(v)
                copy_v.append(1)
                cell.append(copy_v)

            table.append(cell)

        res = table[len(table)-1]
        if len(res) == 1:
            return res[0]

        return res

    def test_get_merging_ranges(self):
        self.assertEqual([1], self.get_denominations(1))
        self.assertEqual([1, 1], self.get_denominations(2))
        self.assertEqual([[1, 2], [1, 1, 1]], self.get_denominations(3))

        self.assertEqual([[1, 3], [2, 2], [1, 2, 1], [1, 1, 1, 1]],
            self.get_denominations(4))

        self.assertEqual([[1, 4], [2, 3], [1, 3, 1], [2, 2, 1], [1, 2, 1, 1],
                          [1, 1, 1, 1, 1]], self.get_denominations(5))

        self.assertEqual([[1, 5], [2, 4], [3, 3], [1, 4, 1], [2, 3, 1],
                          [1, 3, 1, 1], [2, 2, 1, 1], [1, 2, 1, 1, 1],
                          [1, 1, 1, 1, 1, 1]],
                          self.get_denominations(6))

unittest.main()
