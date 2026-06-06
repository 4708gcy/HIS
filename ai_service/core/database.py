"""
数据库连接池
参考来源：mock_login.py 的 pymysql + 04-rag 的 SQLAlchemy 思路
"""
import pymysql
from pymysql.cursors import DictCursor
from contextlib import contextmanager
from core.config import settings


class DatabasePool:
    """MySQL 连接池（简化版）"""

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
        for _ in range(self._max_size):
            self._pool.append(self._create_conn())

    def _create_conn(self):
        return pymysql.connect(**self.config)

    @contextmanager
    def get_cursor(self):
        conn = None
        try:
            conn = self._pool.pop() if self._pool else self._create_conn()
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
