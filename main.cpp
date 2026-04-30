#include <iostream>
#include "Head/UI.h"
#include "Head/LoadData.h"
#include "Head/SaveData.h"
#include "Head/Login.h"
#include <mutex>
#include <csignal>

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
int medicineFlowIDCount = 0;

// 全局指针用于信号处理中的紧急保存
static Admin *g_adminHead = nullptr;
static Doctor *g_docHead = nullptr;
static Nurse *g_nurseHead = nullptr;
static Pharmacist *g_phaHead = nullptr;
static Patient *g_patientHead = nullptr;
static Registration *g_regHead = nullptr;
static Consultation *g_conHead = nullptr;
static Examination *g_examHead = nullptr;
static Hospitalization *g_hosHead = nullptr;
static MedicationRecord *g_medRecHead = nullptr;
static Medicine *g_medHead = nullptr;
static MedicineFlow *g_medFlowHead = nullptr;
static bedInfo *g_bedHead = nullptr;

void emergencySave()
{
    std::cerr << "\n正在紧急保存数据..." << std::endl;
    if (g_adminHead) saveAdminData(g_adminHead, adminIDCount);
    if (g_docHead) saveDoctorData(g_docHead, doctorIDCount);
    if (g_nurseHead) saveNurseData(g_nurseHead, nurseIDCount);
    if (g_phaHead) savePharmacistData(g_phaHead, pharmacistIDCount);
    if (g_patientHead) savePatientData(g_patientHead, patientIDCount);
    if (g_regHead) saveRegistrations(g_regHead, registrationCount);
    if (g_conHead) saveConsultations(g_conHead, consultationCount);
    if (g_examHead) saveExaminations(g_examHead, examinationCount);
    if (g_hosHead) saveHospitalizations(g_hosHead, hospitalizationCount);
    if (g_medRecHead) saveMedicationRecords(g_medRecHead, medicationRecordCount);
    if (g_medHead) saveMedicines(g_medHead, medicineCount);
    if (g_medFlowHead) saveMedicineFlows(g_medFlowHead, medicineFlowIDCount);
    if (g_bedHead) saveBedInfos(g_bedHead, bedCount);
    std::cerr << "数据已紧急保存，程序退出。" << std::endl;
}

void signalHandler(int signum)
{
    emergencySave();
    std::_Exit(signum);
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::signal(SIGINT, signalHandler);
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGTERM, signalHandler);

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
    MedicineFlow *medFlowHead = loadMedicineFlows(medicineFlowIDCount);         // 加载药品流水记录
    bedInfo *bedHead = loadBedInfos(bedCount);                                   // 加载床位信息数据

    // 设置全局指针用于信号处理中的紧急保存
    g_adminHead = adminHead; g_docHead = docHead; g_nurseHead = nurseHead;
    g_phaHead = phaHead; g_patientHead = patientHead; g_regHead = regHead;
    g_conHead = conHead; g_examHead = examHead; g_hosHead = hosHead;
    g_medRecHead = medRecHead; g_medHead = medHead; g_medFlowHead = medFlowHead; g_bedHead = bedHead;

    try
    {

    while (true) // 系统主循环，处理登录和注册逻辑
    {
        int loginChoice = loginRegisterUI();

        if (loginChoice == 1) // 登录
        {
            while (true) // 身份选择界面循环，直到用户选择退出
            {
                int roleChoice = beginUI();
                if (roleChoice == 1) // 管理员登录
                {
                    Admin *client = adminLogin(adminHead);
                    if (client)
                    {
                        LogManager::getInstance().logOperation(client->getUserID(), "管理员", "登录", "管理员登录成功");
                        while (true) // 管理员功能菜单循环，直到用户选择退出
                        {
                            int adminChoice = adminMenu();
                            if (adminChoice == 1) // 账户管理
                            {
                                std::string department = adminDepartmentMenu();
                                if (department == "0") continue;
                                while (true)
                                {
                                        int userChoice = adminUserManagementMenu();
                                        if (userChoice == 1) client->manageDoctors(docHead, department, doctorIDCount);
                                        else if (userChoice == 2) client->manageNurses(nurseHead, department, nurseIDCount);
                                        else if (userChoice == 3) client->managePharmacists(phaHead, department, pharmacistIDCount);
                                        else if (userChoice == 4) client->managePatients(patientHead, department, patientIDCount);
                                        else if (userChoice == 0) break;
                                        else std::cout << "无效的选择! 请重新选择。" << std::endl;
                                    }
                            }
                            else if (adminChoice == 2) // 医疗记录管理
                            {
                                std::string department = adminDepartmentMenu();
                                if (department == "0") continue;
                                while (true)
                                    {
                                        int recordChoice = adminMedicalRecordMenu();
                                        if (recordChoice == 1) client->manageRegistrations(regHead, docHead, department, registrationCount);
                                        else if (recordChoice == 2) client->manageConsultations(conHead, department, regHead, consultationCount);
                                        else if (recordChoice == 3) client->manageExaminations(examHead, department, conHead, examinationCount);
                                        else if (recordChoice == 4) client->manageHospitalizations(hosHead, nurseHead, department, conHead, bedHead, hospitalizationCount);
                                        else if (recordChoice == 5) client->manageMedicationRecords(medRecHead, conHead, phaHead, medHead, department, medicationRecordCount);
                                        else if (recordChoice == 0) break;
                                        else std::cout << "无效的选择! 请重新选择。" << std::endl;
                                    }
                            }
                            else if (adminChoice == 3) // 药品管理
                            {
                                std::string department = adminDepartmentMenu();
                                if (department == "0") continue;
                                client->manageMedicines(medHead, medFlowHead, department, medicineCount, medicineFlowIDCount);
                            }
                            else if (adminChoice == 4) // 床位管理
                            {
                                std::string department = adminDepartmentMenu();
                                if (department == "0") continue;
                                client->manageBedInfo(bedHead, hosHead, department);
                            }
                            else if (adminChoice == 5)
                            {
                                manageAdmins(adminHead, adminIDCount); // 管理管理员信息
                            }
                            else if (adminChoice == 6) // 个人信息管理
                            {
                                client->AdminPersonalInfo(); // 调用管理员个人信息管理函数
                            }
                            else if (adminChoice == 7) // 账号封锁管理
                            {
                                AccountManagement(adminHead, docHead, nurseHead, phaHead, patientHead); // 调用账号激活/封锁管理函数
                            }
                            else if (adminChoice == 8) // 统计报表
                            {
                                while (true)
                                {
                                    int reportChoice = adminReportMenu();
                                    if (reportChoice == 1) client->showDepartmentReport(docHead, regHead, conHead);
                                    else if (reportChoice == 2) client->showDoctorWorkloadReport(docHead);
                                    else if (reportChoice == 3) client->showPatientReport(patientHead, regHead, conHead);
                                    else if (reportChoice == 4) client->showBedUtilizationReport(bedHead, hosHead);
                                    else if (reportChoice == 5) client->showMedicineInventoryReport(medHead, medFlowHead);
                                    else if (reportChoice == 6) client->showDataAnalysisReport(hosHead, bedHead, regHead, medHead, medFlowHead);
                                    else if (reportChoice == 0) break;
                                }
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
                        LogManager::getInstance().logOperation(client->getUserID(), "医生", "登录", "医生登录成功");
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
                    pause();
                }
                else if (roleChoice == 3) // 护士登录
                {
                    Nurse *client = nurseLogin(nurseHead);
                    if (client)
                    {
                        LogManager::getInstance().logOperation(client->getUserID(), "护士", "登录", "护士登录成功");
                        while (true)
                        {
                            int nurseChoice = nurseMenu();
                            if (nurseChoice == 0)
                            {
                                break; // 退出登录
                            }
                            else if (nurseChoice == 1)
                            {
                                client->manageHospitalizations(hosHead, conHead, bedHead, hospitalizationCount); // 管理住院记录和床位信息
                            }
                            else if (nurseChoice == 2)
                            {
                                client->manageExaminations(examHead); // 管理检查记录
                            }
                            else if (nurseChoice == 3)
                            {
                                client->manageBeds(bedHead, bedCount); // 管理床位信息
                            }
                            else if (nurseChoice == 4)
                            {
                                client->managePersonalInfo(); // 个人信息管理
                            }
                        }
                    }
                    pause();
                }
                else if (roleChoice == 4) // 药剂师登录
                {
                    Pharmacist *client = pharmacistLogin(phaHead);
                    if (client)
                    {
                        LogManager::getInstance().logOperation(client->getUserID(), "药剂师", "登录", "药剂师登录成功");
                        while (true)
                        {
                            int pharmacistChoice = pharmacistMenu();
                            if (pharmacistChoice == 0)
                            {
                                break; // 退出登录
                            }
                            else if (pharmacistChoice == 1)
                            {
                                client->manageMedicationRecords(medRecHead, medHead, conHead, medicationRecordCount);
                            }
                            else if (pharmacistChoice == 2)
                            {
                                client->manageMedicines(medHead, medicineCount);
                            }
                            else if (pharmacistChoice == 3)
                            {
                                client->managePersonalInfo(); // 个人信息管理
                            }
                        }
                    }
                    pause("药剂师 > 退出登录");
                }
                else if (roleChoice == 5) // 患者登录
                {
                    Patient *client = patientLogin(patientHead);
                    if (client)
                    {
                        LogManager::getInstance().logOperation(client->getUserID(), "患者", "登录", "患者登录成功");
                        while (true)
                        {
                            int patientChoice = patientMenu();
                            if (patientChoice == 0)
                            {
                                break; // 退出登录
                            }
                            else if (patientChoice == 1)
                            {
                                client->manageRegistrations(regHead, docHead, registrationCount);
                            }
                            else if (patientChoice == 2)
                            {
                                client->manageConsultations(conHead);
                            }
                            else if (patientChoice == 3)
                            {
                                client->manageExaminations(examHead);
                            }
                            else if (patientChoice == 4)
                            {
                                client->manageMedications(medRecHead);
                            }
                            else if (patientChoice == 5)
                            {
                                client->manageHospitalizations(hosHead, bedHead);
                            }
                            else if (patientChoice == 6)
                            {
                                client->managePersonalInfo();
                            }
                            else
                            {
                                std::cout << "无效的选择! 请重新选择。" << std::endl;
                            }
                        }
                    }
                    pause();
                }
                else if (roleChoice == 0) // 返回上一级
                {
                    break; // 返回到登录注册选择界面
                }
            }
        }
        else if (loginChoice == 2) // 注册
        {
            int registerChoice = identitySelectionMenu();
            if (registerChoice == 1) // 管理员注册
            {
                Admin *newAdmin = new Admin();
                if (newAdmin->adminSignUp(adminIDCount))
                {
                    newAdmin->next = adminHead;
                    if (adminHead != nullptr) adminHead->prev = newAdmin;
                    adminHead = newAdmin;
                    printSuccess("管理员注册成功");
                    LogManager::getInstance().logOperation(newAdmin->getUserID(), "管理员", "注册", "新管理员账号注册成功");
                }
                else
                {
                    delete newAdmin;
                    std::cout << "注册失败！" << std::endl;
                }
            }
            else if (registerChoice == 2) // 医生注册
            {
                Doctor *newDoctor = new Doctor();
                if (newDoctor->doctorSignUp(doctorIDCount))
                {
                    newDoctor->next = docHead;
                    if (docHead != nullptr)
                    {
                        docHead->prev = newDoctor;
                    }
                    docHead = newDoctor;
                    printSuccess("医生注册成功！");
                    LogManager::getInstance().logOperation(newDoctor->getUserID(), "医生", "注册", "新医生账号注册成功");
                }
                else
                {
                    delete newDoctor;
                    std::cout << "注册失败！" << std::endl;
                }
            }
            else if (registerChoice == 3) // 护士注册
            {
                Nurse *newNurse = new Nurse();
                if (newNurse->nurseSignUp(nurseIDCount))
                {
                    newNurse->next = nurseHead;
                    if (nurseHead != nullptr)
                    {
                        nurseHead->prev = newNurse;
                    }
                    nurseHead = newNurse;
                    printSuccess("护士注册成功！");
                    LogManager::getInstance().logOperation(newNurse->getUserID(), "护士", "注册", "新护士账号注册成功");
                }
                else
                {
                    delete newNurse;
                    std::cout << "注册失败！" << std::endl;
                }
            }
            else if (registerChoice == 4) // 药剂师注册
            {
                Pharmacist *newPharmacist = new Pharmacist();
                if (newPharmacist->pharmacistSignUp(pharmacistIDCount))
                {
                    newPharmacist->next = phaHead;
                    if (phaHead != nullptr)
                    {
                        phaHead->prev = newPharmacist;
                    }
                    phaHead = newPharmacist;
                    printSuccess("药剂师注册成功！");
                    LogManager::getInstance().logOperation(newPharmacist->getUserID(), "药剂师", "注册", "新药剂师账号注册成功");
                }
                else
                {
                    delete newPharmacist;
                    std::cout << "注册失败！" << std::endl;
                }
            }
            else if (registerChoice == 5) // 患者注册
            {
                Patient *newPatient = new Patient();
                if (newPatient->patientSignUp(patientIDCount))
                {
                    newPatient->next = patientHead;
                    if (patientHead != nullptr)
                    {
                        patientHead->prev = newPatient;
                    }
                    patientHead = newPatient;
                    printSuccess("患者注册成功！");
                    LogManager::getInstance().logOperation(newPatient->getUserID(), "患者", "注册", "新患者账号注册成功");
                }
                else
                {
                    delete newPatient;
                    std::cout << "注册失败！" << std::endl;
                }
            }
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
    saveMedicineFlows(medFlowHead, medicineFlowIDCount);     // 保存药品流水记录
    saveBedInfos(bedHead, bedCount);                          // 保存床位信息数据

    LogManager::getInstance().info("系统退出，所有数据已保存");

    // 清理所有链表内存
    while (adminHead) { Admin *n = adminHead->next; delete adminHead; adminHead = n; }
    while (docHead) { Doctor *n = docHead->next; delete docHead; docHead = n; }
    while (nurseHead) { Nurse *n = nurseHead->next; delete nurseHead; nurseHead = n; }
    while (phaHead) { Pharmacist *n = phaHead->next; delete phaHead; phaHead = n; }
    while (patientHead) { Patient *n = patientHead->next; delete patientHead; patientHead = n; }
    while (regHead) { Registration *n = regHead->next; delete regHead; regHead = n; }
    while (conHead) { Consultation *n = conHead->next; delete conHead; conHead = n; }
    while (examHead) { Examination *n = examHead->next; delete examHead; examHead = n; }
    while (hosHead) { Hospitalization *n = hosHead->next; delete hosHead; hosHead = n; }
    while (medRecHead) { MedicationRecord *n = medRecHead->next; delete medRecHead; medRecHead = n; }
    while (medHead) { Medicine *n = medHead->next; delete medHead; medHead = n; }
    while (medFlowHead) { MedicineFlow *n = medFlowHead->next; delete medFlowHead; medFlowHead = n; }
    while (bedHead) { bedInfo *n = bedHead->next; delete bedHead; bedHead = n; }
    } // end try
    catch (const std::exception &e)
    {
        std::cerr << "\n程序发生异常: " << e.what() << std::endl;
        emergencySave();
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n程序发生未知异常" << std::endl;
        emergencySave();
        return 1;
    }
}
