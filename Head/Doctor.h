
#ifndef DOCTOR_H
#define DOCTOR_H

#include "User.h"

class Doctor : public User
{
public:
    Doctor *next = nullptr; // 医生链表的下一个节点指针
    std::string department; // 医生所属科室（如内科、外科等）

    Doctor();

    Doctor *doctorSignUp(int &idCounter); // 医生注册，调用基类的 signUp(2) 方法，并将医生信息保存到文件中
};
#endif // DOCTOR_H