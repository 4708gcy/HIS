"""数据库连接池"""
import pymysql
from pymysql.cursors import DictCursor
from contextlib import contextmanager
from core.config import settings


class DatabasePool:
    """pymysql 连接池，延迟初始化"""

    def __init__(self):
        db = settings.database
        self.config = {
            "host": db.get("host", "127.0.0.1"),
            "port": db.get("port", 3307),
            "user": db.get("user", "root"),
            "password": db.get("password", "123456"),
            "database": db.get("database", "his_db"),
            "charset": db.get("charset", "utf8mb4"),
            "cursorclass": DictCursor,
            "autocommit": True,
        }
        self._pool = []
        self._max_size = db.get("pool_size", 5)

    def _create_conn(self):
        return pymysql.connect(**self.config)

    def _get_conn(self):
        while self._pool:
            conn = self._pool.pop()
            try:
                conn.ping(reconnect=True)
                return conn
            except pymysql.Error:
                continue
        return self._create_conn()

    @contextmanager
    def get_cursor(self):
        conn = None
        try:
            conn = self._get_conn()
            yield conn.cursor()
        finally:
            if conn:
                self._pool.append(conn)

    def execute(self, sql: str, params=None):
        with self.get_cursor() as cursor:
            cursor.execute(sql, params or ())
            return cursor.fetchall()

    def execute_one(self, sql: str, params=None):
        with self.get_cursor() as cursor:
            cursor.execute(sql, params or ())
            return cursor.fetchone()


db_pool = DatabasePool()
