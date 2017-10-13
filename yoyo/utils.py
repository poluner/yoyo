# -*- coding: utf8 -*-
"""工具"""
import time


def current_timestamp():
    """获取当前时间戳"""
    return int(time.time())


def gen_ip_port_key(ip, port=None):
    """生成ip和端口字符串"""
    if port:
        return "{}:{}".format(ip, port)
    return ip
