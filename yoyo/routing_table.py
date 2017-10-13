# -*- coding: utf8 -*-
"""路由表"""
from collections import OrderedDict

from yoyo.bitmap import Bitmap
from yoyo.utils import current_timestamp


class Node(object):
    """dht node"""
    def __init__(self, id_, addr):
        """构造函数

        Args:
            id_ (str): dht node id
            addr (str): ip和端口
        """
        self.id_ = Bitmap(id_)
        self.addr = addr
        self.last_active = current_timestamp()


class KBucket(object):
    """dht kbucket"""
    def __init__(self, prefix):
        """构造函数

        Args:
            prefix (Bitmap): 前缀
        """
        self.prefix = prefix
        self.nodes = OrderedDict()
        self.candidates = OrderedDict()
        self.last_changed = current_timestamp()
