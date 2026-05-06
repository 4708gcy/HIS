#include "JsonHelper.h"
#include <cmath>

namespace JsonHelper
{
    // 辅助函数：处理 "#" 哨兵值，转为空字符串或 null
    static std::string cleanStr(const std::string &s)
    {
        return (s == "#" || s.empty()) ? "" : s;
    }

    static json cleanStrToJson(const std::string &s)
    {
        return cleanStr(s).empty() ? json(nullptr) : json(cleanStr(s));
    }

    // 医生职称转字符串（内部使用）
    static std::string doctorTitleToStr(int title)
    {
        switch (static_cast<DoctorTitle>(title))
        {
        case DoctorTitle::INTERN: return "实习医生";
        case DoctorTitle::RESIDENT: return "住院医师";
        case DoctorTitle::ATTENDING: return "主治医师";
        case DoctorTitle::ASSOCIATE_CHIEF: return "副主任医师";
        case DoctorTitle::CHIEF: return "主任医师";
        default: return "未知";
        }
    }

    json toJson(const Registration *reg)
    {
        json j;
        j["registrationID"] = cleanStr(reg->registrationID);
        j["patientID"] = cleanStr(reg->patientID);
        j["department"] = cleanStr(reg->department);
        j["doctorID"] = cleanStr(reg->doctorID);
        j["registerTime"] = cleanStr(reg->registerTime);
        j["fee"] = reg->fee;
        j["status"] = static_cast<int>(reg->status);
        j["statusStr"] = regStatusToStr(static_cast<int>(reg->status));
        j["note"] = cleanStr(reg->note);
        j["isDeleted"] = reg->isDeleted;
        json relatedList = json::array();
        for (const auto &r : reg->relatedRegistrationIDs)
            relatedList.push_back(r);
        j["relatedRegistrationIDs"] = relatedList;
        return j;
    }

    json toJson(const Consultation *con)
    {
        json j;
        j["consultationID"] = cleanStr(con->consultationID);
        j["registrationID"] = cleanStr(con->registrationID);
        j["patientID"] = cleanStr(con->patientID);
        j["doctorID"] = cleanStr(con->doctorID);
        j["consultationTime"] = cleanStr(con->consultationTime);
        j["department"] = cleanStr(con->department);
        j["chiefComplaint"] = cleanStr(con->chiefComplaint);
        j["historyOfPresentIllness"] = cleanStr(con->historyOfPresentIllness);
        j["pastMedicalHistory"] = cleanStr(con->pastMedicalHistory);
        j["familyHistory"] = cleanStr(con->familyHistory);
        j["preliminaryDiagnosis"] = cleanStr(con->preliminaryDiagnosis);
        j["isPrescriptionReviewed"] = con->isPrescriptionReviewed;
        j["isHospitalizationRecommended"] = con->isHospitalizationRecommended;
        j["note"] = cleanStr(con->note);
        j["status"] = static_cast<int>(con->status);
        j["statusStr"] = conStatusToStr(static_cast<int>(con->status));
        j["isDeleted"] = con->isDeleted;

        json examList = json::array();
        for (const auto &e : con->examinationlist)
            examList.push_back(e);
        j["examinationList"] = examList;

        json prescList = json::array();
        for (const auto &p : con->prescriptions)
        {
            json pj;
            pj["medicineID"] = cleanStr(p.medicineID);
            pj["name"] = cleanStr(p.name);
            pj["quantity"] = p.quantity;
            pj["dosage"] = cleanStr(p.dosage);
            pj["frequency"] = cleanStr(p.frequency);
            pj["duration"] = cleanStr(p.duration);
            pj["note"] = cleanStr(p.note);
            prescList.push_back(pj);
        }
        j["prescriptions"] = prescList;

        json attachList = json::array();
        for (const auto &a : con->attachments)
            attachList.push_back(a);
        j["attachments"] = attachList;

        json relatedList = json::array();
        for (const auto &r : con->relatedConsultationIDs)
            relatedList.push_back(r);
        j["relatedConsultationIDs"] = relatedList;

        return j;
    }

    json toJson(const Examination *exa)
    {
        json j;
        j["examinationID"] = cleanStr(exa->examinationID);
        j["consultationID"] = cleanStr(exa->consultationID);
        j["patientID"] = cleanStr(exa->patientID);
        j["doctorID"] = cleanStr(exa->doctorID);
        j["department"] = cleanStr(exa->department);
        j["itemName"] = cleanStr(exa->itemName);
        j["orderTime"] = cleanStr(exa->orderTime);
        j["reportTime"] = cleanStr(exa->reportTime);
        j["reportSummary"] = cleanStr(exa->reportSummary);
        j["vitalSigns"] = toJson(exa->vitalSigns);
        j["fee"] = exa->fee;
        j["status"] = static_cast<int>(exa->status);
        j["statusStr"] = examStatusToStr(static_cast<int>(exa->status));
        j["note"] = cleanStr(exa->note);
        j["isDeleted"] = exa->isDeleted;

        json attachList = json::array();
        for (const auto &a : exa->attachments)
            attachList.push_back(a);
        j["attachments"] = attachList;

        json relatedList = json::array();
        for (const auto &r : exa->relatedExaminationIDs)
            relatedList.push_back(r);
        j["relatedExaminationIDs"] = relatedList;

        return j;
    }

    json toJson(const VitalSigns &vs)
    {
        json j;
        j["temperatureC"] = vs.temperatureC;
        j["systolicBP"] = vs.systolicBP;
        j["diastolicBP"] = vs.diastolicBP;
        j["heartRate"] = vs.heartRate;
        j["respiratoryRate"] = vs.respiratoryRate;
        j["spo2"] = vs.spo2;
        j["height"] = vs.height;
        j["weight"] = vs.weight;
        j["bmi"] = vs.bmi;
        j["painScore"] = vs.painScore;
        j["waistCircumference"] = vs.waistCircumference;
        j["bloodSugar"] = vs.bloodSugar;
        j["bodyFat"] = vs.bodyFat;
        j["uricAcid"] = vs.uricAcid;
        j["cholesterol"] = vs.cholesterol;
        return j;
    }

    json toJson(const Hospitalization *hos)
    {
        json j;
        j["hospitalizationID"] = cleanStr(hos->hospitalizationID);
        j["consultationID"] = cleanStr(hos->consultationID);
        j["patientID"] = cleanStr(hos->patientID);
        j["doctorID"] = cleanStr(hos->doctorID);
        j["nurseID"] = cleanStr(hos->nurseID);
        j["department"] = cleanStr(hos->department);
        j["wardType"] = cleanStr(hos->wardType);
        j["bedNumber"] = cleanStr(hos->bedNumber);
        j["applyTime"] = cleanStr(hos->applyTime);
        j["admitTime"] = cleanStr(hos->admitTime);
        j["dischargeTime"] = cleanStr(hos->dischargeTime);
        j["availableAdmitTime"] = cleanStr(hos->availableAdmitTime);
        j["deposit"] = hos->deposit;
        j["totalCost"] = hos->totalCost;
        j["status"] = static_cast<int>(hos->status);
        j["statusStr"] = hosStatusToStr(static_cast<int>(hos->status));
        j["isDeleted"] = hos->isDeleted;

        json relatedList = json::array();
        for (const auto &r : hos->relatedHospitalizationIDs)
            relatedList.push_back(r);
        j["relatedHospitalizationIDs"] = relatedList;

        return j;
    }

    json toJson(const MedicationRecord *med)
    {
        json j;
        j["medRecordID"] = cleanStr(med->medRecordID);
        j["consultationID"] = cleanStr(med->consultationID);
        j["doctorID"] = cleanStr(med->doctorID);
        j["pharmacistID"] = cleanStr(med->pharmacistID);
        j["patientID"] = cleanStr(med->patientID);
        j["department"] = cleanStr(med->department);
        j["createTime"] = cleanStr(med->createTime);
        j["totalCost"] = med->totalCost;
        j["reviewStatus"] = static_cast<int>(med->reviewStatus);
        j["reviewStatusStr"] = medReviewStatusToStr(static_cast<int>(med->reviewStatus));
        j["status"] = static_cast<int>(med->status);
        j["statusStr"] = medStatusToStr(static_cast<int>(med->status));
        j["paymentTime"] = cleanStr(med->paymentTime);
        j["dispenseTime"] = cleanStr(med->dispenseTime);
        j["note"] = cleanStr(med->note);
        j["isDeleted"] = med->isDeleted;

        json linesArr = json::array();
        for (const auto &line : med->lines)
        {
            json lj;
            lj["medicineID"] = cleanStr(line.medicineID);
            lj["medicineName"] = cleanStr(line.medicineName);
            lj["quantity"] = line.quantity;
            lj["unitPrice"] = line.unitPrice;
            lj["note"] = cleanStr(line.note);
            linesArr.push_back(lj);
        }
        j["lines"] = linesArr;

        return j;
    }

    json toJson(const Medicine *med)
    {
        json j;
        j["medicineID"] = cleanStr(med->medicineID);
        j["name"] = cleanStr(med->name);
        j["specification"] = cleanStr(med->specification);
        j["manufacturer"] = cleanStr(med->manufacturer);
        j["purchasePrice"] = med->purchasePrice;
        j["salePrice"] = med->salePrice;
        j["stock"] = med->stock;
        j["safetyStock"] = med->safetyStock;
        j["productionDate"] = cleanStr(med->productionDate);
        j["expiryDate"] = cleanStr(med->expiryDate);
        j["department"] = cleanStr(med->department);
        j["isSpecial"] = med->isSpecial;
        j["genericName"] = cleanStr(med->genericName);
        json aliasList = json::array();
        for (const auto &alias : med->aliases)
            aliasList.push_back(alias);
        j["aliases"] = aliasList;
        j["note"] = cleanStr(med->note);
        j["status"] = static_cast<int>(med->status);
        j["statusStr"] = medicineStatusToStr(static_cast<int>(med->status));
        j["isDeleted"] = med->isDeleted;
        return j;
    }

    json toJson(const bedInfo *bed)
    {
        json j;
        j["bedID"] = cleanStr(bed->bedID);
        j["status"] = static_cast<int>(bed->status);
        j["statusStr"] = bedStatusToStr(static_cast<int>(bed->status));
        j["wardType"] = cleanStr(bed->wardType);
        j["department"] = cleanStr(bed->department);
        j["areaNumber"] = bed->areaNumber;
        j["wardNumber"] = bed->wardNumber;
        j["bedNumber"] = bed->bedNumber;
        j["note"] = cleanStr(bed->note);
        j["vitalSigns"] = toJson(bed->vitalSigns);
        j["patientID"] = cleanStr(bed->patientID);
        j["nurseID"] = cleanStr(bed->nurseID);
        j["isDeleted"] = bed->isDeleted;
        j["useTimes"] = bed->useTimes;
        j["daysOccupied"] = bed->daysOccupied;
        return j;
    }

    // ===== 角色用户序列化 =====
    json userToJson(const Admin *admin)
    {
        json j;
        j["userID"] = cleanStr(admin->getUserID());
        j["username"] = cleanStr(admin->getUsername());
        j["role"] = static_cast<int>(admin->getRole());
        j["roleStr"] = "管理员";
        j["gender"] = cleanStr(admin->getGender());
        j["age"] = admin->getAge();
        j["telephone"] = cleanStr(admin->getTelephone());
        j["email"] = cleanStr(admin->getEmail());
        j["createTime"] = cleanStr(admin->getCreateTime());
        j["isAccountActive"] = admin->getIsAccountActive();
        j["isDeleted"] = admin->getIsDeleted();
        return j;
    }

    json userToJson(const Doctor *doc)
    {
        json j;
        j["userID"] = cleanStr(doc->getUserID());
        j["doctorID"] = cleanStr(doc->getDoctorID());
        j["username"] = cleanStr(doc->getUsername());
        j["role"] = static_cast<int>(doc->getRole());
        j["roleStr"] = "医生";
        j["department"] = cleanStr(doc->getDepartment());
        j["title"] = static_cast<int>(doc->getTitle());
        j["titleStr"] = doctorTitleToStr(static_cast<int>(doc->getTitle()));
        j["specialty"] = cleanStr(doc->getSpecialty());
        j["scheduleInfo"] = cleanStr(doc->getScheduleInfo());
        j["consultationCount"] = doc->getConsultationCount();
        j["examinationCount"] = doc->getExaminationCount();
        j["hospitalizationApplyCount"] = doc->getHospitalizationApplyCount();
        j["isOnDuty"] = doc->getIsOnDuty();
        j["gender"] = cleanStr(doc->getGender());
        j["age"] = doc->getAge();
        j["telephone"] = cleanStr(doc->getTelephone());
        j["email"] = cleanStr(doc->getEmail());
        j["createTime"] = cleanStr(doc->getCreateTime());
        j["isAccountActive"] = doc->getIsAccountActive();
        j["isDeleted"] = doc->getIsDeleted();
        return j;
    }

    json userToJson(const Nurse *nurse)
    {
        json j;
        j["userID"] = cleanStr(nurse->getUserID());
        j["nurseID"] = cleanStr(nurse->getNurseID());
        j["username"] = cleanStr(nurse->getUsername());
        j["role"] = static_cast<int>(nurse->getRole());
        j["roleStr"] = "护士";
        j["department"] = cleanStr(nurse->getDepartment());
        j["level"] = static_cast<int>(nurse->getLevel());
        j["scheduleInfo"] = cleanStr(nurse->getScheduleInfo());
        j["patientCareCount"] = nurse->getPatientCareCount();
        j["bedManageCount"] = nurse->getBedManageCount();
        j["isOnDuty"] = nurse->getIsOnDuty();
        j["gender"] = cleanStr(nurse->getGender());
        j["age"] = nurse->getAge();
        j["telephone"] = cleanStr(nurse->getTelephone());
        j["email"] = cleanStr(nurse->getEmail());
        j["createTime"] = cleanStr(nurse->getCreateTime());
        j["isAccountActive"] = nurse->getIsAccountActive();
        j["isDeleted"] = nurse->getIsDeleted();
        return j;
    }

    json userToJson(const Pharmacist *pha)
    {
        json j;
        j["userID"] = cleanStr(pha->getUserID());
        j["pharmacistID"] = cleanStr(pha->getPharmacistID());
        j["username"] = cleanStr(pha->getUsername());
        j["role"] = static_cast<int>(pha->getRole());
        j["roleStr"] = "药剂师";
        j["department"] = cleanStr(pha->getDepartment());
        j["level"] = static_cast<int>(pha->getLevel());
        j["scheduleInfo"] = cleanStr(pha->getScheduleInfo());
        j["reviewCount"] = pha->getReviewCount();
        j["dispenseCount"] = pha->getDispenseCount();
        j["inventoryManageCount"] = pha->getInventoryManageCount();
        j["isOnDuty"] = pha->getIsOnDuty();
        j["gender"] = cleanStr(pha->getGender());
        j["age"] = pha->getAge();
        j["telephone"] = cleanStr(pha->getTelephone());
        j["email"] = cleanStr(pha->getEmail());
        j["createTime"] = cleanStr(pha->getCreateTime());
        j["isAccountActive"] = pha->getIsAccountActive();
        j["isDeleted"] = pha->getIsDeleted();
        return j;
    }

    json userToJson(const Patient *patConst)
    {
        // Patient 的 getter 非 const，需要 const_cast
        Patient *pat = const_cast<Patient*>(patConst);
        json j;
        j["userID"] = cleanStr(pat->getUserID());
        j["patientID"] = cleanStr(pat->getPatientID());
        j["username"] = cleanStr(pat->getUsername());
        j["role"] = static_cast<int>(pat->getRole());
        j["roleStr"] = "患者";
        j["department"] = cleanStr(pat->department);
        j["address"] = cleanStr(pat->address);
        j["idCardNumber"] = cleanStr(pat->idCardNumber);
        j["emergencyContactName"] = cleanStr(pat->emergencyContactName);
        j["emergencyContactPhone"] = cleanStr(pat->emergencyContactPhone);
        j["allergyHistory"] = cleanStr(pat->allergyHistory);
        j["pastMedicalHistory"] = cleanStr(pat->pastMedicalHistory);
        j["maritalStatus"] = static_cast<int>(pat->maritalStatus);
        j["registrationCount"] = pat->registrationCount;
        j["consultationCount"] = pat->consultationCount;
        j["hospitalizationCount"] = pat->hospitalizationCount;
        j["medicationCount"] = pat->medicationCount;
        j["balance"] = pat->balance;
        j["isHospitalized"] = pat->isHospitalized;
        j["gender"] = cleanStr(pat->getGender());
        j["age"] = pat->getAge();
        j["telephone"] = cleanStr(pat->getTelephone());
        j["email"] = cleanStr(pat->getEmail());
        j["createTime"] = cleanStr(pat->getCreateTime());
        j["isAccountActive"] = pat->getIsAccountActive();
        j["isDeleted"] = pat->getIsDeleted();
        return j;
    }

    // ===== 枚举转字符串 =====
    std::string regStatusToStr(int status)
    {
        switch (static_cast<RegistrationStatus>(status))
        {
        case RegistrationStatus::BOOKED: return "已预约";
        case RegistrationStatus::PAID: return "已支付";
        case RegistrationStatus::CANCELED: return "已取消";
        case RegistrationStatus::FINISHED: return "已完成";
        default: return "未知";
        }
    }

    std::string conStatusToStr(int status)
    {
        switch (static_cast<ConsultationStatus>(status))
        {
        case ConsultationStatus::PENDING: return "待就诊";
        case ConsultationStatus::IN_PROGRESS: return "进行中";
        case ConsultationStatus::COMPLETED: return "已完成";
        case ConsultationStatus::VOIDED: return "已作废";
        default: return "未知";
        }
    }

    std::string examStatusToStr(int status)
    {
        switch (static_cast<ExaminationStatus>(status))
        {
        case ExaminationStatus::ORDERED: return "已下单";
        case ExaminationStatus::PAID: return "已支付";
        case ExaminationStatus::IN_PROGRESS: return "检查中";
        case ExaminationStatus::COMPLETED: return "检查完成";
        case ExaminationStatus::REPORTED: return "报告已出";
        case ExaminationStatus::VOIDED: return "已作废";
        default: return "未知";
        }
    }

    std::string hosStatusToStr(int status)
    {
        switch (static_cast<HospitalizationStatus>(status))
        {
        case HospitalizationStatus::APPLIED: return "申请中";
        case HospitalizationStatus::PAID: return "已缴费待分床";
        case HospitalizationStatus::ADMITTED: return "已入院";
        case HospitalizationStatus::DISCHARGED: return "已出院";
        case HospitalizationStatus::VOIDED: return "已作废";
        default: return "未知";
        }
    }

    std::string medStatusToStr(int status)
    {
        switch (static_cast<MedicationStatus>(status))
        {
        case MedicationStatus::UNPAID: return "未缴费";
        case MedicationStatus::PAID: return "已缴费";
        case MedicationStatus::DISPENSED: return "已发药";
        case MedicationStatus::RETURNED: return "已退药";
        default: return "未知";
        }
    }

    std::string medReviewStatusToStr(int status)
    {
        switch (static_cast<MedicationReviewStatus>(status))
        {
        case MedicationReviewStatus::PENDING_REVIEW: return "待审核";
        case MedicationReviewStatus::REJECTED: return "审核拒绝";
        case MedicationReviewStatus::APPROVED: return "审核通过";
        case MedicationReviewStatus::CANCELED: return "已撤销";
        default: return "未知";
        }
    }

    std::string medicineStatusToStr(int status)
    {
        switch (static_cast<MedicineStatus>(status))
        {
        case MedicineStatus::NORMAL: return "正常";
        case MedicineStatus::LOW_STOCK: return "库存不足";
        case MedicineStatus::EXPIRED: return "已过期";
        case MedicineStatus::DISCONTINUED: return "已停用";
        default: return "未知";
        }
    }

    std::string bedStatusToStr(int status)
    {
        switch (static_cast<bedStatus>(status))
        {
        case bedStatus::OCCUPIED: return "已占用";
        case bedStatus::ClEANING: return "清洁中";
        case bedStatus::AVAILABLE: return "可分配";
        case bedStatus::UNAVAILABLE: return "不可用";
        default: return "未知";
        }
    }

    std::string roleToStr(int role)
    {
        switch (static_cast<UserRole>(role))
        {
        case UserRole::ADMIN: return "管理员";
        case UserRole::DOCTOR: return "医生";
        case UserRole::NURSE: return "护士";
        case UserRole::PHARMACIST: return "药剂师";
        case UserRole::PATIENT: return "患者";
        default: return "未知";
        }
    }
}
