"""从 HIS MySQL 加载各维度数据"""
from core.database import db_pool


class DataLoader:

    @staticmethod
    def load_monthly_stats(months=6, department=None):
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
    def load_bed_info(department=None):
        sql = "SELECT * FROM bed_info WHERE 1=1"
        params = []
        if department:
            sql += " AND department = %s"
            params.append(department)
        return db_pool.execute(sql, tuple(params))

    @staticmethod
    def load_hospitalizations(department=None):
        sql = "SELECT * FROM hospitalizations WHERE discharge_time IS NOT NULL"
        params = []
        if department:
            sql += " AND department = %s"
            params.append(department)
        return db_pool.execute(sql, tuple(params))

    @staticmethod
    def load_medicine_inventory():
        return db_pool.execute("SELECT * FROM medicines")

    @staticmethod
    def load_doctor_workload():
        return db_pool.execute("""
            SELECT department,
                   COUNT(*) as doctor_count,
                   SUM(consultation_count) as total_consultations
            FROM doctors
            GROUP BY department
        """)

    @staticmethod
    def load_patient_visits():
        return db_pool.execute("""
            SELECT department,
                   COUNT(*) as patient_count,
                   SUM(registrationCount) as total_registrations,
                   SUM(consultationCount) as total_consultations
            FROM patients
            GROUP BY department
        """)
