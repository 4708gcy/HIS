#include <iostream>
#include "Head/UI.h"
#include "Head/LoadData.h"
#include "Head/SaveData.h"
#include "Head/Login.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    Admin *adminHead = loadAdminData(); // 加载管理员数据
    if (adminHead == nullptr)
    {
        std::cout << "没有管理员数据，请先创建管理员账号，否则系统无法启动！" << std::endl;
        Admin *newAdmin = new Admin();
        if (newAdmin->adminSignUp())
        {
            adminHead = newAdmin;
        }
        else
        {
            delete newAdmin;
            std::cout << "管理员注册失败，系统无法启动！" << std::endl;
            return 1; // 退出程序
        }
    }

    Registration *regHead = loadRegistrations();            // 加载挂号记录数据
    Consultation *conHead = loadConsultations();            // 加载看诊记录数据
    Examination *examHead = loadExaminations();             // 加载检查记录数据
    Hospitalization *hosHead = loadHospitalizations();      // 加载住院记录数据
    MedicationRecord *medRecHead = loadMedicationRecords(); // 加载用药记录数据
    Medicine *medHead = loadMedicines();                    // 加载药品信息数据

    while (true) // 系统主循环，处理登录和注册逻辑
    {
        int loginChoice = loginRegisterUI();

        if (loginChoice == 1)
        {
            while (true) // 身份选择界面循环，直到用户选择退出
            {
                int roleChoice = beginUI();
                if (roleChoice == 1) // 管理员登录
                {
                    Admin *client = adminLogin(adminHead);
                    if (client)
                    {
                        while (true) // 管理员功能菜单循环，直到用户选择退出
                        {
                            int adminChoice = adminMenu();
                            if (adminChoice == 1) // 账户管理
                            {
                            }
                            else if (adminChoice == 2) // 医疗记录管理
                            {
                                while (true) // 科室选择循环，直到用户选择返回上级菜单
                                {
                                    std::string department = adminDepartmentMenu();
                                    if (department == "内科")
                                    {
                                        while (true) // 医疗记录管理菜单循环，直到用户选择返回上级菜单
                                        {
                                            int recordChoice = adminMedicalRecordMenu();
                                            if (recordChoice == 1)
                                            {
                                                client->manageRegistrations(regHead, department);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, department);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, department);
                                            }
                                            else if (recordChoice == 0)
                                            {
                                                break; // 返回上级菜单
                                            }
                                            else
                                            {
                                                std::cout << "无效的选择! 请重新选择。" << std::endl;
                                            }
                                        }
                                    }
                                    else if (department == "外科")
                                    {
                                        while (true) // 医疗记录管理菜单循环，直到用户选择返回上级菜单
                                        {
                                            int recordChoice = adminMedicalRecordMenu();
                                            if (recordChoice == 1)
                                            {
                                                client->manageRegistrations(regHead, department);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, department);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, department);
                                            }
                                            else if (recordChoice == 0)
                                            {
                                                break; // 返回上级菜单
                                            }
                                            else
                                            {
                                                std::cout << "无效的选择! 请重新选择。" << std::endl;
                                            }
                                        }
                                    }
                                    else if (department == "妇产科")
                                    {
                                        while (true) // 医疗记录管理菜单循环，直到用户选择返回上级菜单
                                        {
                                            int recordChoice = adminMedicalRecordMenu();
                                            if (recordChoice == 1)
                                            {
                                                client->manageRegistrations(regHead, department);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, department);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, department);
                                            }
                                            else if (recordChoice == 0)
                                            {
                                                break; // 返回上级菜单
                                            }
                                            else
                                            {
                                                std::cout << "无效的选择! 请重新选择。" << std::endl;
                                            }
                                        }
                                    }
                                    else if (department == "急诊科")
                                    {
                                        while (true) // 医疗记录管理菜单循环，直到用户选择返回上级菜单
                                        {
                                            int recordChoice = adminMedicalRecordMenu();
                                            if (recordChoice == 1)
                                            {
                                                client->manageRegistrations(regHead, department);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, department);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, department);
                                            }
                                            else if (recordChoice == 0)
                                            {
                                                break; // 返回上级菜单
                                            }
                                            else
                                            {
                                                std::cout << "无效的选择! 请重新选择。" << std::endl;
                                            }
                                        }
                                    }
                                    else if (department == "儿科")
                                    {
                                        while (true) // 医疗记录管理菜单循环，直到用户选择返回上级菜单
                                        {
                                            int recordChoice = adminMedicalRecordMenu();
                                            if (recordChoice == 1)
                                            {
                                                client->manageRegistrations(regHead, department);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, department);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, department);
                                            }
                                            else if (recordChoice == 0)
                                            {
                                                break; // 返回上级菜单
                                            }
                                            else
                                            {
                                                std::cout << "无效的选择! 请重新选择。" << std::endl;
                                            }
                                        }
                                    }
                                    else if (department == "0")
                                    {
                                        break; // 返回上级菜单
                                    }
                                    else
                                    {
                                        std::cout << "无效的选择! 请重新选择。" << std::endl;
                                    }
                                }
                            }
                            else if (adminChoice == 3) // 药品管理
                            {
                            }
                            else if (adminChoice == 0) // 退出登录
                            {
                                std::cout << "成功退出登录" << std::endl;
                                break;
                            }
                        }
                    }
                    pause();
                }
                else if (roleChoice == 2) // 医生登录
                {
                    std::cout << "医生登录功能尚未实现，敬请期待！" << std::endl;
                }
                else if (roleChoice == 3) // 护士登录
                {
                    std::cout << "护士登录功能尚未实现，敬请期待！" << std::endl;
                }
                else if (roleChoice == 4) // 药剂师登录
                {
                    std::cout << "药剂师登录功能尚未实现，敬请期待！" << std::endl;
                }
                else if (roleChoice == 5) // 患者登录
                {
                    std::cout << "患者登录功能尚未实现，敬请期待！" << std::endl;
                }
                else if (roleChoice == 0) // 返回上一级
                {
                    break; // 返回到登录注册选择界面
                }
                else
                {
                    std::cout << "无效的选择! 请重新选择。" << std::endl;
                }
            }
        }
        else if (loginChoice == 2)
        {
            std::cout << "请先登录管理员账号进行注册操作！" << std::endl;
            pause();
        }
        else
        {
            std::cout << "感谢使用HIS系统，再见！" << std::endl;
            break;
        }
    }

    saveAdminData(adminHead); // 保存管理员数据
}
