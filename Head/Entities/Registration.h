/**
 * @file Registration.h
 * @brief 医疗管理信息中的挂号信息结构体
 * @details 该头文件定义了一个 Registration 结构体，用于存储患者的挂号信息，包括挂号ID、患者ID、医生ID、挂号时间和挂号状态等
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <vector>
#include <string>

/**
 * @file Registration.h
 * @brief 挂号信息结构体
 * @details 每条挂号记录为双向链表节点，包含挂号基本信息与状态
 */

enum class RegistrationStatus
{
    BOOKED,   // 已预约(尚未缴费或就诊)
    PAID,     // 已支付(支付成功之后才可以将这条记录链接到挂号链表中)
    CANCELED, // 已取消
    FINISHED  // 已完成（看诊结束）
};

struct Registration
{
    std::string registrationID = "#";                             // 挂号唯一ID
    std::string patientID = "#";                                  // 患者ID（关联Patient）
    std::string department = "#";                                 // 科室名称
    std::string doctorID = "#";                                   // 医生ID（可为空表示未指定）
    std::string registerTime = "#";                               // 挂号时间戳（字符串格式）
    int fee = 0;                                       // 挂号费用（单位：分）
    RegistrationStatus status = RegistrationStatus::BOOKED; // 挂号状态
    std::string note = "#";                                       // 备注信息

    // 关联之前的挂号记录（如同一患者多次挂号）
    std::vector<std::string> relatedRegistrationIDs; // 相关挂号记录ID列表

    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）

    Registration *prev = nullptr; // 链表前向指针
    Registration *next = nullptr; // 链表后向指针
};


#endif // REGISTRATION_H
