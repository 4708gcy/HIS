"""
数据加载服务
从 HIS MySQL 数据库加载各维度数据
"""
from typing import List, Dict, Any
from core.database import db_pool


class DataLoader:
    """HIS 数据加载器"""

    @staticmethod
    def load_monthly_stats(months: int = 6, department: str = None) -> List[Dict[str, Any]]:
        """加载月度统计数据（科室入院人数）"""
        sql = """
            SELECT
                department,
                DATE_FORMAT(admit_time, '%%Y-%%m') as month,
                COUNT(*) as new_admissions
            FROM hospitalizations
            WHERE admit_time >= DATE_SUB(CURDATE(), INTERVAL %s MONTH)
            GROUP BY department, month
            ORDER BY department, month
        """
        params = [months]
        if department:
            sql = sql.replace("GROUP BY", f"AND department = %s GROUP BY")
            params = [months, department]
        return db_pool.execute(sql, tuple(params))

    @staticmethod
    def load_bed_info(department: str = None) -> List[Dict[str, Any]]:
        """加载床位信息"""
        sql = "SELECT * FROM bed_info WHERE 1=1"
        params = []
        if department:
            sql += " AND department = %s"
            params.append(department)
        return db_pool.execute(sql, tuple(params))

    @staticmethod
    def load_hospitalizations(department: str = None) -> List[Dict[str, Any]]:
        """加载住院记录"""
        sql = "SELECT * FROM hospitalizations WHERE discharge_time IS NOT NULL"
        params = []
        if department:
            sql += " AND department = %s"
            params.append(department)
        return db_pool.execute(sql, tuple(params))

    @staticmethod
    def load_medicine_inventory() -> List[Dict[str, Any]]:
        """加载药品库存"""
        return db_pool.execute("SELECT * FROM medicines")

    @staticmethod
    def load_doctor_workload() -> List[Dict[str, Any]]:
        """加载医生工作量"""
        return db_pool.execute("""
            SELECT department,
                   COUNT(*) as doctor_count,
                   SUM(consultation_count) as total_consultations
            FROM doctors
            GROUP BY department
        """)

    @staticmethod
    def load_patient_visits() -> List[Dict[str, Any]]:
        """加载患者就诊统计"""
        return db_pool.execute("""
            SELECT department,
                   COUNT(*) as patient_count,
                   SUM(registrationCount) as total_registrations,
                   SUM(consultationCount) as total_consultations
            FROM patients
            GROUP BY department
        """)
