# -*- coding: utf8 -*-
"""黑名单"""
import asyncio
from yoyo.utils import (
    current_timestamp,
    gen_ip_port_key
)


class Blacklist(object):
    """黑名单"""
    def __init__(self, expire_secs):
        """构造函数

        Args:
            expire_secs (int): 黑名单的过期时间
        """
        self.expire_secs = expire_secs
        self._blacklist = {}

    def add(self, ip, port):
        """黑名单添加一条记录

        Args:
            ip (string): internet address
            port (int): 端口
        """
        key = gen_ip_port_key(ip, port)
        self._blacklist[key] = current_timestamp()

    def delete(self, ip, port):
        """黑名单删除一条记录

        Args:
            ip (string): internet address
            port (int): 端口
        """
        key = gen_ip_port_key(ip, port)
        self._blacklist.pop(key)

    def in_(self, ip, port):
        """判断是否在黑名单中

        Args:
            ip (string): internet address
            port (int): 端口
        """
        if ip in self._blacklist:
            return True

        key = gen_ip_port_key(ip, port)
        if key in self._blacklist:
            insert_time = self._blacklist[key]
            if current_timestamp() - insert_time < self.expire_secs:
                return True
            else:
                self._blacklist.pop(key)
        return False

    async def clear(self):
        """清除过期的黑名单"""
        while True:
            current = current_timestamp()
            expire_keys = []
            for key, val in self._blacklist.items():
                if current - val > self.expire_secs:
                    expire_keys.append(key)

            for item in expire_keys:
                self._blacklist.pop(item)

            print(self._blacklist)
            await asyncio.sleep(5)


if __name__ == '__main__':
    b = Blacklist(10)
    b.add('1.1.1.1', 79)
    b.add('2.2.2.2', 80)
    b.add('3.3.3.3', 8080)

    loop = asyncio.get_event_loop()
    tasks = [
        asyncio.ensure_future(b.clear()),
    ]
    loop.run_until_complete(asyncio.wait(tasks))
    loop.close()
