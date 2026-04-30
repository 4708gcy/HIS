#ifndef API_SERVER_H
#define API_SERVER_H

#include "httplib.h"
#include "json.hpp"
#include "../Head/User.h"
#include "../Head/Admin.h"
#include "../Head/Doctor.h"
#include "../Head/Nurse.h"
#include "../Head/Pharmacist.h"
#include "../Head/Patient.h"
#include "../Head/ApiResponse.h"
#include "../Head/JsonHelper.h"
#include "../Head/JWTAuth.h"
#include <mutex>

using json = nlohmann::json;

// 全局数据管理器（线程安全）
class DataManager
{
public:
    static DataManager &getInstance();

    // 初始化：加载所有数据
    void init();

    // 保存所有数据到文件（带锁，供外部无锁上下文调用）
    void saveAll();
    // 保存所有数据到文件（不加锁，调用方必须已持有 mtx）
    void saveAllUnsafe();

    // 获取数据头指针（可修改，用于链表头插入）
    Admin *&getAdminHead() { return adminHead; }
    Doctor *&getDoctorHead() { return docHead; }
    Nurse *&getNurseHead() { return nurseHead; }
    Pharmacist *&getPharmacistHead() { return phaHead; }
    Patient *&getPatientHead() { return patientHead; }
    Registration *&getRegHead() { return regHead; }
    Consultation *&getConHead() { return conHead; }
    Examination *&getExamHead() { return examHead; }
    Hospitalization *&getHosHead() { return hosHead; }
    MedicationRecord *&getMedRecHead() { return medRecHead; }
    Medicine *&getMedHead() { return medHead; }
    MedicineFlow *&getMedFlowHead() { return medFlowHead; }
    bedInfo *&getBedHead() { return bedHead; }

    // ID 计数器
    int &adminIDCount() { return adminIDCount_; }
    int &doctorIDCount() { return doctorIDCount_; }
    int &nurseIDCount() { return nurseIDCount_; }
    int &pharmacistIDCount() { return pharmacistIDCount_; }
    int &patientIDCount() { return patientIDCount_; }
    int &registrationCount() { return registrationCount_; }
    int &consultationCount() { return consultationCount_; }
    int &examinationCount() { return examinationCount_; }
    int &hospitalizationCount() { return hospitalizationCount_; }
    int &medicationRecordCount() { return medicationRecordCount_; }
    int &medicineCount() { return medicineCount_; }
    int &medicineFlowCount() { return medicineFlowCount_; }
    int &bedCount() { return bedCount_; }

    std::mutex &getMutex() { return mtx; }

private:
    DataManager() = default;
    ~DataManager();

    Admin *adminHead = nullptr;
    Doctor *docHead = nullptr;
    Nurse *nurseHead = nullptr;
    Pharmacist *phaHead = nullptr;
    Patient *patientHead = nullptr;
    Registration *regHead = nullptr;
    Consultation *conHead = nullptr;
    Examination *examHead = nullptr;
    Hospitalization *hosHead = nullptr;
    MedicationRecord *medRecHead = nullptr;
    Medicine *medHead = nullptr;
    MedicineFlow *medFlowHead = nullptr;
    bedInfo *bedHead = nullptr;

    int adminIDCount_ = 0;
    int doctorIDCount_ = 0;
    int nurseIDCount_ = 0;
    int pharmacistIDCount_ = 0;
    int patientIDCount_ = 0;
    int registrationCount_ = 0;
    int consultationCount_ = 0;
    int examinationCount_ = 0;
    int hospitalizationCount_ = 0;
    int medicationRecordCount_ = 0;
    int medicineCount_ = 0;
    int medicineFlowCount_ = 0;
    int bedCount_ = 0;

    std::mutex mtx;
};

// 注册所有 API 路由
void registerApiRoutes(httplib::Server &svr);

// 认证中间件：验证 JWT 并注入用户信息
struct AuthResult
{
    bool valid = false;
    std::string userID;
    int role = 0;
};
AuthResult authenticateRequest(const httplib::Request &req);

#endif
