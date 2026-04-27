#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include "json.hpp"
#include "../Head/Registration.h"
#include "../Head/Consultation.h"
#include "../Head/Examination.h"
#include "../Head/Hospitalization.h"
#include "../Head/MedicationRecord.h"
#include "../Head/Medicine.h"
#include "../Head/User.h"
#include "../Head/Doctor.h"
#include "../Head/Nurse.h"
#include "../Head/Pharmacist.h"
#include "../Head/Patient.h"
#include "../Head/Admin.h"

using json = nlohmann::json;

namespace JsonHelper
{
    // ===== 数据结构序列化 =====
    json toJson(const Registration *reg);
    json toJson(const Consultation *con);
    json toJson(const Examination *exa);
    json toJson(const Hospitalization *hos);
    json toJson(const MedicationRecord *med);
    json toJson(const Medicine *med);
    json toJson(const bedInfo *bed);
    json toJson(const VitalSigns &vs);

    // ===== 角色用户序列化（含基础信息） =====
    json userToJson(const Admin *admin);
    json userToJson(const Doctor *doc);
    json userToJson(const Nurse *nurse);
    json userToJson(const Pharmacist *pha);
    json userToJson(const Patient *pat);

    // ===== 枚举转字符串 =====
    std::string regStatusToStr(int status);
    std::string conStatusToStr(int status);
    std::string examStatusToStr(int status);
    std::string hosStatusToStr(int status);
    std::string medStatusToStr(int status);
    std::string medReviewStatusToStr(int status);
    std::string medicineStatusToStr(int status);
    std::string bedStatusToStr(int status);
    std::string roleToStr(int role);
}

#endif
