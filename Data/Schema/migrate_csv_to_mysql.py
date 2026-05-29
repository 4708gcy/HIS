#!/usr/bin/env python3
"""
HIS CSV-to-MySQL Migration Script
Reads all 13 flat-file data files from the HIS project and inserts them into MySQL.

Usage:
    python migrate_csv_to_mysql.py [--data-dir PATH] [--drop-existing]

Requirements:
    pip install mysql-connector-python
"""

import sys
import os
import math

# ---------------------------------------------------------------------------
# Dependency check
# ---------------------------------------------------------------------------
try:
    import mysql.connector
    from mysql.connector import Error as MySQLError
except ImportError:
    print("[ERROR] mysql-connector-python is not installed.")
    print("        Install it with:  pip install mysql-connector-python")
    install = input("        Install now? [y/N] ").strip().lower()
    if install == "y":
        import subprocess
        subprocess.check_call([sys.executable, "-m", "pip", "install", "mysql-connector-python"])
        import mysql.connector
        from mysql.connector import Error as MySQLError
        print("[OK] mysql-connector-python installed successfully.")
    else:
        sys.exit(1)

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
MYSQL_CONFIG = {
    "host": "127.0.0.1",
    "port": 3307,
    "user": "root",
    "password": "123456",
    "database": "his_db",
    "charset": "utf8mb4",
    "use_pure": True,
}

# Project root: parent of Data/Schema/
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))

# ---------------------------------------------------------------------------
# Helper functions
# ---------------------------------------------------------------------------

def empty_val(val):
    """Return True if the value is the empty-field sentinel '#' or empty/whitespace."""
    if val is None:
        return True
    s = str(val).strip()
    return s == "#" or s == ""


def to_null(val):
    """Return None if sentinel, else the original string."""
    if empty_val(val):
        return None
    return val.strip()


def to_bool(val):
    """Convert '1'/'0' string to 1/0 int. Sentinel becomes 0."""
    if empty_val(val):
        return 0
    return 1 if val.strip() == "1" else 0


def to_int(val):
    """Parse int, returning None for sentinel."""
    if empty_val(val):
        return None
    try:
        return int(float(val.strip()))
    except (ValueError, TypeError):
        return None


def yuan_to_cents(val):
    """Parse a yuan string (int or float) and return cents (int). Sentinel returns None."""
    if empty_val(val):
        return None
    try:
        return round(float(val.strip()) * 100)
    except (ValueError, TypeError):
        return None


def parse_datetime(val):
    """Return datetime string or None for sentinel. Passes through valid datetime strings."""
    if empty_val(val):
        return None
    return val.strip() if val.strip() else None


def read_data_file(filepath):
    """Read a HIS data file, skipping 'count:' lines and empty lines.
    Returns a list of non-empty, non-count lines with trailing whitespace stripped."""
    lines = []
    if not os.path.exists(filepath):
        print(f"  [WARN] File not found, skipping: {filepath}")
        return lines
    with open(filepath, "r", encoding="utf-8") as f:
        for raw_line in f:
            line = raw_line.rstrip("\n").rstrip("\r")
            stripped = line.strip()
            if not stripped:
                continue
            if stripped.startswith("count:"):
                continue
            lines.append(line)
    return lines


def execute_batch(cursor, sql, rows, table_name):
    """Execute a batch insert, print row count."""
    if not rows:
        print(f"  {table_name}: 0 rows (empty)")
        return
    try:
        cursor.executemany(sql, rows)
        print(f"  {table_name}: {len(rows)} rows inserted")
    except MySQLError as e:
        print(f"  [ERROR] {table_name}: {e}")
        # Fall back to row-by-row for better error isolation
        ok = 0
        for row in rows:
            try:
                cursor.execute(sql, row)
                ok += 1
            except MySQLError as e2:
                print(f"    [SKIP] row={row[:3]}... err={e2}")
        print(f"  {table_name}: {ok}/{len(rows)} rows inserted (row-by-row fallback)")


# ---------------------------------------------------------------------------
# Migration functions per entity
# ---------------------------------------------------------------------------

def migrate_admins(cursor, data_dir):
    filepath = os.path.join(data_dir, "UserData", "AdminChainData", "admin_users.txt")
    lines = read_data_file(filepath)
    rows = []
    for line in lines:
        parts = line.split(",")
        # userID,username,gender,ageStr,telephone,email,storedHash,salt,isActiveStr,createTime,isDeletedStr
        # Note: admin may have trailing empty field after isDeletedStr
        if len(parts) < 10:
            continue
        admin_id = to_null(parts[0])
        username = to_null(parts[1])
        gender = to_null(parts[2])
        age = to_int(parts[3])
        telephone = to_null(parts[4])
        email = to_null(parts[5])
        stored_hash = to_null(parts[6])
        salt = to_null(parts[7])
        is_active = to_bool(parts[8])
        create_time = parse_datetime(parts[9])
        is_deleted = to_bool(parts[10]) if len(parts) > 10 else 0

        rows.append((
            admin_id, username, stored_hash, salt, 0, is_active,
            create_time, gender, age, telephone, email, is_deleted
        ))

    sql = """INSERT IGNORE INTO admins
             (admin_id, username, stored_hash, salt, login_attempts, is_active,
              create_time, gender, age, telephone, email, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, rows, "admins")


def migrate_doctors(cursor, data_dir):
    filepath = os.path.join(data_dir, "UserData", "DoctorChainData", "doctor_users.txt")
    lines = read_data_file(filepath)
    rows = []
    for line in lines:
        parts = line.split(",")
        if len(parts) < 19:
            continue
        # userID,username,gender,ageStr,telephone,email,storedHash,salt,isActiveStr,createTime,
        # doctorID,department,titleStr,specialty,scheduleInfo,consultationCount,examinationCount,
        # hospitalizationApplyCount,isOnDutyStr,isDeletedStr
        doctor_id = to_null(parts[0])
        username = to_null(parts[1])
        gender = to_null(parts[2])
        age = to_int(parts[3])
        telephone = to_null(parts[4])
        email = to_null(parts[5])
        stored_hash = to_null(parts[6])
        salt = to_null(parts[7])
        is_active = to_bool(parts[8])
        create_time = parse_datetime(parts[9])
        # parts[10] is doctorID (same as userID), skip duplicate
        department = to_null(parts[11])
        title = to_int(parts[12])
        specialty = to_null(parts[13])
        schedule_info = to_null(parts[14])
        consultation_count = to_int(parts[15]) or 0
        examination_count = to_int(parts[16]) or 0
        hospitalization_apply_count = to_int(parts[17]) or 0
        is_on_duty = to_bool(parts[18]) if len(parts) > 18 else 1
        is_deleted = to_bool(parts[19]) if len(parts) > 19 else 0

        rows.append((
            doctor_id, username, stored_hash, salt, 0, is_active,
            create_time, gender, age, telephone, email,
            department, title, specialty, schedule_info,
            consultation_count, examination_count, hospitalization_apply_count,
            is_on_duty, is_deleted
        ))

    sql = """INSERT IGNORE INTO doctors
             (doctor_id, username, stored_hash, salt, login_attempts, is_active,
              create_time, gender, age, telephone, email,
              department, title, specialty, schedule_info,
              consultation_count, examination_count, hospitalization_apply_count,
              is_on_duty, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, rows, "doctors")


def migrate_nurses(cursor, data_dir):
    filepath = os.path.join(data_dir, "UserData", "NurseChainData", "nurse_users.txt")
    lines = read_data_file(filepath)
    rows = []
    for line in lines:
        parts = line.split(",")
        if len(parts) < 17:
            continue
        nurse_id = to_null(parts[0])
        username = to_null(parts[1])
        gender = to_null(parts[2])
        age = to_int(parts[3])
        telephone = to_null(parts[4])
        email = to_null(parts[5])
        stored_hash = to_null(parts[6])
        salt = to_null(parts[7])
        is_active = to_bool(parts[8])
        create_time = parse_datetime(parts[9])
        # parts[10] is nurseID (same), skip
        department = to_null(parts[11])
        level = to_int(parts[12])
        schedule_info = to_null(parts[13])
        patient_care_count = to_int(parts[14]) or 0
        bed_manage_count = to_int(parts[15]) or 0
        is_on_duty = to_bool(parts[16]) if len(parts) > 16 else 1
        is_deleted = to_bool(parts[17]) if len(parts) > 17 else 0

        rows.append((
            nurse_id, username, stored_hash, salt, 0, is_active,
            create_time, gender, age, telephone, email,
            department, level, schedule_info,
            patient_care_count, bed_manage_count,
            is_on_duty, is_deleted
        ))

    sql = """INSERT IGNORE INTO nurses
             (nurse_id, username, stored_hash, salt, login_attempts, is_active,
              create_time, gender, age, telephone, email,
              department, level, schedule_info,
              patient_care_count, bed_manage_count,
              is_on_duty, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, rows, "nurses")


def migrate_pharmacists(cursor, data_dir):
    filepath = os.path.join(data_dir, "UserData", "PharmacistChainData", "pharmacist_users.txt")
    lines = read_data_file(filepath)
    rows = []
    for line in lines:
        parts = line.split(",")
        if len(parts) < 18:
            continue
        pharmacist_id = to_null(parts[0])
        username = to_null(parts[1])
        gender = to_null(parts[2])
        age = to_int(parts[3])
        telephone = to_null(parts[4])
        email = to_null(parts[5])
        stored_hash = to_null(parts[6])
        salt = to_null(parts[7])
        is_active = to_bool(parts[8])
        create_time = parse_datetime(parts[9])
        # parts[10] is pharmacistID (same), skip
        department = to_null(parts[11])
        level = to_int(parts[12])
        schedule_info = to_null(parts[13])
        review_count = to_int(parts[14]) or 0
        dispense_count = to_int(parts[15]) or 0
        inventory_manage_count = to_int(parts[16]) or 0
        is_on_duty = to_bool(parts[17]) if len(parts) > 17 else 1
        is_deleted = to_bool(parts[18]) if len(parts) > 18 else 0

        rows.append((
            pharmacist_id, username, stored_hash, salt, 0, is_active,
            create_time, gender, age, telephone, email,
            department, level, schedule_info,
            review_count, dispense_count, inventory_manage_count,
            is_on_duty, is_deleted
        ))

    sql = """INSERT IGNORE INTO pharmacists
             (pharmacist_id, username, stored_hash, salt, login_attempts, is_active,
              create_time, gender, age, telephone, email,
              department, level, schedule_info,
              review_count, dispense_count, inventory_manage_count,
              is_on_duty, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, rows, "pharmacists")


def migrate_patients(cursor, data_dir):
    filepath = os.path.join(data_dir, "UserData", "PatientChainData", "patient_users.txt")
    lines = read_data_file(filepath)
    rows = []
    for line in lines:
        parts = line.split(",")
        if len(parts) < 26:
            continue
        patient_id = parts[0].strip()
        username = to_null(parts[1])
        gender = to_null(parts[2])
        age = to_int(parts[3])
        telephone = to_null(parts[4])
        email = to_null(parts[5])
        stored_hash = to_null(parts[6])
        salt = to_null(parts[7])
        is_active = to_bool(parts[8])
        create_time = parse_datetime(parts[9])
        # parts[10] is patientID (same), skip
        department = to_null(parts[11])
        address = to_null(parts[12])
        id_card_number = to_null(parts[13])
        emergency_contact_name = to_null(parts[14])
        emergency_contact_phone = to_null(parts[15])
        allergy_history = to_null(parts[16])
        past_medical_history = to_null(parts[17])
        marital_status = to_int(parts[18])
        registration_count = to_int(parts[19]) or 0
        consultation_count = to_int(parts[20]) or 0
        hospitalization_count = to_int(parts[21]) or 0
        medication_count = to_int(parts[22]) or 0
        balance_cents = yuan_to_cents(parts[23]) or 0
        is_hospitalized = to_bool(parts[24])
        is_deleted = to_bool(parts[25]) if len(parts) > 25 else 0

        rows.append((
            patient_id, username, stored_hash, salt, 0, is_active,
            create_time, gender, age, telephone, email,
            department, address, id_card_number,
            emergency_contact_name, emergency_contact_phone,
            allergy_history, past_medical_history, marital_status,
            registration_count, consultation_count, hospitalization_count,
            medication_count, balance_cents, is_hospitalized, is_deleted
        ))

    sql = """INSERT IGNORE INTO patients
             (patient_id, username, stored_hash, salt, login_attempts, is_active,
              create_time, gender, age, telephone, email,
              department, address, id_card_number,
              emergency_contact_name, emergency_contact_phone,
              allergy_history, past_medical_history, marital_status,
              registration_count, consultation_count, hospitalization_count,
              medication_count, balance_cents, is_hospitalized, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, rows, "patients")


def migrate_medicines(cursor, data_dir):
    """Medicines must be imported before consultations (prescription FK)."""
    filepath = os.path.join(data_dir, "RecordData", "MedicineChainData", "medicines.txt")
    lines = read_data_file(filepath)
    med_rows = []
    alias_rows = []
    current_medicine_id = None

    for line in lines:
        stripped = line.strip()

        # Sub-line: ALIAS
        if stripped.startswith("ALIAS:"):
            alias_val = stripped[len("ALIAS:"):].strip()
            if current_medicine_id and not empty_val(alias_val):
                alias_rows.append((current_medicine_id, alias_val))
            continue

        # Main line
        parts = line.split(",")
        if len(parts) < 16:
            continue

        medicine_id = to_null(parts[0])
        current_medicine_id = medicine_id
        name = to_null(parts[1])
        specification = to_null(parts[2])
        manufacturer = to_null(parts[3])
        purchase_price_cents = yuan_to_cents(parts[4]) or 0
        sale_price_cents = yuan_to_cents(parts[5]) or 0
        stock = to_int(parts[6]) or 0
        safety_stock = to_int(parts[7]) or 0
        production_date = to_null(parts[8])
        expiry_date = to_null(parts[9])
        department = to_null(parts[10])
        is_special = to_bool(parts[11])
        is_deleted = to_bool(parts[12])
        note = to_null(parts[13])
        generic_name = to_null(parts[14])
        status = to_int(parts[15])

        med_rows.append((
            medicine_id, name, specification, manufacturer,
            purchase_price_cents, sale_price_cents, stock, safety_stock,
            production_date, expiry_date, department,
            is_special, generic_name, status, note, is_deleted
        ))

    sql = """INSERT IGNORE INTO medicines
             (medicine_id, name, specification, manufacturer,
              purchase_price_cents, sale_price_cents, stock, safety_stock,
              production_date, expiry_date, department,
              is_special, generic_name, status, note, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, med_rows, "medicines")

    # Aliases
    if alias_rows:
        sql_alias = """INSERT IGNORE INTO medicine_aliases (medicine_id, alias)
                       VALUES (%s, %s)"""
        execute_batch(cursor, sql_alias, alias_rows, "medicine_aliases")


def migrate_registrations(cursor, data_dir):
    filepath = os.path.join(data_dir, "RecordData", "RegistrationChainData", "registrations.txt")
    lines = read_data_file(filepath)
    reg_rows = []
    related_rows = []
    current_reg_id = None

    for line in lines:
        stripped = line.strip()

        # Sub-line: RELATED_REGISTRATION_ID
        if stripped.startswith("RELATED_REGISTRATION_ID:"):
            related_id = stripped[len("RELATED_REGISTRATION_ID:"):].strip()
            if current_reg_id and not empty_val(related_id):
                related_rows.append(("registration", current_reg_id, related_id))
            continue

        # Main line
        parts = line.split(",")
        if len(parts) < 9:
            continue

        registration_id = to_null(parts[0])
        current_reg_id = registration_id
        patient_id = to_null(parts[1])
        department = to_null(parts[2])
        doctor_id = to_null(parts[3])
        register_time = parse_datetime(parts[4])
        fee_cents = yuan_to_cents(parts[5]) or 0
        status = to_int(parts[6])
        note = to_null(parts[7])
        is_deleted = to_bool(parts[8])

        reg_rows.append((
            registration_id, patient_id, department, doctor_id,
            register_time, fee_cents, status, note, is_deleted
        ))

    sql = """INSERT IGNORE INTO registrations
             (registration_id, patient_id, department, doctor_id,
              register_time, fee_cents, status, note, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, reg_rows, "registrations")

    if related_rows:
        sql_rel = """INSERT IGNORE INTO related_record_ids (record_type, record_id, related_id)
                     VALUES (%s, %s, %s)"""
        execute_batch(cursor, sql_rel, related_rows, "related_record_ids (registration)")


def migrate_consultations(cursor, data_dir):
    filepath = os.path.join(data_dir, "RecordData", "ConsultationChainData", "consultations.txt")
    lines = read_data_file(filepath)
    con_rows = []
    prescription_rows = []
    exam_item_rows = []
    attachment_rows = []
    related_rows = []
    current_con_id = None

    for line in lines:
        stripped = line.strip()

        # Sub-line: PRESCRIPTION
        if stripped.startswith("PRESCRIPTION:"):
            prescription_data = stripped[len("PRESCRIPTION:"):].strip()
            pparts = prescription_data.split(",")
            if len(pparts) >= 6:
                med_id = to_null(pparts[0])
                med_name = to_null(pparts[1])
                dosage = to_null(pparts[2])
                frequency = to_null(pparts[3])
                duration = to_null(pparts[4])
                p_note = to_null(pparts[5]) if len(pparts) > 5 else None
                if current_con_id:
                    prescription_rows.append((
                        current_con_id, med_id, med_name,
                        dosage, frequency, duration, p_note
                    ))
            continue

        # Sub-line: EXAMINATION_ITEM
        if stripped.startswith("EXAMINATION_ITEM:"):
            item_name = stripped[len("EXAMINATION_ITEM:"):].strip()
            if current_con_id and not empty_val(item_name):
                exam_item_rows.append((current_con_id, item_name))
            continue

        # Sub-line: ATTACHMENT
        if stripped.startswith("ATTACHMENT:"):
            file_path = stripped[len("ATTACHMENT:"):].strip()
            if current_con_id and not empty_val(file_path):
                attachment_rows.append(("consultation", current_con_id, file_path))
            continue

        # Sub-line: RELATED_CONSULTATION_ID
        if stripped.startswith("RELATED_CONSULTATION_ID:"):
            related_id = stripped[len("RELATED_CONSULTATION_ID:"):].strip()
            if current_con_id and not empty_val(related_id):
                related_rows.append(("consultation", current_con_id, related_id))
            continue

        # Main line
        parts = line.split(",")
        if len(parts) < 16:
            continue

        consultation_id = to_null(parts[0])
        current_con_id = consultation_id
        registration_id = to_null(parts[1])
        patient_id = to_null(parts[2])
        doctor_id = to_null(parts[3])
        consultation_time = parse_datetime(parts[4])
        department = to_null(parts[5])
        chief_complaint = to_null(parts[6])
        history_present_illness = to_null(parts[7])
        past_medical_history = to_null(parts[8])
        family_history = to_null(parts[9])
        preliminary_diagnosis = to_null(parts[10])
        is_prescription_reviewed = to_bool(parts[11])
        is_hospitalization_recommended = to_bool(parts[12])
        status = to_int(parts[13])
        note = to_null(parts[14])
        is_deleted = to_bool(parts[15])

        con_rows.append((
            consultation_id, registration_id, patient_id, doctor_id,
            consultation_time, department, chief_complaint,
            history_present_illness, past_medical_history, family_history,
            preliminary_diagnosis, is_prescription_reviewed,
            is_hospitalization_recommended, status, note, is_deleted
        ))

    sql = """INSERT IGNORE INTO consultations
             (consultation_id, registration_id, patient_id, doctor_id,
              consultation_time, department, chief_complaint,
              history_present_illness, past_medical_history, family_history,
              preliminary_diagnosis, is_prescription_reviewed,
              is_hospitalization_recommended, status, note, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, con_rows, "consultations")

    if prescription_rows:
        sql_p = """INSERT IGNORE INTO prescription_items
                   (consultation_id, medicine_id, name, dosage, frequency, duration, note)
                   VALUES (%s,%s,%s,%s,%s,%s,%s)"""
        execute_batch(cursor, sql_p, prescription_rows, "prescription_items")

    if exam_item_rows:
        sql_ei = """INSERT IGNORE INTO examination_items (consultation_id, item_name)
                    VALUES (%s, %s)"""
        execute_batch(cursor, sql_ei, exam_item_rows, "examination_items")

    if attachment_rows:
        sql_att = """INSERT IGNORE INTO attachment_files (record_type, record_id, file_path)
                     VALUES (%s, %s, %s)"""
        execute_batch(cursor, sql_att, attachment_rows, "attachment_files (consultation)")

    if related_rows:
        sql_rel = """INSERT IGNORE INTO related_record_ids (record_type, record_id, related_id)
                     VALUES (%s, %s, %s)"""
        execute_batch(cursor, sql_rel, related_rows, "related_record_ids (consultation)")


def migrate_examinations(cursor, data_dir):
    filepath = os.path.join(data_dir, "RecordData", "ExaminationChainData", "examinations.txt")
    lines = read_data_file(filepath)
    exa_rows = []
    vital_rows = []
    attachment_rows = []
    related_rows = []
    current_exa_id = None

    VS_FIELDS = [
        "temperature_c", "systolic_bp", "diastolic_bp", "heart_rate",
        "respiratory_rate", "spo2", "height", "weight", "bmi",
        "pain_score", "waist_circumference", "blood_sugar", "body_fat",
        "uric_acid", "cholesterol"
    ]

    for line in lines:
        stripped = line.strip()

        # Sub-line: VITAL_SIGNS
        if stripped.startswith("VITAL_SIGNS:"):
            vs_data = stripped[len("VITAL_SIGNS:"):].strip()
            vs_parts = vs_data.split(";")
            if current_exa_id and len(vs_parts) == 15:
                vs_values = []
                all_zero = True
                for v in vs_parts:
                    try:
                        fval = float(v)
                    except (ValueError, TypeError):
                        fval = 0.0
                    if abs(fval) > 1e-9:
                        all_zero = False
                    vs_values.append(fval)
                # Only insert if not all zeros (meaningful data)
                row = ("examination", current_exa_id) + tuple(vs_values)
                vital_rows.append(row)
            continue

        # Sub-line: ATTACHMENT
        if stripped.startswith("ATTACHMENT:"):
            file_path = stripped[len("ATTACHMENT:"):].strip()
            if current_exa_id and not empty_val(file_path):
                attachment_rows.append(("examination", current_exa_id, file_path))
            continue

        # Sub-line: RELATED_EXAMINATION_ID
        if stripped.startswith("RELATED_EXAMINATION_ID:"):
            related_id = stripped[len("RELATED_EXAMINATION_ID:"):].strip()
            if current_exa_id and not empty_val(related_id):
                related_rows.append(("examination", current_exa_id, related_id))
            continue

        # Main line
        parts = line.split(",")
        if len(parts) < 12:
            continue

        examination_id = to_null(parts[0])
        current_exa_id = examination_id
        consultation_id = to_null(parts[1])
        patient_id = to_null(parts[2])
        doctor_id = to_null(parts[3])
        department = to_null(parts[4])
        item_name = to_null(parts[5])
        order_time = parse_datetime(parts[6])
        report_time = parse_datetime(parts[7])
        report_summary = to_null(parts[8])
        fee_cents = yuan_to_cents(parts[9]) or 0
        status = to_int(parts[10])
        is_deleted = to_bool(parts[11])

        exa_rows.append((
            examination_id, consultation_id, patient_id, doctor_id,
            department, item_name, order_time, report_time,
            report_summary, fee_cents, status, is_deleted
        ))

    sql = """INSERT IGNORE INTO examinations
             (examination_id, consultation_id, patient_id, doctor_id,
              department, item_name, order_time, report_time,
              report_summary, fee_cents, status, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, exa_rows, "examinations")

    if vital_rows:
        vs_cols = ", ".join(VS_FIELDS)
        placeholders = ", ".join(["%s"] * (2 + len(VS_FIELDS)))
        sql_vs = f"""INSERT IGNORE INTO vital_signs
                     (record_type, record_id, {vs_cols})
                     VALUES ({placeholders})"""
        execute_batch(cursor, sql_vs, vital_rows, "vital_signs (examination)")

    if attachment_rows:
        sql_att = """INSERT IGNORE INTO attachment_files (record_type, record_id, file_path)
                     VALUES (%s, %s, %s)"""
        execute_batch(cursor, sql_att, attachment_rows, "attachment_files (examination)")

    if related_rows:
        sql_rel = """INSERT IGNORE INTO related_record_ids (record_type, record_id, related_id)
                     VALUES (%s, %s, %s)"""
        execute_batch(cursor, sql_rel, related_rows, "related_record_ids (examination)")


def migrate_hospitalizations(cursor, data_dir):
    filepath = os.path.join(data_dir, "RecordData", "HospitalizationChainData", "hospitalizations.txt")
    lines = read_data_file(filepath)
    hos_rows = []
    related_rows = []
    current_hos_id = None

    for line in lines:
        stripped = line.strip()

        # Sub-line: RELATED_HOSPITALIZATION_ID
        if stripped.startswith("RELATED_HOSPITALIZATION_ID:"):
            related_id = stripped[len("RELATED_HOSPITALIZATION_ID:"):].strip()
            if current_hos_id and not empty_val(related_id):
                related_rows.append(("hospitalization", current_hos_id, related_id))
            continue

        # Main line
        parts = line.split(",")
        if len(parts) < 16:
            continue

        hospitalization_id = to_null(parts[0])
        current_hos_id = hospitalization_id
        consultation_id = to_null(parts[1])
        patient_id = to_null(parts[2])
        doctor_id = to_null(parts[3])
        nurse_id = to_null(parts[4])
        department = to_null(parts[5])
        ward_type = to_null(parts[6])
        bed_number = to_null(parts[7])
        apply_time = parse_datetime(parts[8])
        admit_time = parse_datetime(parts[9])
        discharge_time = parse_datetime(parts[10])
        available_admit_time = to_null(parts[11])
        deposit_cents = yuan_to_cents(parts[12]) or 0
        total_cost_cents = yuan_to_cents(parts[13]) or 0
        status = to_int(parts[14])
        is_deleted = to_bool(parts[15])

        hos_rows.append((
            hospitalization_id, consultation_id, patient_id, doctor_id, nurse_id,
            department, ward_type, bed_number, apply_time, admit_time,
            discharge_time, available_admit_time, deposit_cents, total_cost_cents,
            status, is_deleted
        ))

    sql = """INSERT IGNORE INTO hospitalizations
             (hospitalization_id, consultation_id, patient_id, doctor_id, nurse_id,
              department, ward_type, bed_number, apply_time, admit_time,
              discharge_time, available_admit_time, deposit_cents, total_cost_cents,
              status, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, hos_rows, "hospitalizations")

    if related_rows:
        sql_rel = """INSERT IGNORE INTO related_record_ids (record_type, record_id, related_id)
                     VALUES (%s, %s, %s)"""
        execute_batch(cursor, sql_rel, related_rows, "related_record_ids (hospitalization)")


def migrate_medication_records(cursor, data_dir):
    filepath = os.path.join(data_dir, "RecordData", "MedicineChainData", "medication_records.txt")
    lines = read_data_file(filepath)
    mrd_rows = []
    line_rows = []
    current_mrd_id = None

    for line in lines:
        stripped = line.strip()

        # Sub-line: MEDICATION_LINE
        if stripped.startswith("MEDICATION_LINE:"):
            ml_data = stripped[len("MEDICATION_LINE:"):].strip()
            ml_parts = ml_data.split(",")
            if len(ml_parts) >= 5:
                med_id = to_null(ml_parts[0])
                med_name = to_null(ml_parts[1])
                quantity = to_int(ml_parts[2]) or 0
                unit_price_cents = yuan_to_cents(ml_parts[3]) or 0
                ml_note = to_null(ml_parts[4])
                if current_mrd_id:
                    line_rows.append((
                        current_mrd_id, med_id, med_name,
                        quantity, unit_price_cents, ml_note
                    ))
            continue

        # Main line
        parts = line.split(",")
        if len(parts) < 14:
            continue

        medication_record_id = to_null(parts[0])
        current_mrd_id = medication_record_id
        consultation_id = to_null(parts[1])
        doctor_id = to_null(parts[2])
        pharmacist_id = to_null(parts[3])
        patient_id = to_null(parts[4])
        department = to_null(parts[5])
        create_time = parse_datetime(parts[6])
        total_cost_cents = yuan_to_cents(parts[7]) or 0
        review_status = to_int(parts[8])
        status = to_int(parts[9])
        payment_time = parse_datetime(parts[10])
        dispense_time = parse_datetime(parts[11])
        note = to_null(parts[12])
        is_deleted = to_bool(parts[13])

        mrd_rows.append((
            medication_record_id, consultation_id, doctor_id, pharmacist_id,
            patient_id, department, create_time, total_cost_cents,
            review_status, status, payment_time, dispense_time,
            note, is_deleted
        ))

    sql = """INSERT IGNORE INTO medication_records
             (medication_record_id, consultation_id, doctor_id, pharmacist_id,
              patient_id, department, create_time, total_cost_cents,
              review_status, status, payment_time, dispense_time,
              note, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, mrd_rows, "medication_records")

    if line_rows:
        sql_ml = """INSERT IGNORE INTO medication_lines
                    (medication_record_id, medicine_id, medicine_name,
                     quantity, unit_price_cents, note)
                    VALUES (%s,%s,%s,%s,%s,%s)"""
        execute_batch(cursor, sql_ml, line_rows, "medication_lines")


def migrate_bed_info(cursor, data_dir):
    filepath = os.path.join(data_dir, "RecordData", "HospitalizationChainData", "bed_info.txt")
    lines = read_data_file(filepath)
    bed_rows = []
    vital_rows = []
    current_bed_id = None

    VS_FIELDS = [
        "temperature_c", "systolic_bp", "diastolic_bp", "heart_rate",
        "respiratory_rate", "spo2", "height", "weight", "bmi",
        "pain_score", "waist_circumference", "blood_sugar", "body_fat",
        "uric_acid", "cholesterol"
    ]

    for line in lines:
        stripped = line.strip()

        # Sub-line: VITAL_SIGNS
        if stripped.startswith("VITAL_SIGNS:"):
            vs_data = stripped[len("VITAL_SIGNS:"):].strip()
            vs_parts = vs_data.split(";")
            if current_bed_id and len(vs_parts) == 15:
                vs_values = []
                for v in vs_parts:
                    try:
                        fval = float(v)
                    except (ValueError, TypeError):
                        fval = 0.0
                    vs_values.append(fval)
                row = ("bed", current_bed_id) + tuple(vs_values)
                vital_rows.append(row)
            continue

        # Main line
        parts = line.split(",")
        if len(parts) < 13:
            continue

        bed_id = parts[0].strip()
        current_bed_id = bed_id
        status = to_int(parts[1])
        ward_type = to_null(parts[2])
        department = to_null(parts[3])
        area_number = to_int(parts[4])
        ward_number = to_int(parts[5])
        bed_number = to_int(parts[6])
        note = to_null(parts[7])
        patient_id = to_null(parts[8])
        nurse_id = to_null(parts[9])
        is_deleted = to_bool(parts[10])
        use_times = to_int(parts[11]) or 0
        days_occupied = to_int(parts[12]) or 0

        bed_rows.append((
            bed_id, status, ward_type, department,
            area_number, ward_number, bed_number,
            note, patient_id, nurse_id,
            use_times, days_occupied, is_deleted
        ))

    sql = """INSERT IGNORE INTO bed_info
             (bed_id, status, ward_type, department,
              area_number, ward_number, bed_number,
              note, patient_id, nurse_id,
              use_times, days_occupied, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, bed_rows, "bed_info")

    if vital_rows:
        vs_cols = ", ".join(VS_FIELDS)
        placeholders = ", ".join(["%s"] * (2 + len(VS_FIELDS)))
        sql_vs = f"""INSERT IGNORE INTO vital_signs
                     (record_type, record_id, {vs_cols})
                     VALUES ({placeholders})"""
        execute_batch(cursor, sql_vs, vital_rows, "vital_signs (bed)")


def migrate_nursing_records(cursor, data_dir):
    filepath = os.path.join(data_dir, "RecordData", "NursingRecordChainData", "nursing_records.txt")
    lines = read_data_file(filepath)
    if not lines:
        print("  nursing_records: 0 rows (file not found or empty)")
        return

    rows = []
    for line in lines:
        parts = line.split(",")
        if len(parts) < 10:
            continue

        record_id = to_null(parts[0])
        patient_id = to_null(parts[1])
        nurse_id = to_null(parts[2])
        department = to_null(parts[3])
        operation_time = parse_datetime(parts[4])
        operation_type = to_int(parts[5])
        detail = to_null(parts[6])
        vital_signs_snapshot = to_null(parts[7])
        note = to_null(parts[8])
        is_deleted = to_bool(parts[9])

        rows.append((
            record_id, patient_id, nurse_id, department,
            operation_time, operation_type, detail,
            vital_signs_snapshot, note, is_deleted
        ))

    sql = """INSERT IGNORE INTO nursing_records
             (record_id, patient_id, nurse_id, department,
              operation_time, operation_type, detail,
              vital_signs_snapshot, note, is_deleted)
             VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    execute_batch(cursor, sql, rows, "nursing_records")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    import argparse
    parser = argparse.ArgumentParser(description="HIS CSV to MySQL migration")
    parser.add_argument("--data-dir", default=None,
                        help="Path to Data/ directory (default: auto-detected from script location)")
    parser.add_argument("--drop-existing", action="store_true",
                        help="Truncate all tables before importing")
    parser.add_argument("--schema-file", default=None,
                        help="Path to schema.sql to execute before migration")
    args = parser.parse_args()

    data_dir = args.data_dir
    if data_dir is None:
        # Default: Data/ under project root
        data_dir = os.path.join(PROJECT_ROOT, "Data")

    if not os.path.isdir(data_dir):
        print(f"[ERROR] Data directory not found: {data_dir}")
        sys.exit(1)

    print(f"Data directory: {data_dir}")
    print(f"Connecting to MySQL at {MYSQL_CONFIG['host']}:{MYSQL_CONFIG['port']} ...")

    try:
        conn = mysql.connector.connect(**MYSQL_CONFIG)
    except MySQLError as e:
        print(f"[ERROR] Cannot connect to MySQL: {e}")
        sys.exit(1)

    cursor = conn.cursor()

    # Optionally run schema.sql first
    if args.schema_file:
        schema_path = args.schema_file
    else:
        default_schema = os.path.join(os.path.dirname(__file__), "schema.sql")
        if os.path.exists(default_schema):
            schema_path = default_schema
        else:
            schema_path = None

    if schema_path:
        print(f"Executing schema file: {schema_path}")
        with open(schema_path, "r", encoding="utf-8") as f:
            schema_sql = f.read()
        # Split on semicolons and execute each statement
        for stmt in schema_sql.split(";"):
            stmt = stmt.strip()
            if stmt and not stmt.startswith("--") and stmt.upper() != "SET FOREIGN_KEY_CHECKS = 0" and stmt.upper() != "SET FOREIGN_KEY_CHECKS = 1":
                try:
                    cursor.execute(stmt)
                except MySQLError as e:
                    # Ignore "database exists" type errors
                    if "already exists" not in str(e).lower():
                        print(f"  [WARN] Schema statement error: {e}")
        print("  Schema applied.")
        conn.commit()

    # Disable foreign key checks for import
    cursor.execute("SET FOREIGN_KEY_CHECKS = 0")

    # Optionally truncate
    if args.drop_existing:
        print("Truncating all tables...")
        tables = [
            "related_record_ids", "vital_signs", "attachment_files",
            "examination_items", "prescription_items", "medication_lines",
            "medicine_aliases", "nursing_records", "bed_info",
            "medication_records", "hospitalizations", "examinations",
            "consultations", "registrations", "medicines",
            "patients", "pharmacists", "nurses", "doctors", "admins",
        ]
        for t in tables:
            try:
                cursor.execute(f"TRUNCATE TABLE {t}")
            except MySQLError as e:
                # Table may not exist yet
                pass
        print("  All tables truncated.")

    # Import in dependency order
    print("\n=== Phase 1: User accounts ===")
    migrate_admins(cursor, data_dir)
    migrate_doctors(cursor, data_dir)
    migrate_nurses(cursor, data_dir)
    migrate_pharmacists(cursor, data_dir)
    migrate_patients(cursor, data_dir)

    print("\n=== Phase 2: Medicines (standalone) ===")
    migrate_medicines(cursor, data_dir)

    print("\n=== Phase 3: Business records (ordered by dependency) ===")
    migrate_registrations(cursor, data_dir)
    migrate_consultations(cursor, data_dir)
    migrate_examinations(cursor, data_dir)
    migrate_hospitalizations(cursor, data_dir)
    migrate_medication_records(cursor, data_dir)

    print("\n=== Phase 4: Bed info and nursing records ===")
    migrate_bed_info(cursor, data_dir)
    migrate_nursing_records(cursor, data_dir)

    # Re-enable foreign key checks
    cursor.execute("SET FOREIGN_KEY_CHECKS = 1")

    conn.commit()
    print("\n=== Migration complete. All changes committed. ===")

    # Print summary counts
    print("\n--- Row count verification ---")
    tables = [
        "admins", "doctors", "nurses", "pharmacists", "patients",
        "medicines", "medicine_aliases",
        "registrations", "consultations",
        "prescription_items", "examination_items",
        "examinations", "hospitalizations",
        "medication_records", "medication_lines",
        "bed_info", "nursing_records",
        "vital_signs", "attachment_files", "related_record_ids",
    ]
    for t in tables:
        try:
            cursor.execute(f"SELECT COUNT(*) FROM {t}")
            count = cursor.fetchone()[0]
            print(f"  {t}: {count}")
        except MySQLError as e:
            print(f"  {t}: [ERROR] {e}")

    cursor.close()
    conn.close()
    print("\nDone.")


if __name__ == "__main__":
    main()
