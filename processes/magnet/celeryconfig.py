# -*- coding: utf8 -*-
"""celery配置"""
from celery.schedules import crontab

BROKER_URL = 'redis://:0m9d7R1DYydixHnJ@172.31.10.234:6385/1'

CELERY_IMPORTS = (
    'magnet.tasks',
)

CELERY_TASK_SERIALIZER = 'pickle'
CELERY_ACCEPT_CONTENT = ['pickle', 'json', 'msgpack', 'yaml']

CELERYBEAT_SCHEDULE = {
    "begin_tasks": {
        "task": "magnet.tasks.begin_task",
        "schedule": crontab(hour=18, minute=0),
        "args": (),
    }
}