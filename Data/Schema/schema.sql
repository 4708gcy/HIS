-- HIS Database Schema
-- MySQL 8.0+ | utf8mb4

CREATE DATABASE IF NOT EXISTS his_db
  CHARACTER SET utf8mb4
  COLLATE utf8mb4_unicode_ci;

USE his_db;

SET FOREIGN_KEY_CHECKS = 0;

-- ============================================================
-- 主表 (14 张)
-- ============================================================

-- (1) 管理员
DROP TABLE IF EXISTS admins;
CREATE TABLE admins (
    admin_id VARCHAR(10) PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    stored_hash VARCHAR(150) NOT NULL,
    salt VARCHAR(20) NOT NULL,
    login_attempts INT DEFAULT 0,
    is_active TINYINT DEFAULT 1,
    create_time DATETIME,
    gender VARCHAR(5),
    age INT,
    telephone VARCHAR(20),
    email VARCHAR(100),
    is_deleted TINYINT DEFAULT 0,
    UNIQUE INDEX ux_admins_username (username)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (2) 医生
DROP TABLE IF EXISTS doctors;
CREATE TABLE doctors (
    doctor_id VARCHAR(10) PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    stored_hash VARCHAR(150) NOT NULL,
    salt VARCHAR(20) NOT NULL,
    login_attempts INT DEFAULT 0,
    is_active TINYINT DEFAULT 1,
    create_time DATETIME,
    gender VARCHAR(5),
    age INT,
    telephone VARCHAR(20),
    email VARCHAR(100),
    department VARCHAR(50),
    title INT COMMENT '1=实习 2=住院 3=主治 4=副主任 5=主任',
    specialty VARCHAR(200),
    schedule_info VARCHAR(200),
    consultation_count INT DEFAULT 0,
    examination_count INT DEFAULT 0,
    hospitalization_apply_count INT DEFAULT 0,
    is_on_duty TINYINT DEFAULT 1,
    is_deleted TINYINT DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (3) 护士
DROP TABLE IF EXISTS nurses;
CREATE TABLE nurses (
    nurse_id VARCHAR(10) PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    stored_hash VARCHAR(150) NOT NULL,
    salt VARCHAR(20) NOT NULL,
    login_attempts INT DEFAULT 0,
    is_active TINYINT DEFAULT 1,
    create_time DATETIME,
    gender VARCHAR(5),
    age INT,
    telephone VARCHAR(20),
    email VARCHAR(100),
    department VARCHAR(50),
    level INT COMMENT '1=实习 2=初级 3=高级 4=护士长',
    schedule_info VARCHAR(200),
    patient_care_count INT DEFAULT 0,
    bed_manage_count INT DEFAULT 0,
    is_on_duty TINYINT DEFAULT 1,
    is_deleted TINYINT DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (4) 药剂师
DROP TABLE IF EXISTS pharmacists;
CREATE TABLE pharmacists (
    pharmacist_id VARCHAR(10) PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    stored_hash VARCHAR(150) NOT NULL,
    salt VARCHAR(20) NOT NULL,
    login_attempts INT DEFAULT 0,
    is_active TINYINT DEFAULT 1,
    create_time DATETIME,
    gender VARCHAR(5),
    age INT,
    telephone VARCHAR(20),
    email VARCHAR(100),
    department VARCHAR(50),
    level INT COMMENT '1=实习 2=初级 3=高级 4=主管',
    schedule_info VARCHAR(200),
    review_count INT DEFAULT 0,
    dispense_count INT DEFAULT 0,
    inventory_manage_count INT DEFAULT 0,
    is_on_duty TINYINT DEFAULT 1,
    is_deleted TINYINT DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (5) 患者
DROP TABLE IF EXISTS patients;
CREATE TABLE patients (
    patient_id VARCHAR(10) PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    stored_hash VARCHAR(150) NOT NULL,
    salt VARCHAR(20) NOT NULL,
    login_attempts INT DEFAULT 0,
    is_active TINYINT DEFAULT 1,
    create_time DATETIME,
    gender VARCHAR(5),
    age INT,
    telephone VARCHAR(20),
    email VARCHAR(100),
    department VARCHAR(50),
    address VARCHAR(300),
    id_card_number VARCHAR(20),
    emergency_contact_name VARCHAR(50),
    emergency_contact_phone VARCHAR(20),
    allergy_history VARCHAR(500),
    past_medical_history VARCHAR(500),
    marital_status INT COMMENT '1=未婚 2=已婚 3=离异 4=丧偶',
    registration_count INT DEFAULT 0,
    consultation_count INT DEFAULT 0,
    hospitalization_count INT DEFAULT 0,
    medication_count INT DEFAULT 0,
    balance_cents INT DEFAULT 0 COMMENT '余额（分）',
    is_hospitalized TINYINT DEFAULT 0,
    is_deleted TINYINT DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (6) 挂号记录
DROP TABLE IF EXISTS registrations;
CREATE TABLE registrations (
    registration_id VARCHAR(12) PRIMARY KEY,
    patient_id VARCHAR(10),
    department VARCHAR(50),
    doctor_id VARCHAR(10),
    register_time DATETIME,
    fee_cents INT DEFAULT 0,
    status INT COMMENT '1=BOOKED 2=PAID 3=CANCELED 4=FINISHED',
    note TEXT,
    is_deleted TINYINT DEFAULT 0,
    INDEX idx_patient (patient_id),
    INDEX idx_doctor (doctor_id),
    CONSTRAINT ck_reg_fee CHECK (fee_cents >= 0),
    CONSTRAINT ck_reg_status CHECK (status BETWEEN 1 AND 4),
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE SET NULL,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (7) 看诊记录
DROP TABLE IF EXISTS consultations;
CREATE TABLE consultations (
    consultation_id VARCHAR(12) PRIMARY KEY,
    registration_id VARCHAR(12),
    patient_id VARCHAR(10),
    doctor_id VARCHAR(10),
    consultation_time DATETIME,
    department VARCHAR(50),
    chief_complaint TEXT,
    history_present_illness TEXT,
    past_medical_history TEXT,
    family_history TEXT,
    preliminary_diagnosis TEXT,
    is_prescription_reviewed TINYINT DEFAULT 0,
    is_hospitalization_recommended TINYINT DEFAULT 0,
    status INT COMMENT '1=PENDING 2=IN_PROGRESS 3=COMPLETED 4=VOIDED',
    note TEXT,
    is_deleted TINYINT DEFAULT 0,
    INDEX idx_registration (registration_id),
    INDEX idx_patient (patient_id),
    INDEX idx_doctor (doctor_id),
    CONSTRAINT ck_con_status CHECK (status BETWEEN 1 AND 4),
    FOREIGN KEY (registration_id) REFERENCES registrations(registration_id) ON DELETE SET NULL,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE SET NULL,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (8) 检查记录
DROP TABLE IF EXISTS examinations;
CREATE TABLE examinations (
    examination_id VARCHAR(12) PRIMARY KEY,
    consultation_id VARCHAR(12),
    patient_id VARCHAR(10),
    doctor_id VARCHAR(10),
    department VARCHAR(50),
    item_name VARCHAR(100),
    order_time DATETIME,
    report_time DATETIME,
    report_summary TEXT,
    fee_cents INT DEFAULT 0,
    status INT COMMENT '1=ORDERED 2=PAID 3=IN_PROGRESS 4=COMPLETED 5=CANCELED 6=VOIDED',
    is_deleted TINYINT DEFAULT 0,
    INDEX idx_consultation (consultation_id),
    INDEX idx_patient (patient_id),
    INDEX idx_doctor (doctor_id),
    CONSTRAINT ck_exam_fee CHECK (fee_cents >= 0),
    CONSTRAINT ck_exam_status CHECK (status BETWEEN 1 AND 6),
    FOREIGN KEY (consultation_id) REFERENCES consultations(consultation_id) ON DELETE SET NULL,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE SET NULL,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (9) 住院记录
DROP TABLE IF EXISTS hospitalizations;
CREATE TABLE hospitalizations (
    hospitalization_id VARCHAR(12) PRIMARY KEY,
    consultation_id VARCHAR(12),
    patient_id VARCHAR(10),
    doctor_id VARCHAR(10),
    nurse_id VARCHAR(10),
    department VARCHAR(50),
    ward_type VARCHAR(10),
    bed_number VARCHAR(20),
    apply_time DATETIME,
    admit_time DATETIME,
    discharge_time DATETIME,
    available_admit_time VARCHAR(30),
    deposit_cents INT DEFAULT 0,
    total_cost_cents INT DEFAULT 0,
    status INT COMMENT '1=APPLIED 2=ADMITTED 3=DISCHARGED 4=CANCELED 5=VOIDED',
    is_deleted TINYINT DEFAULT 0,
    INDEX idx_consultation (consultation_id),
    INDEX idx_patient (patient_id),
    INDEX idx_doctor (doctor_id),
    INDEX idx_nurse (nurse_id),
    CONSTRAINT ck_hos_deposit CHECK (deposit_cents >= 0),
    CONSTRAINT ck_hos_total CHECK (total_cost_cents >= 0),
    CONSTRAINT ck_hos_status CHECK (status BETWEEN 1 AND 5),
    FOREIGN KEY (consultation_id) REFERENCES consultations(consultation_id) ON DELETE SET NULL,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE SET NULL,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE SET NULL,
    FOREIGN KEY (nurse_id) REFERENCES nurses(nurse_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (10) 用药记录
DROP TABLE IF EXISTS medication_records;
CREATE TABLE medication_records (
    medication_record_id VARCHAR(12) PRIMARY KEY,
    consultation_id VARCHAR(12),
    doctor_id VARCHAR(10),
    pharmacist_id VARCHAR(10),
    patient_id VARCHAR(10),
    department VARCHAR(50),
    create_time DATETIME,
    total_cost_cents INT DEFAULT 0,
    review_status INT COMMENT '1=PENDING_REVIEW 2=REJECTED 3=APPROVED 4=CANCELED',
    status INT COMMENT '1=UNPAID 2=PAID 3=DISPENSED 4=REFUNDED',
    payment_time DATETIME,
    dispense_time DATETIME,
    note TEXT,
    is_deleted TINYINT DEFAULT 0,
    INDEX idx_consultation (consultation_id),
    INDEX idx_doctor (doctor_id),
    INDEX idx_pharmacist (pharmacist_id),
    INDEX idx_patient (patient_id),
    CONSTRAINT ck_medrec_total CHECK (total_cost_cents >= 0),
    CONSTRAINT ck_medrec_review CHECK (review_status BETWEEN 1 AND 4),
    CONSTRAINT ck_medrec_status CHECK (status BETWEEN 1 AND 4),
    FOREIGN KEY (consultation_id) REFERENCES consultations(consultation_id) ON DELETE SET NULL,
    FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE SET NULL,
    FOREIGN KEY (pharmacist_id) REFERENCES pharmacists(pharmacist_id) ON DELETE SET NULL,
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (11) 药品库存
DROP TABLE IF EXISTS medicines;
CREATE TABLE medicines (
    medicine_id VARCHAR(12) PRIMARY KEY,
    name VARCHAR(100),
    specification VARCHAR(100),
    manufacturer VARCHAR(100),
    purchase_price_cents INT DEFAULT 0,
    sale_price_cents INT DEFAULT 0,
    stock INT DEFAULT 0,
    safety_stock INT DEFAULT 0,
    production_date VARCHAR(20),
    expiry_date VARCHAR(20),
    department VARCHAR(50),
    is_special TINYINT DEFAULT 0,
    generic_name VARCHAR(100),
    status INT COMMENT '1=NORMAL 2=LOW_STOCK 3=EXPIRED 4=DISCONTINUED',
    note TEXT,
    is_deleted TINYINT DEFAULT 0,
    CONSTRAINT ck_medicine_stock CHECK (stock >= 0),
    CONSTRAINT ck_medicine_safety CHECK (safety_stock >= 0),
    CONSTRAINT ck_medicine_status CHECK (status BETWEEN 1 AND 4)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (12) 床位信息
DROP TABLE IF EXISTS bed_info;
CREATE TABLE bed_info (
    bed_id VARCHAR(30) PRIMARY KEY,
    status INT COMMENT '0=AVAILABLE 1=OCCUPIED 2=CLEANING 3=UNAVAILABLE',
    ward_type VARCHAR(10),
    department VARCHAR(50),
    area_number INT,
    ward_number INT,
    bed_number INT,
    note TEXT,
    patient_id VARCHAR(10),
    nurse_id VARCHAR(10),
    use_times INT DEFAULT 0,
    days_occupied INT DEFAULT 0,
    is_deleted TINYINT DEFAULT 0,
    INDEX idx_patient (patient_id),
    INDEX idx_nurse (nurse_id),
    CONSTRAINT ck_bed_status CHECK (status BETWEEN 0 AND 3),
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE SET NULL,
    FOREIGN KEY (nurse_id) REFERENCES nurses(nurse_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (13) 护理记录
DROP TABLE IF EXISTS nursing_records;
CREATE TABLE nursing_records (
    record_id VARCHAR(12) PRIMARY KEY,
    patient_id VARCHAR(10),
    nurse_id VARCHAR(10),
    department VARCHAR(50),
    operation_time DATETIME,
    operation_type INT COMMENT '1=生命体征 2=输液 3=注射 4=口服给药 5=换药 6=翻身 7=导尿 8=其他',
    detail TEXT,
    vital_signs_snapshot TEXT COMMENT '15项生命体征，分号分隔',
    note TEXT,
    is_deleted TINYINT DEFAULT 0,
    INDEX idx_patient (patient_id),
    INDEX idx_nurse (nurse_id),
    CONSTRAINT ck_nur_op_type CHECK (operation_type BETWEEN 1 AND 8),
    FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE SET NULL,
    FOREIGN KEY (nurse_id) REFERENCES nurses(nurse_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ============================================================
-- 子表 (7 张) — 嵌套数据的规范化
-- ============================================================

-- (14a) 处方明细
DROP TABLE IF EXISTS prescription_items;
CREATE TABLE prescription_items (
    id INT AUTO_INCREMENT PRIMARY KEY,
    consultation_id VARCHAR(12),
    medicine_id VARCHAR(12),
    name VARCHAR(100),
    dosage VARCHAR(50),
    frequency VARCHAR(50),
    duration VARCHAR(50),
    note TEXT,
    INDEX idx_consultation (consultation_id),
    FOREIGN KEY (consultation_id) REFERENCES consultations(consultation_id) ON DELETE CASCADE,
    FOREIGN KEY (medicine_id) REFERENCES medicines(medicine_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (14b) 看诊中的检查项目
DROP TABLE IF EXISTS examination_items;
CREATE TABLE examination_items (
    id INT AUTO_INCREMENT PRIMARY KEY,
    consultation_id VARCHAR(12),
    item_name VARCHAR(100),
    INDEX idx_consultation (consultation_id),
    FOREIGN KEY (consultation_id) REFERENCES consultations(consultation_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (14c) 附件文件路径
DROP TABLE IF EXISTS attachment_files;
CREATE TABLE attachment_files (
    id INT AUTO_INCREMENT PRIMARY KEY,
    record_type VARCHAR(20) COMMENT 'consultation/examination',
    record_id VARCHAR(30),
    file_path VARCHAR(500),
    INDEX idx_record (record_type, record_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (14d) 用药记录明细行
DROP TABLE IF EXISTS medication_lines;
CREATE TABLE medication_lines (
    id INT AUTO_INCREMENT PRIMARY KEY,
    medication_record_id VARCHAR(12),
    medicine_id VARCHAR(12),
    medicine_name VARCHAR(100),
    quantity INT,
    unit_price_cents INT,
    note TEXT,
    INDEX idx_med_record (medication_record_id),
    FOREIGN KEY (medication_record_id) REFERENCES medication_records(medication_record_id) ON DELETE CASCADE,
    FOREIGN KEY (medicine_id) REFERENCES medicines(medicine_id) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (14e) 药品别名
DROP TABLE IF EXISTS medicine_aliases;
CREATE TABLE medicine_aliases (
    id INT AUTO_INCREMENT PRIMARY KEY,
    medicine_id VARCHAR(12),
    alias VARCHAR(100),
    INDEX idx_medicine (medicine_id),
    FOREIGN KEY (medicine_id) REFERENCES medicines(medicine_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (14f) 生命体征
DROP TABLE IF EXISTS vital_signs;
CREATE TABLE vital_signs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    record_type VARCHAR(20) COMMENT 'examination/bed',
    record_id VARCHAR(30),
    temperature_c DOUBLE,
    systolic_bp DOUBLE,
    diastolic_bp DOUBLE,
    heart_rate DOUBLE,
    respiratory_rate DOUBLE,
    spo2 DOUBLE,
    height DOUBLE,
    weight DOUBLE,
    bmi DOUBLE,
    pain_score DOUBLE,
    waist_circumference DOUBLE,
    blood_sugar DOUBLE,
    body_fat DOUBLE,
    uric_acid DOUBLE,
    cholesterol DOUBLE,
    INDEX idx_record (record_type, record_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- (14g) 关联记录 ID
DROP TABLE IF EXISTS related_record_ids;
CREATE TABLE related_record_ids (
    id INT AUTO_INCREMENT PRIMARY KEY,
    record_type VARCHAR(30) COMMENT 'registration/consultation/examination/hospitalization',
    record_id VARCHAR(30),
    related_id VARCHAR(12),
    INDEX idx_record (record_type, record_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 操作日志表
DROP TABLE IF EXISTS operation_logs;
CREATE TABLE operation_logs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    timestamp DATETIME,
    user_id VARCHAR(10),
    role VARCHAR(20),
    operation_type VARCHAR(50),
    detail TEXT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

SET FOREIGN_KEY_CHECKS = 1;
