
#ifndef DOCTOR_H
#define DOCTOR_H

#include "User.h"

class Doctor : public User
{
public:
    std::string department;     // 医生所属科室（如内科、外科等）
    std::string title;          // 医生职称（如主任医师、副主任医师等）
    std::string specialization; // 医生专业领域（如心血管、消化等）
    bool isDeleted = false;     // 逻辑删除标志（实际删除时设置为 true）

    Doctor *next = nullptr; // 医生链表的下一个节点指针
    Doctor *pre = nullptr;  // 医生链表的前一个节点指针
};
#endif // DOCTOR_H