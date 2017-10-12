# -*- coding: utf8 -*-
"""
dht bitmap
"""
from yoyo.error import DHTError


class Bitmap(object):
    """key space"""
    def __init__(self, s):
        """构造函数

        Args:
            s (str): 位图值
        """
        self.value = ''.join(['{:0>8b}'.format(ord(c)) for c in s])

    def __str__(self):
        """打印为字符串"""
        start, step = 0, 8
        result = []
        while True:
            seg = self.value[start: start + step]
            if not seg:
                break
            result.append(chr(int(seg, 2)))
            start += step
        return ''.join(result)

    def compare(self, other, prefix_len):
        """和other比较
        如果 self[:prefix_len] == other[:prefix_len] 返回0
        如果 self[:prefix_len] < other[:prefix_len] 返回-1
        如果 self[:prefix_len] > other[:prefix_len] 返回1

        Args:
            other (Bitmap): 比较对象
            prefix_len (int): 比较的长度
        """
        if prefix_len <= 0 or prefix_len > len(self.value) \
                or prefix_len > len(other.value):
            raise DHTError("invalid prefix length")

        self_prefix = int(self.value[:prefix_len], 2)
        other_prefix = int(other.value[:prefix_len], 2)
        if self_prefix < other_prefix:
            return -1
        elif self_prefix > other_prefix:
            return 1
        else:
            return 0

    def xor(self, other):
        """和other进行异或操作

        Args:
            other (Bitmap): 比较对象
        """
        if len(self.value) != len(other.value):
            raise DHTError("value not the same length")

        distance = ['0'] * len(self.value)
        for i, a in enumerate(self.value):
            b = other.value[i]
            if a != b:
                distance[i] = '1'

        result = object.__new__(self.__class__)
        result.value = ''.join(distance)
        return result
