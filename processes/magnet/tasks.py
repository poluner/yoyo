# -*- coding: utf-8 -*-
"""异步任务"""
import subprocess
import torrent_parser as tp

from . import (
    celery_app,
    get_torrent_path,
    InfohashTask,
    AnnouncePeer,
    logger,
)


def retrieve_meta_info(infohash):
    torrent_path = "/tmp/torrent/{}.torrent".format(infohash)
    content = tp.parse_torrent_file(torrent_path)
    meta_info = content.pop('info')
    name = meta_info.pop('name')
    result = {
        'name': name,
    }
    if 'length' in meta_info:
        length = meta_info.pop('length')
        result['length'] = length
    else:
        files = meta_info.pop('files')
        new_files = []
        for item in files:
            new_files.append({
                'length': item['length'],
                'path': item['path']
            })
        result['files'] = new_files
    return result


@celery_app.task
def update_meta_info(infohash):
    meta_info = retrieve_meta_info(infohash)
    hot = AnnouncePeer.get_hot(infohash)
    # todo 更新热度和下载个数
    InfohashTask.update_status(infohash, 1)


@celery_app.task
def download_torrent(infohash):
    out = subprocess.check_output(
        "{} {}".format(get_torrent_path, infohash), shell=True)
    logger.info('{} {}'.format(infohash, out.decode()))
    if out == b'yes':
        update_meta_info.delay(infohash)
    else:
        InfohashTask.update_status(infohash, 2)


@celery_app.task
def begin_task(offset):
    limit = 100
    records = InfohashTask.ready_records(offset, limit)
    if not records:
        begin_task.apply_async((0,), countdown=86400)

    for record in records:
        download_torrent.delay(record.infohash)

    offset += limit
    if offset > 200000:
        offset = 0
    begin_task.apply_async((offset,), countdown=10)
