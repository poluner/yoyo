# -*- coding: utf-8 -*-
"""异步任务"""
import time
import datetime
import subprocess
import torrent_parser as tp

from . import (
    celery_app,
    get_torrent_path,
    Infohash,
    es_client,
    logger,
)


def retrieve_meta_info(infohash):
    torrent_path = "/tmp/torrent/{}.torrent".format(infohash)
    content = tp.parse_torrent_file(torrent_path)
    meta_info = content.pop('info')
    name = meta_info.pop('name')
    now = datetime.datetime.now()
    collected_at = now.strftime('%Y-%m-%dT%H:%M:%S.000000000Z')
    result = {
        'name': name,
        'name2': name,
        'collected_at': collected_at,
    }
    if 'length' in meta_info:
        length = meta_info.pop('length')
        result['length'] = length
    else:
        files = meta_info.pop('files')
        length = 0
        new_files = []
        for item in files:
            length += item['length']
            new_files.append({
                'length': item['length'],
                'path': item['path']
            })
        result['length'] = length
        result['files'] = new_files
    return result


@celery_app.task
def update_meta_info(infohash):
    meta_info = retrieve_meta_info(infohash)
    es_client.index('torrent', 'doc', meta_info, id=infohash)
    Infohash.update_status(infohash, 1)


@celery_app.task
def download_torrent(infohash):
    out = subprocess.check_output(
        "{} {}".format(get_torrent_path, infohash), shell=True)
    logger.info(infohash, out.decode())
    if out == b'yes':
        update_meta_info.delay(infohash)
    else:
        Infohash.update_status(infohash, 2)


@celery_app.task
def begin_task():
    offset, limit = 0, 100
    while True:
        records = Infohash.ready_records(offset, limit)
        if not records:
            break

        for record in records:
            download_torrent.delay(record.infohash)

        offset += limit
        time.sleep(10)
