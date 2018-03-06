# -*- coding: utf8 -*-
"""celery配置"""

BROKER_URL = 'redis://:0m9d7R1DYydixHnJ@172.31.10.234:6385/1'

CELERY_IMPORTS = (
    'magnet.tasks',
)

CELERY_TASK_SERIALIZER = 'pickle'
CELERY_ACCEPT_CONTENT = ['pickle', 'json', 'msgpack', 'yaml']
