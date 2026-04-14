/**
 * @file Nurse.h
 * @brief 医疗管理系统中的护士结构体定义
 * @details 该头文件定义了一个 Nurse 结构体，用于存储护士的基本信息，包括护士ID、姓名、性别、年龄、联系方式和所属科室等
 * @author 郭承宇
 * @date 2026-4-14
 * @version 1.0
 */

#ifndef NURSE_H
#define NURSE_H

#include "User.h"

class Nurse : public User
{
public:
    std::string department; // 所属科室


    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）

    Nurse *next = nullptr; // 护士链表的下一个节点指针
    Nurse *pre = nullptr;  // 护士链表的前一个节点指针
};

#endif // NURSE_H