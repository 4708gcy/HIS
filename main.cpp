#include <iostream>
#include "Head/UI.h"
#include "Head/LoadData.h"
#include "Head/SaveData.h"
#include "Head/Login.h"

#ifdef _WIN32
#include <windows.h>
#endif

// 记录各个人物已经使用的ID数量，用于生成新的唯一ID
int adminIDCount = 0;
int doctorIDCount = 0;
int nurseIDCount = 0;
int pharmacistIDCount = 0;
int patientIDCount = 0;

// 记录医疗记录的数量，用于生成新的唯一ID
int registrationCount = 0;
int consultationCount = 0;
int examinationCount = 0;
int hospitalizationCount = 0;
int medicationRecordCount = 0;
int medicineCount = 0;
int bedCount = 0;

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    Admin *adminHead = loadAdminData(adminIDCount); // 加载管理员数据
    if (adminHead == nullptr)
    {
        std::cout << "没有管理员数据，请先创建管理员账号，否则系统无法启动！" << std::endl;
        Admin *newAdmin = new Admin();
        if (newAdmin->adminSignUp(adminIDCount)) // 注册新管理员并更新ID计数器
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

    Doctor *docHead = loadDoctorData(doctorIDCount);             // 加载医生数据
    Nurse *nurseHead = loadNurseData(nurseIDCount);              // 加载护士数据
    Pharmacist *phaHead = loadPharmacistData(pharmacistIDCount); // 加载药师数据
    Patient *patientHead = loadPatientData(patientIDCount);      // 加载患者数据

    Registration *regHead = loadRegistrations(registrationCount);                // 加载挂号记录数据
    Consultation *conHead = loadConsultations(consultationCount);                // 加载看诊记录数据
    Examination *examHead = loadExaminations(examinationCount);                  // 加载检查记录数据
    Hospitalization *hosHead = loadHospitalizations(hospitalizationCount);       // 加载住院记录数据
    MedicationRecord *medRecHead = loadMedicationRecords(medicationRecordCount); // 加载用药记录数据
    Medicine *medHead = loadMedicines(medicineCount);                            // 加载药品信息数据
    bedInfo *bedHead = loadBedInfos(bedCount);                                   // 加载床位信息数据

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
                                while (true) // 科室选择循环，直到用户选择返回上级菜单
                                {
                                    std::string department = adminDepartmentMenu();
                                    if (department == "内科")
                                    {
                                        while (true)
                                        {
                                            int userChoice = adminUserManagementMenu();
                                            if (userChoice == 1)
                                            {
                                                client->manageDoctors(docHead, department, doctorIDCount);
                                            }
                                            else if (userChoice == 2)
                                            {
                                                client->manageNurses(nurseHead, department, nurseIDCount);
                                            }
                                            else if (userChoice == 3)
                                            {
                                                client->managePharmacists(phaHead, department, pharmacistIDCount);
                                            }
                                            else if (userChoice == 4)
                                            {
                                                client->managePatients(patientHead, department, patientIDCount);
                                            }
                                            else if (userChoice == 0)
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
                                        while (true)
                                        {
                                            int userChoice = adminUserManagementMenu();
                                            if (userChoice == 1)
                                            {
                                                client->manageDoctors(docHead, department, doctorIDCount);
                                            }
                                            else if (userChoice == 2)
                                            {
                                                client->manageNurses(nurseHead, department, nurseIDCount);
                                            }
                                            else if (userChoice == 3)
                                            {
                                                client->managePharmacists(phaHead, department, pharmacistIDCount);
                                            }
                                            else if (userChoice == 4)
                                            {
                                                client->managePatients(patientHead, department, patientIDCount);
                                            }
                                            else if (userChoice == 0)
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
                                        while (true)
                                        {
                                            int userChoice = adminUserManagementMenu();
                                            if (userChoice == 1)
                                            {
                                                client->manageDoctors(docHead, department, doctorIDCount);
                                            }
                                            else if (userChoice == 2)
                                            {
                                                client->manageNurses(nurseHead, department, nurseIDCount);
                                            }
                                            else if (userChoice == 3)
                                            {
                                                client->managePharmacists(phaHead, department, pharmacistIDCount);
                                            }
                                            else if (userChoice == 4)
                                            {
                                                client->managePatients(patientHead, department, patientIDCount);
                                            }
                                            else if (userChoice == 0)
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
                                        while (true)
                                        {
                                            int userChoice = adminUserManagementMenu();
                                            if (userChoice == 1)
                                            {
                                                client->manageDoctors(docHead, department, doctorIDCount);
                                            }
                                            else if (userChoice == 2)
                                            {
                                                client->manageNurses(nurseHead, department, nurseIDCount);
                                            }
                                            else if (userChoice == 3)
                                            {
                                                client->managePharmacists(phaHead, department, pharmacistIDCount);
                                            }
                                            else if (userChoice == 4)
                                            {
                                                client->managePatients(patientHead, department, patientIDCount);
                                            }
                                            else if (userChoice == 0)
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
                                        while (true)
                                        {
                                            int userChoice = adminUserManagementMenu();
                                            if (userChoice == 1)
                                            {
                                                client->manageDoctors(docHead, department, doctorIDCount);
                                            }
                                            else if (userChoice == 2)
                                            {
                                                client->manageNurses(nurseHead, department, nurseIDCount);
                                            }
                                            else if (userChoice == 3)
                                            {
                                                client->managePharmacists(phaHead, department, pharmacistIDCount);
                                            }
                                            else if (userChoice == 4)
                                            {
                                                client->managePatients(patientHead, department, patientIDCount);
                                            }
                                            else if (userChoice == 0)
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
                                        break;
                                    }
                                }
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
                                                client->manageRegistrations(regHead, docHead, department, registrationCount);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department, regHead, consultationCount);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department, conHead, examinationCount);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, nurseHead, department, conHead, bedHead, hospitalizationCount);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, conHead, phaHead, medHead, department, medicationRecordCount);
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
                                                client->manageRegistrations(regHead, docHead, department, registrationCount);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department, regHead, consultationCount);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department, conHead, examinationCount);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, nurseHead, department, conHead, bedHead, hospitalizationCount);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, conHead, phaHead, medHead, department, medicationRecordCount);
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
                                                client->manageRegistrations(regHead, docHead, department, registrationCount);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department, regHead, consultationCount);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department, conHead, examinationCount);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, nurseHead, department, conHead, bedHead, hospitalizationCount);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, conHead, phaHead, medHead, department, medicationRecordCount);
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
                                                client->manageRegistrations(regHead, docHead, department, registrationCount);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department, regHead, consultationCount);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department, conHead, examinationCount);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, nurseHead, department, conHead, bedHead, hospitalizationCount);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, conHead, phaHead, medHead, department, medicationRecordCount);
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
                                                client->manageRegistrations(regHead, docHead, department, registrationCount);
                                            }
                                            else if (recordChoice == 2)
                                            {
                                                client->manageConsultations(conHead, department, regHead, consultationCount);
                                            }
                                            else if (recordChoice == 3)
                                            {
                                                client->manageExaminations(examHead, department, conHead, examinationCount);
                                            }
                                            else if (recordChoice == 4)
                                            {
                                                client->manageHospitalizations(hosHead, nurseHead, department, conHead, bedHead, hospitalizationCount);
                                            }
                                            else if (recordChoice == 5)
                                            {
                                                client->manageMedicationRecords(medRecHead, conHead, phaHead, medHead, department, medicationRecordCount);
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
                                while (true) // 科室选择循环，直到用户选择返回上级菜单
                                {
                                    std::string department = adminDepartmentMenu();
                                    if (department == "内科")
                                    {
                                        client->manageMedicines(medHead, department, medicineCount);
                                    }
                                    else if (department == "外科")
                                    {
                                        client->manageMedicines(medHead, department, medicineCount);
                                    }
                                    else if (department == "妇产科")
                                    {
                                        client->manageMedicines(medHead, department, medicineCount);
                                    }
                                    else if (department == "急诊科")
                                    {
                                        client->manageMedicines(medHead, department, medicineCount);
                                    }
                                    else if (department == "儿科")
                                    {
                                        client->manageMedicines(medHead, department, medicineCount);
                                    }
                                    else if (department == "0")
                                    {
                                        // 返回上级菜单
                                    }
                                    else
                                    {
                                        std::cout << "无效的选择! 请重新选择。" << std::endl;
                                    }
                                }
                            }
                            else if (adminChoice == 4) // 床位管理
                            {
                                while (true) // 科室选择循环，直到用户选择返回上级菜单
                                {
                                    std::string department = adminDepartmentMenu();
                                    if (department == "内科")
                                    {
                                        client->manageBedInfo(bedHead, hosHead, department);
                                    }
                                    else if (department == "外科")
                                    {
                                        client->manageBedInfo(bedHead, hosHead, department);
                                    }
                                    else if (department == "妇产科")
                                    {
                                        client->manageBedInfo(bedHead, hosHead, department);
                                    }
                                    else if (department == "急诊科")
                                    {
                                        client->manageBedInfo(bedHead, hosHead, department);
                                    }
                                    else if (department == "儿科")
                                    {
                                        client->manageBedInfo(bedHead, hosHead, department);
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
                            else if (adminChoice == 5)
                            {
                                manageAdmins(adminHead, adminIDCount); // 管理管理员信息
                            }
                            else if (adminChoice == 6) // 个人信息管理
                            {
                                client->AdminPersionalInfo(); // 调用管理员个人信息管理函数
                            }
                            else if (adminChoice == 7) // 账号封锁管理
                            {
                                AccountManagement(adminHead, docHead, nurseHead, phaHead, patientHead); // 调用账号激活/封锁管理函数
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
                    Doctor *client = doctorLogin(docHead);
                    if (client)
                    {
                        while (true)
                        {
                            int doctorChoice = doctorMenu();
                            if (doctorChoice == 0)
                            {
                                break; // 退出登录
                            }
                            else if (doctorChoice == 1)
                            {
                                client->manageRegistrations(regHead, docHead, patientHead, registrationCount); // 管理挂号记录
                            }
                            else if (doctorChoice == 2)
                            {
                                client->manageConsultations(conHead, regHead, medHead, consultationCount); // 管理看诊记录
                            }
                            else if (doctorChoice == 3)
                            {
                                client->manageExaminations(examHead, conHead, examinationCount); // 管理检查记录
                            }
                            else if (doctorChoice == 4)
                            {
                                client->managePersonalInfo(); // 个人信息管理
                            }
                        }
                    }
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

    saveAdminData(adminHead, adminIDCount);         // 保存管理员数据
    saveDoctorData(docHead, doctorIDCount);         // 保存医生数据
    saveNurseData(nurseHead, nurseIDCount);         // 保存护士数据
    savePharmacistData(phaHead, pharmacistIDCount); // 保存药剂师数据
    savePatientData(patientHead, patientIDCount);   // 保存患者数据

    saveRegistrations(regHead, registrationCount);            // 保存挂号记录数据
    saveConsultations(conHead, consultationCount);            // 保存看诊记录数据
    saveExaminations(examHead, examinationCount);             // 保存检查记录数据
    saveHospitalizations(hosHead, hospitalizationCount);      // 保存住院记录数据
    saveMedicationRecords(medRecHead, medicationRecordCount); // 保存用药记录数据
    saveMedicines(medHead, medicineCount);                    // 保存药品信息数据
    saveBedInfos(bedHead, bedCount);                          // 保存床位信息数据
}
