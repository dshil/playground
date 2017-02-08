import unittest

class Helper(unittest.TestCase):
    def get_merging_ranges(self, data):
        data = sorted(data, key=lambda tup: tup[0])
        res = []

        left, right = data[0]
        for i, v in enumerate(data):
            if i == 0:
                continue

            new_left, new_right = v

            if new_left > right:
                res.append((left, right))
                left, right = v
            elif new_right > right:
                right = new_right
                if i == len(data) - 1:
                    res.append((left, right))

        if len(res) == 0:
            res.append((left, right))

        return res

    def test_get_merging_ranges(self):
        self.assertEqual([(0, 1), (3, 8), (9, 12)],
            self.get_merging_ranges([(0, 1), (3, 5), (4, 8), (10, 12), (9, 10)]))

        self.assertEqual([(1, 3)], self.get_merging_ranges([(1, 2), (2, 3)]))
        self.assertEqual([(1, 5)], self.get_merging_ranges([(1, 5), (2, 3)]))

        self.assertEqual([(0, 10)],
            self.get_merging_ranges([(0, 10), (1, 4), (2, 3)]))

        self.assertEqual([(0, 11)],
            self.get_merging_ranges([(0, 10), (1, 4), (2, 3), (1, 11)]))

        self.assertEqual([(1, 3)],
            self.get_merging_ranges([(1, 2), (2, 3), (2, 2)]))

        self.assertEqual([(1, 10)],
            self.get_merging_ranges([(1, 10), (2, 6), (3, 5), (7, 9)]))

        self.assertEqual([(1, 3), (15, 16)],
            self.get_merging_ranges([(1, 2), (15, 16), (2, 3), (2, 2)]))

unittest.main()
