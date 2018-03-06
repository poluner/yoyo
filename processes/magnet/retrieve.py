# -*- coding:utf-8 -*-
"""更新es"""
import os
import datetime
import traceback
import subprocess

import torrent_parser as tp
from elasticsearch import Elasticsearch
from sqlalchemy import (
    event,
    Column,
    String,
    Integer,
    DateTime,
    SmallInteger,
)
from sqlalchemy.orm import sessionmaker
from sqlalchemy.exc import DisconnectionError
from sqlalchemy.engine import create_engine
from sqlalchemy.ext.declarative import declarative_base

es_hosts = ["172.31.23.5:9200", "172.31.23.5:9201", "172.31.10.234:9200"]
db_url = "mysql+pymysql://watchnow:watchnow2018@172.31.21.32:3306/yoyo?charset=utf8mb4"


def checkout_listener(dbapi_con, con_record, con_proxy):
    try:
        try:
            dbapi_con.ping(False)
        except TypeError:
            dbapi_con.ping()
    except dbapi_con.OperationalError as exc:
        if exc.args[0] in (2006, 2013, 2014, 2045, 2055):
            raise DisconnectionError()
        else:
            raise

BaseModel = declarative_base()
faceless_engine = create_engine(
    db_url,
    pool_size=100,
    pool_recycle=3600)
event.listen(faceless_engine, 'checkout', checkout_listener)
DBSession = sessionmaker(bind=faceless_engine, expire_on_commit=False)


es_client = Elasticsearch(es_hosts)


class Infohash(BaseModel):

    __tablename__ = 'infohash_task'

    id = Column(Integer, primary_key=True)
    infohash = Column(String(40), nullable=False)
    status = Column(SmallInteger, nullable=False)
    created_at = Column(DateTime, nullable=False, default=datetime.datetime.now)
    updated_at = Column(DateTime, nullable=False, default=datetime.datetime.now,
                        onupdate=datetime.datetime.now)

    @classmethod
    def update_status(cls, infohash, status):
        session = DBSession()
        target = session.query(cls).filter(cls.infohash == infohash)
        target.update({"status": status})
        session.commit()

    @classmethod
    def ready_records(cls, size):
        session = DBSession()
        records = session.query(cls).filter(cls.status == 0). \
            order_by(cls.updated_at.desc()).limit(size).all()
        session.commit()
        return records


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


if __name__ == '__main__':
    file_path = os.path.abspath(__file__)
    current_dir = os.path.dirname(file_path)
    get_torrent_path = os.path.join(current_dir, 'gettorrent')
    while True:
        records = Infohash.ready_records(100)
        if not records:
            break

        for record in records:
            try:
                out = subprocess.check_output("{} {}".format(get_torrent_path, record.infohash), shell=True)
                print(record.infohash, out)
                if out == b'yes':
                    meta_info = retrieve_meta_info(record.infohash)
                    es_client.index('torrent', 'doc', meta_info, id=record.infohash)
                    Infohash.update_status(record.infohash, 1)
                else:
                    Infohash.update_status(record.infohash, 2)
            except Exception:
                traceback.print_exc()
