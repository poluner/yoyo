# -*- coding: utf8 -*-
"""celery配置"""

BROKER_URL = 'amqp://guest:guest@10.33.1.37:5672/comments'

CELERY_IMPORTS = (
    'magnet.tasks',
)

CELERY_TASK_SERIALIZER = 'pickle'
CELERY_ACCEPT_CONTENT = ['pickle', 'json', 'msgpack', 'yaml']
