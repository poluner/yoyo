# -*- coding:utf8 -*-
from functools import partial
from yoyo.error import (
    EncodeError,
    DecodeError,
)


def _encode_string(data):
    """benode编码字符串

    Args:
        data (str): 编码数据
    """
    return "{}:{}".format(len(data), data)


def _encode_int(data):
    """benode编码整数

    Args:
        data (int): 编码数据
    """
    return "i{}e".format(data)


def _encode_list(data):
    """benode编码列表

    Args:
        data (list): 编码列表
    """
    result = []
    for item in data:
        result.append(encode(item))
    return "l{}e".format("".join(result))


def _encode_dict(data):
    """benode编码列表

    Args:
        data (dict): 编码字典
    """
    result = []
    for key, val in data.items():
        result.append("{}{}".format(
            _encode_string(key),
            encode(val)))
    return "d{}e".format("".join(result))


def encode(data):
    """becode编码

    Args:
        data: 编码数据
    Returns:
        (str): 编码结果
    """
    if isinstance(data, str):
        return _encode_string(data)
    elif isinstance(data, int):
        return _encode_int(data)
    elif isinstance(data, list):
        return _encode_list(data)
    elif isinstance(data, dict):
        return _encode_dict(data)
    else:
        raise EncodeError("unknown type")


def _decode_string(data, start):
    """becode解码字符串

    Args:
        data (str): 要解码的字符串
        start (int): 解码位置
    Returns:
        (str): 解码的字符串
        (int): 下次解码位置
    """
    if start >= len(data) or not data[start].isdigit():
        return None, -1

    colon = data.find(':', start)
    if colon == -1:
        return None, -1

    s_length = data[start: colon]
    if not s_length.isdigit():
        return None, -1

    length = int(s_length)
    if length < 0:
        return None, -1

    index = colon + 1 + length
    if index > len(data):
        return None, -1

    return data[colon + 1: index], index


def _decode_int(data, start):
    """becode解码整数

    Args:
        data (str): 要解码的字符串
        start (int): 解码位置
    Returns:
        (int): 解码的整数
        (int): 下次解码位置
    """
    if start >= len(data) or data[start] != 'i':
        return None, -1

    e = data.find('e', start)
    if e == -1:
        return None, -1

    s_value = data[start + 1: e]
    try:
        value = int(s_value)
    except ValueError:
        return None, -1
    else:
        return value, e + 1


def _decode_list(data, start):
    """becode解码列表

    Args:
        data (str): 要解码的字符串
        start (int): 解码位置
    Returns:
        (list): 解码的列表
        (int): 下次解码位置
    """
    if start >= len(data) or data[start] != 'l':
        return None, -1

    index = start + 1
    result = []
    while index < len(data):
        if data[index] == 'e':
            break

        item, index = _decode_item(data, index)
        result.append(item)

    if index >= len(data):
        return None, -1

    return result, index + 1


def _decode_dict(data, start):
    """becode解码字典

    Args:
        data (str): 要解码的字符串
        start (int): 解码位置
    Returns:
        (dict): 解码的字典
        (int): 下次解码位置
    """
    if start >= len(data) or data[start] != 'd':
        return None, -1

    index = start + 1
    result = {}
    while index < len(data):
        if data[index] == 'e':
            break

        key, index = _decode_string(data, index)
        if key is None:
            return None, -1

        value, index = _decode_item(data, index)
        result[key] = value

    if index >= len(data):
        return None, -1

    return result, index + 1


def _decode_item(data, start):
    """becode解码字典

    Args:
        data (str): 要解码的字符串
        start (int): 解码位置
    Returns:
        解码的数据
        (int): 下次解码位置
    """
    for func in (_decode_int, _decode_string, _decode_list, _decode_dict):
        result, index = func(data, start)
        if result is not None:
            return result, index

    raise DecodeError("invalid bencode")

decode = partial(_decode_item, start=0)
