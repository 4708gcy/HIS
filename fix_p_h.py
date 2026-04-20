import re

patient_h = "D:/vscode编程练习c/C课设-HIS/Head/Patient.h"
patient_cpp = "D:/vscode编程练习c/C课设-HIS/Source/Patient.cpp"

with open(patient_h, 'r', encoding='utf-8') as f:
    header = f.read()

# adding methods in Patient.h
new_methods = """
    // ---------------- 住院信息管理功能 ----------------
    void manageHospitalizations(Hospitalization *&hosHead, bedInfo *&bedHead);
    bool getAllHospitalizations(Hospitalization *&hosHead);
    bool getHospitalizationByID(Hospitalization *&hosHead);
    bool getHospitalizationsByAdmitTimeRange(Hospitalization *&hosHead);
    bool getHospitalizationsByDischargeTimeRange(Hospitalization *&hosHead);
    bool getHospitalizationsByStatus(Hospitalization *&hosHead, int select = -1);
    bool getHospitalizationsByDepartment(Hospitalization *&hosHead);
    bool getHospitalizationsByWardType(Hospitalization *&hosHead);
    bool getHospitalizationsByBedNumber(Hospitalization *&hosHead);
    bool getHospitalizationsByDoctorID(Hospitalization *&hosHead);
    bool getHospitalizationsByConsultationID(Hospitalization *&hosHead);

    void applyForDischarge(Hospitalization *&hosHead, bedInfo *&bedHead);
    void payHospitalizationDeposit(Hospitalization *&hosHead);
"""

if "manageHospitalizations" not in header:
    header = header.replace("    // ---------------- 患者个人信息管理 ----------------", new_methods + "\n    // ---------------- 患者个人信息管理 ----------------")

with open(patient_h, 'w', encoding='utf-8') as f:
    f.write(header)

print("Patient.h updated.")
