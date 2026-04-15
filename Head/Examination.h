/**
 * @file Examination.h
 * @brief 医疗管理系统中的检查信息结构体
 * @details 该头文件定义了一个 Examination 结构体，用于存储患者的检查信息，包括检查ID、患者ID、医生ID、检查时间、检查结果等
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef EXAMINATION_H
#define EXAMINATION_H

#include <string>
#include <vector>

/**
 * @file Examination.h
 * @brief 检查/检验记录结构体
 * @details 包括检查项目、下单时间、报告时间与报告摘要
 */

enum class ExaminationStatus
{
    ORDERED,     // 已下单
    PAID,        // 已支付
    IN_PROGRESS, // 检查中
    COMPLETED,   // 检查完成
    REPORTED,    // 报告已出
    VOIDED       // 已作废
};

// 生命体征：记录检查时的瞬时体征（如测量时采集）

/*
检查项目价格参考表（单位：元）

| 体温测量         | temperatureC               |   5     
| 血压测量         | systolicBP/diastolicBP     |   8             
| 心率测量/脉搏    | heartRate                  |   5               
| 呼吸频率测量     | respiratoryRate            |   5              
| 血氧饱和度       | spo2                       |  10            
| 身高测量         | height                     |   5               
| 体重测量         | weight                     |   5               
| BMI计算          | bmi                        |   5     
| 疼痛评分         | painScore                  |   2      
| 腰围测量         | waistCircumference         |   5               
| 血糖测量         | bloodSugar                 |  20     
| 体脂率测量       | bodyFat                    |  30     
| 尿酸测定         | uricAcid                   |  25             
| 总胆固醇         | cholesterol                |  25              
*/

struct VitalSigns
{
    double temperatureC;       // 体温（摄氏度）——体温测量
    double systolicBP;         // 收缩压（mmHg）——血压测量
    double diastolicBP;        // 舒张压（mmHg）——血压测量
    double heartRate;          // 心率（次/分钟）——心率测量/脉搏
    double respiratoryRate;    // 呼吸频率（次/分钟）——呼吸频率测量
    double spo2;               // 血氧饱和度（%）——脉搏血氧测量
    double height;             // 身高（cm）——身高测量
    double weight;             // 体重（kg）——体重测量
    double bmi;                // 体质指数——BMI计算
    double painScore;          // 疼痛评分（0-10）——疼痛评估
    double waistCircumference; // 腰围（cm）——腰围测量
    double bloodSugar;         // 血糖（mmol/L）——血糖测量
    double bodyFat;            // 体脂率（%）——体脂测量
    double uricAcid;           // 尿酸（μmol/L）——尿酸测定
    double cholesterol;        // 总胆固醇（mmol/L）——血脂测定
    // 可根据实际需求继续扩展
};

struct Examination
{
    std::string examinationID;  // 检查记录唯一ID
    std::string consultationID; // 关联看诊记录ID
    std::string patientID;      // 患者ID
    std::string doctorID;       // 开单医生ID
    std::string department;     // 科室名称

    std::string itemName;      // 检查项目名称
    std::string orderTime;     // 下单时间
    std::string reportTime;    // 报告时间（若已出）
    std::string reportSummary; // 报告摘要或结果文本

    VitalSigns vitalSigns; // 生命体征（可选，视检查类型而定）

    // 新增用于计费/支付的字段
    double fee = 0.0; // 该检查的费用

    ExaminationStatus status = ExaminationStatus::ORDERED;

    // 附件和备注
    std::vector<std::string> attachments; // 检查相关的附件文件路径列表
    std::string note;                     // 医生备注或特殊说明

    // 关联之前的检查记录（如同一看诊中多次检查）
    std::vector<std::string> relatedExaminationIDs; // 相关检查记录ID列表

    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）

    Examination *prev = nullptr;
    Examination *next = nullptr;
};

#endif // EXAMINATION_H