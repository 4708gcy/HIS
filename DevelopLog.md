# HIS 开发日志

---

## 2026.4.8

### 1. 类与数据结构设计

- 创建了3个医疗信息类
  - Registration    - 挂号信息类
  - Consultation    - 看诊信息类
  - Examination     - 检查信息类

### 2. 用户交互界面

- 创建了用户交互界面的头文件 `UI.h`，并完成了 6 个界面的初步设计
  - int beginUI();                          - 启动界面，显示欢迎信息和主菜单
  - int adminMenu();                        - 管理员菜单，提供账户管理和医疗记录管理选项
  - std::string adminDepartmentMenu();      - 管理员科室管理菜单
  - int adminMedicalRecordMenu();           - 管理员医疗记录管理菜单
  - int adminRegistrationManagementMenu();  - 管理员挂号记录管理菜单
  - int adminRegistrationViewMenu()         - 管理员挂号记录查看方式选择菜单

### 3. 管理员相关功能

- 完成了管理员的 `注册账号`函数
- 完成了管理员的 `登录账号`函数
- 完成了管理员的 `解锁/封禁账号`函数（使用泛函编程的方式实现）

### 4. 挂号信息管理

- 完成了挂号信息管理中的 3 个功能
  - 查看某一个科室的挂号信息（支持全部检索，按照挂号信息的状态检索，按照患者ID检索，按照医生ID进行检索）
  - 修改某一个科室的挂号信息的状态（已预约/已支付/已取消/已完成）
  - 删除某一个科室中的某一条挂号信息

---

## 2026.4.9

### 1. 输入校验与数据录入

- 在 `UI.h` 中添加并实现了 6 个输入校验函数
  - int selectIntCheck(const int min, const int max); // 检查输入的选择是否在指定范围内
  - double inputFeeCheck(); // 检查输入的费用是否为有效的正数
  - std::string inputStringCheck(const std::string &prompt); // 检查输入的字符串是否符合要求（如非空等）
  - std::string inputIDCheck(const std::string &prompt); // 检查输入的用户ID是否符合格式要求（如长度、前缀等）
  - std::string inputRecordIDCheck(const std::string &prompt); // 检查输入的记录ID是否符合格式要求（如长度、前缀等）
  - std::string inputPwdCheck(const std::string &prompt); // 检查输入的密码是否符合安全要求（如长度、复杂度等）

### 2. 挂号信息管理优化

- 将昨天的挂号信息管理中查看操作的 4 种视图还有修改和删除挂号信息的代码全部封装成单独的函数，提高可读性
- 添加了创建挂号信息的函数：
  - void addRegistration(Registration *&reg, const std::string &department); // 添加挂号记录（根据输入信息创建新的 Registration 对象，并插入到链表中）

### 3. 数据持久化与加载

- 创建了从文件中加载数据的头文件 `LoadData.h`，并实现了管理员信息的加载函数
  - Admin *loadAdminData(); // 从文件中加载管理员数据并返回管理员链表的头指针
- 创建了实现数据持久化的头文件 `SaveData.h`，并实现了管理员信息的存储函数，以及 4 种医疗信息（挂号/看诊/检查/住院）记录的存储函数
  - void saveAdminData(Admin *adminHead); // 将管理员数据保存到文件中
  - void saveRegistrations(Registration *regHead); // 将挂号记录保存到文件中
  - void saveConsultations(Consultation*conHead); // 将看诊记录保存到文件中
  - void saveExaminations(Examination *examHead); // 将检查记录保存到文件中
  - void saveHospitalizations(Hospitalization*hosHead); // 将住院记录保存到文件中

### 4. 登录功能

- 创建了可以实现登录功能的头文件 `Login.h`，并实现了管理员的登录验证函数
  - Admin*adminLogin(Admin*&adminHead); // 管理员登录函数，验证管理员身份并返回登录结果

### 5. 状态转字符串工具

- 在 User 基类中添加并实现了 6 个将医疗记录的状态转为可识别字符串的函数，提高了后序代码的可读性，降低了编写难度
  - std::string regStatusToString(RegistrationStatus status); // 将挂号状态枚举转换为字符串表示
  - std::string examStatusToString(ExaminationStatus status);  // 将检查状态枚举转换为字符串表示
  - std::string hosStatusToString(HospitalizationStatus status); // 将住院状态枚举转换为字符串表示
  - std::string medicationStatusToString(MedicationStatus status); // 将用药状态枚举转换为字符串表示
  - std::string medicationReviewStatusToString(MedicationReviewStatus status); // 将用药审核状态枚举转换为字符串表示
  - std::string medicineStatusToString(MedicineStatus status); // 将药品状态枚举转换为字符串表示

### 6. 主程序框架与调试经验

- 在 main.cpp 文件中编写了系统的大体运行框架，将不同的功能简单地串联了一下，并且测试了管理员的注册和登录功能，都可以正常运行
- 在测试过程中踩的最大的坑就是 `Admin` 类中的 "**next**" 指针一定不能是野指针，必须要初始化为 nullptr，一开始没有注意，程序莫名其妙崩了好几次
- 后来通过网上学习了配置 `launch.json` 和 `tasks.json`，进行一步步调试才发现程序是因为野指针崩溃的，这是一个教训，不过也让我学会了在 VSCode 中如何配置调试文件

---

## 2026.4.10

### 1. 任务与调试配置

- 将昨天的`tasks.json`文件新增了两个任务("cmake环境配置"和"build文件夹清理")，并为"cmake编译"添加了依赖:"cmake环境配置"，将`launch.json`文件添加了调试之后自动调用任务"build文件夹清理"

### 2. 挂号记录加载函数

- 在`LoadData.h`中添加了挂号记录的加载函数
  - Registration *loadRegistrations(int& count); // 从文件中加载挂号记录并返回挂号链表的头指针

### 3. 看诊记录属性扩展

- 给`Consultation.h`中的看诊记录添加了两个新属性
  - bool isHospitalizationRecommended = false; // 是否建议住院（根据病情严重程度等因素评估得出）
  - bool isPrecriptionReviewed = false; // 处方是否已审核（由药师或医生审核后设置为 true）

### 4. 管理员看诊信息管理功能

- 在`Admin.h`中添加并实现了管理员医疗记录管理功能中的看诊信息管理，总共拆分为 9 个函数
  - void manageConsultations(Consultation *&con, const std::string &department, Registration*reg, int &conCounter);               // 管理看诊记录（查看、修改诊断结果等）
  - void viewAllConsultations(Consultation *&con, const std::string &department);                                            // 查看所有看诊记录（可按患者ID、医生ID、状态过滤）
  - void viewConsultationsByDoctor(Consultation*&con, const std::string &department);                                       // 查看指定医生的看诊记录
  - void viewConsultationsByPatient(Consultation *&con, const std::string &department);                                      // 查看指定患者的看诊记录
  - void viewConsultationsByStatus(Consultation*&con, const std::string &department);                                       // 查看指定状态的看诊记录
  - void viewConsultationByRegistrationID(Consultation *&con, const std::string &department);                                // 根据挂号ID查看看诊记录
  - void modifyConsultation(Consultation*&con, const std::string &department);                                              // 修改看诊记录（如修改诊断结果、添加医生备注等）
  - void deleteConsultation(Consultation *&con, const std::string &department);                                              // 删除看诊记录（逻辑删除，设置 isDeleted 标志）
  - void addConsultation(Consultation*&con, const std::string &department, Registration*reg, int &conCounter); // 添加看诊记录（根据输入信息创建新的 Consultation 对象，并插入到链表中）

### 5. 管理员看诊记录管理UI

- 在`UI.h`中添加并实现了管理员管理看诊记录的UI交互界面函数，共 2 个
  - int adminConsultationManagementMenu(); // 管理员看诊记录管理菜单
  - int adminConsultationViewMenu(); // 管理员看诊记录查看方式选择菜单

### 6. 输入处理辅助函数

- 在`UI.h`中又添加并实现了一个好用的输入处理函数
  - std::string trim(const std::string &str); // 去除字符串首尾空格的辅助函数

### 7. 看诊记录状态转字符串

- 在`User.h`中补充了看诊记录信息的状态转字符串函数
  - std::string conStatusToString(ConsultationStatus status); // 将看诊状态枚举转换为字符串表示

### 8. ID变量存储策略调整

- 废除了原来各个人物和各类医疗管理信息的可用ID变量存储到`User`基类中的策略，改用全局变量存储可用ID变量

### 9. 看诊记录枚举修正

- 修正了看诊记录中的枚举类型变量，让其更贴合实际

---

## 2026.4.11

### 1. 看诊记录的持久化

- 补全了 `LoadData.cpp` 中的看诊记录加载函数
- 补全了 `LoadData.cpp` 中检查记录的加载函数
- 补全了 `SaveData.cpp` 中的看诊记录保存函数
- 补全了 `SaveData.cpp` 中的检查记录保存函数

### 2. 检查记录相关 UI

- 在 `UI.h` 中添加了 3 个界面函数
  - 管理员的检查记录界面函数
  - 查询检查记录的不同方式的界面函数
  - 检查项目的选择界面函数

### 3. 体征信息转字符串

- 在 `User.h` 中添加了将检查记录中的生命体征结构体数据转为字符串的函数

### 4. 记录查找功能补充

- 对前两天的代码补充了 2 个函数
  - 根据挂号ID查找挂号记录的函数
  - 根据检查ID查找检查记录的函数

### 5. 检查记录管理功能

- 在 `Admin.h` 中添加了关于检查记录的 9 个功能函数
- 这些函数相互结合实现了管理检查记录的功能

---

## 2026.4.12

### 1. 住院记录与床位管理功能完善

- 在 `UI.h` 中添加并实现了 3 个界面函数：
  - 管理员住院记录管理菜单
  - 管理员住院记录查看方式选择菜单
  - 住院记录病房类型选择菜单
- 在 `UI.h` 中添加并实现了 1 个床位ID输入校验函数
- 在 `Hospitalization.h` 中新增了 `bedInfo` 结构体，用于保存每一个床位的具体信息
- 在 `Admin.h` 中添加并实现了 11 个管理住院信息的功能函数，涵盖住院信息的增删查改等完整流程
- 在 `SaveData.h` 中添加了住院信息保存函数和床位信息保存函数
- 在 `LoadData.h` 中添加了住院信息加载函数和床位信息加载函数
- 在 `User.h` 中添加并实现了床位状态转字符串的函数

---

## 2026.4.13-4.14

### 1. 住院床位管理功能完善

- 在 `Hospitalization.h` 中新增 `bedInfo` 结构体，用于存储每一个床位的具体信息，便于后续护士对床位进行细节操作
- 在 `UI.h` 中添加并实现了以下界面函数：
  - 病房类型选择菜单
  - 床位信息管理菜单
  - 床位信息查看方式选择菜单
- 在 `Admin.h` 中编写了 11 个管理床位信息的功能函数，实现了床位信息的增删查改等管理流程
- 在 `SaveData.h` 中添加并实现了床位信息保存函数
- 在 `LoadData.h` 中添加并实现了床位信息加载函数
- 在 `User.h` 中添加了床位状态转字符串的函数

### 2. 用药记录管理功能完善

- 在 `UI.h` 中添加并实现了以下界面函数：
  - 用药记录管理菜单
  - 用药记录查看方式选择菜单
  - 用药记录审核状态选择菜单
  - 用药记录发药状态选择菜单
- 在 `Admin.h` 中编写了 13 个管理用药记录信息的功能函数，实现了用药记录的增删查改及审核、发药等流程
- 在 `SaveData.h` 中添加并实现了用药记录保存函数
- 在 `LoadData.h` 中添加并实现了用药记录加载函数

### 3. 用户信息属性扩展

- 在 `User.h` 中新增了性别、年龄、电话号、邮箱属性
- 修改了 `User.h` 中的注册函数以支持新属性
- 修改了 `SaveData.h` 中的管理员信息保存函数以支持新属性
- 修改了 `LoadData.h` 中的管理员信息加载函数以支持新属性

---
