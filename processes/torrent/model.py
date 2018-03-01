# -*- coding:utf-8 -*-
"""更新es"""
import datetime

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
    hot = Column(Integer, nullable=False)
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

    @classmethod
    def finish_records(cls, size):
        session = DBSession()
        records = session.query(cls).filter(cls.status == 3). \
            order_by(cls.updated_at.desc()).limit(size).all()
        session.commit()
        return records
