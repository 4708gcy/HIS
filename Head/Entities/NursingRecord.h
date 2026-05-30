#ifndef NURSING_RECORD_H
#define NURSING_RECORD_H

#include <string>

/**
 * @brief 护理记录结构体
 */
struct NursingRecord
{
    std::string recordID;           // 格式 "nur" + 6位数字
    std::string patientID;          // 所属患者
    std::string nurseID;            // 执行护士
    std::string department;         // 执行科室
    std::string operationTime;      // 操作时间
    int operationType = 1;          // 操作类型: 1=生命体征测量 2=静脉输液 3=肌肉注射 4=口服给药 5=换药 6=翻身拍背 7=导尿 8=其他
    std::string detail;             // 操作详情
    std::string vitalSignsSnapshot; // 生命体征快照（分号分隔）
    std::string note;               // 备注
    bool isDeleted = false;
    NursingRecord *prev = nullptr;
    NursingRecord *next = nullptr;

    NursingRecord() : operationType(1), isDeleted(false), prev(nullptr), next(nullptr) {}
};

#endif // NURSING_RECORD_H
