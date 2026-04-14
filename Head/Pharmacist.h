/** 
 * @file Pharmacist.h
 * @brief 药师类的定义
 * @details 该头文件定义了一个 Pharmacist 类，继承自 User 类，包含药师特有的功能和药品库存管理方法
 * @author 郭承宇
 * @date 2026-4-14
 * @version 1.0
 */

#ifndef PHARMACIST_H
#define PHARMACIST_H

#include "User.h"

class Pharmacist : public User
{
public:
    std::string department; // 所属科室
    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）
    Pharmacist *next = nullptr; // 药师链表的下一个节点指针
    Pharmacist *pre = nullptr;  // 药师链表的前一个节点指针
};

#endif // PHARMACIST_H