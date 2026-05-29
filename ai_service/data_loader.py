"""
HIS AI 微服务 — MySQL 数据加载模块
从 HIS 的 MySQL 数据库直接读取结构化数据，替代已废弃的 CSV 文件解析
"""
import pymysql
from collections import defaultdict


def _get_connection():
    """获取 MySQL 连接（从 DatabaseConfig.txt 读取参数）"""
    import os
    config_path = os.path.join(os.path.dirname(__file__), '..', 'Data', 'DatabaseConfig.txt')
    cfg = {}
    if os.path.exists(config_path):
        with open(config_path, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                if '=' in line:
                    k, v = line.split('=', 1)
                    cfg[k.strip()] = v.strip()
    return pymysql.connect(
        host=cfg.get('host', '127.0.0.1'),
        port=int(cfg.get('port', 3307)),
        user=cfg.get('user', 'root'),
        password=cfg.get('password', '123456'),
        database=cfg.get('database', 'his_db'),
        charset='utf8mb4',
        cursorclass=pymysql.cursors.DictCursor
    )


def _query(sql, params=None):
    """执行查询并返回字典列表"""
    conn = _get_connection()
    try:
        with conn.cursor() as cursor:
            cursor.execute(sql, params)
            return cursor.fetchall()
    finally:
        conn.close()


def load_patients():
    return _query("SELECT * FROM patients WHERE is_deleted = 0")


def load_doctors():
    return _query("SELECT * FROM doctors WHERE is_deleted = 0")


def load_nurses():
    return _query("SELECT * FROM nurses WHERE is_deleted = 0")


def load_pharmacists():
    return _query("SELECT * FROM pharmacists WHERE is_deleted = 0")


def load_hospitalizations():
    return _query("SELECT * FROM hospitalizations WHERE is_deleted = 0")


def load_registrations():
    return _query("SELECT * FROM registrations WHERE is_deleted = 0")


def load_consultations():
    return _query("SELECT * FROM consultations WHERE is_deleted = 0")


def load_examinations():
    return _query("SELECT * FROM examinations WHERE is_deleted = 0")


def load_medicines():
    return _query("SELECT * FROM medicines WHERE is_deleted = 0")


def load_beds():
    return _query("SELECT * FROM bed_info WHERE is_deleted = 0")


def load_medication_records():
    return _query("SELECT * FROM medication_records WHERE is_deleted = 0")


def load_nursing_records():
    return _query("SELECT * FROM nursing_records WHERE is_deleted = 0")


def extract_monthly_stats(hospitalizations=None, registrations=None):
    """按科室和月份聚合统计数据"""
    if hospitalizations is None:
        hospitalizations = load_hospitalizations()
    if registrations is None:
        registrations = load_registrations()

    stats = defaultdict(lambda: defaultdict(lambda: {
        'new_admissions': 0, 'discharges': 0,
        'total_cost_yuan': 0.0, 'registration_count': 0
    }))

    for h in hospitalizations:
        dept = h['department']
        # MySQL DATETIME → str "YYYY-MM-DD HH:MM:SS"
        admit_time = str(h['admit_time']) if h['admit_time'] else None
        if admit_time and admit_time != 'None' and admit_time != '#':
            admit_month = admit_time[:7]
            stats[dept][admit_month]['new_admissions'] += 1
            stats[dept][admit_month]['total_cost_yuan'] += float(h.get('total_cost_cents', 0)) / 100.0

    for r in registrations:
        dept = r['department']
        reg_time = str(r['register_time']) if r['register_time'] else None
        if reg_time and reg_time != 'None' and reg_time != '#':
            month = reg_time[:7]
            stats[dept][month]['registration_count'] += 1

    result = []
    for dept, months in stats.items():
        for month, data in months.items():
            result.append({
                'department': dept,
                'month': month,
                'new_admissions': data['new_admissions'],
                'discharges': data['discharges'],
                'total_cost_yuan': round(data['total_cost_yuan'], 2),
                'registration_count': data['registration_count']
            })
    result.sort(key=lambda x: (x['department'], x['month']))
    return result
