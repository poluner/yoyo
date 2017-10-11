# -*- coding: utf8 -*-
"""异常"""


class DHTError(Exception):
    """错误基类"""
    def __init__(self, *args, **kwargs):
        super(DHTError, self).__init__(*args, **kwargs)


class EncodeError(DHTError):
    """编码错误"""


class DecodeError(DHTError):
    """解码错误"""
