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

#include <string>

/**
 * @brief 挂号状态枚举类
 * 定义了挂号记录可能的状态
 */
enum class RegistrationStatus
{
    BOOKED,   /**< 已预约状态 */
    PAID,     /**< 已支付状态 */
    CANCELED, /**< 已取消状态 */
    FINISHED  /**< 已完成状态 */
};

/**
 * @brief 挂号信息结构体
 * 存储患者挂号的详细信息，包括基本信息、费用、状态等，并支持双向链表操作
 */
struct Registration
{
    std::string registrationID;                             /**< 挂号ID */
    std::string patientID;                                  /**< 患者ID */
    std::string department;                                 /**< 科室名称 */
    std::string doctorID;                                   /**< 医生ID */
    std::string registerTime;                               /**< 挂号时间 */
    double fee = 0.0;                                       /**< 挂号费用，默认为0.0 */
    RegistrationStatus status = RegistrationStatus::BOOKED; /**< 挂号状态，默认为已预约 */
    std::string note;                                       /**< 备注信息 */

    Registration *prev = nullptr; /**< 指向前一个挂号记录的指针，用于构建双向链表 */
    Registration *next = nullptr; /**< 指向后一个挂号记录的指针，用于构建双向链表 */
};
#endif