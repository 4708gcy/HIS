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

### 4. 药品信息管理功能完善

- 对 `Medicine.h` 中的药品结构体中的属性进行了整理，删除一些多余的属性，增加了一些实用的属性
- 再 `UI.h` 中添加并实现了以下界面函数:
  - 药品管理界面
  - 药品查看方式选择菜单
  - 药品状态设置菜单
  - 修改药品信息菜单
- 在 `Admin.h` 中编写了 21 个管理药品信息的功能函数，实现了药品信息的增删改查，尤其是 "改" 的操作函数详细化了，之前的几个医疗信息的 "改" 操作只能修改记录的状态，其他的信息都无法修改，今天的药品信息的改操作覆盖了药品类含有的所有属性
- 在 `SaveData.h` 中添加并实现了药品信息的保存函数
- 在 `LoadData.h` 中添加并实现了药品信息的加载函数
- 今天还发现了一个 Bug，那就是我保存和读取函数都是使用 英文中的逗号 "," 分隔信息的，但是忘了诸如备注属性中也可能存在英文逗号，这样很可能导致程序崩溃，因此我对 `UI.h` 中的字符串输入检查函数(`inputStringCheck`)进行了优化，直接禁止在医疗信息和人物属性中输入英文逗号

---

## 2026.4.15

### 1. 人物类头文件完善

- 拉取了 4 个人物类的头文件，并进行了部分修正。

### 2. 管理员医生信息管理功能

- 在 `Admin.h` 中添加并实现了 22 个管理员管理医生信息的函数，实现了医生信息的增删改查功能。

### 3. 管理员相关 UI 菜单

- 在 `UI.h` 中添加并实现了以下菜单界面：
  - 管理员用户（医生/护士/药剂师/患者）管理菜单
  - 管理员医生信息管理菜单
  - 管理员医生信息查看方式选择菜单
  - 医生职称选择菜单
  - 管理员医生信息修改菜单

### 4. 医生信息数据持久化

- 在 `SaveData.h` 和 `LoadData.h` 中分别添加并实现了医生信息保存函数和加载函数。

### 5. 费用计算规则与实现

- 制定了挂号费用依据医生职称、检查费用依据检查项目名称、住院费用依据床位类型和住院天数的规定。
- 在 `User.h` 中添加并实现了 3 个对应的价格转换函数：

  ```cpp
  double calculateRegistrationFee(DoctorTitle title);                 // 根据医生职称计算挂号费用
  double calculateHospitalizationFee(std::string wardType, int days); // 根据床位类型和住院天数计算住院费用
  double calculateExaminationFee(std::string itemName);               // 根据检查项目名称计算检查费用
  ```

#### 具体价格规定

- 医生职称与挂号费用：

  ```cpp
  enum class DoctorTitle
  {
      INTERN = 1,      // 实习医生    挂号费用：10元
      RESIDENT,        // 住院医师    挂号费用：20元
      ATTENDING,       // 主治医师    挂号费用：30元
      ASSOCIATE_CHIEF, // 副主任医师  挂号费用：40元
      CHIEF            // 主任医师    挂号费用：50元
  };
  ```

- 检查项目价格表（单位：元）：

  | 检查项目         | 代码/名称                 | 价格 |
  | :--------------- | :------------------------ | ---: |
  | 体温测量         | temperatureC              |   5  |
  | 血压测量         | systolicBP/diastolicBP    |   8  |
  | 心率测量/脉搏    | heartRate                 |   5  |
  | 呼吸频率测量     | respiratoryRate           |   5  |
  | 血氧饱和度       | spo2                      |  10  |
  | 身高测量         | height                    |   5  |
  | 体重测量         | weight                    |   5  |
  | BMI计算          | bmi                       |   5  |
  | 疼痛评分         | painScore                 |   2  |
  | 腰围测量         | waistCircumference        |   5  |
  | 血糖测量         | bloodSugar                |  20  |
  | 体脂率测量       | bodyFat                   |  30  |
  | 尿酸测定         | uricAcid                  |  25  |
  | 总胆固醇         | cholesterol               |  25  |

- 住院费用：
  - 普通病房 50元/天
  - 隔离病房 100元/天
  - VIP病房 200元/天
  - ICU病房 500元/天

---

## 2026.4.16

### 1. 管理员管理功能完善

- 在 `Admin.h` 中添加并实现了 22 个有关管理护士信息的函数，将它们相互组合封装，实现了对护士信息的增删改查功能。
- 添加并实现了 20 个有关管理药剂师信息的函数，将它们相互组合封装，实现了对药剂师信息的增删改查功能。
- 添加并实现了 27 个有关管理患者信息的函数，将它们组合封装，实现了对患者信息的增删改查功能。

### 2. 人物类特殊属性方法完善

- 将 `Nurse.h`、`Doctor.h`、`Pharmacist.h` 和 `Patient.h` 中的特殊属性的基础 **setter** 和 **getter** 函数全部实现。

### 3. 人员与医疗记录持久化

- 在 `SaveData.h` 和 `LoadData.h` 中将护士、药剂师和患者的保存与加载函数全部实现，至此，所有人员信息和所有医疗记录信息的持久化功能全部实现。

### 4. 管理员账户管理功能

- 在 `Login.h` 中添加并实现了 14 个有关管理管理员账户的功能函数，将它们相互组合封装，实现了对管理员信息的增删改查功能。

### 5. 管理员主功能框架

- 在 `main.h` 中将管理员的全部功能（普通人员账户管理、医疗信息管理、药品管理、床位管理、管理员账户管理）的框架彻底搭建成功，但尚未进行 bug 测试。

---

## 2026.4.17

### 1. 管理员功能测试

- 编译测试了管理员的功能，目前来看可以正常运行。

### 2. 医生菜单与界面实现

- 在 `UI.h` 中添加并实现了以下医生相关菜单界面函数：
  - 医生菜单
  - 医生挂号信息管理菜单
  - 医生挂号信息查看方式选择菜单
  - 医生看诊记录管理菜单
  - 医生看诊记录查看方式选择菜单
  - 医生看诊记录修改菜单

### 3. 医生信息管理功能

- 在 `Doctor.h` 中：
  - 添加并实现了 9 个挂号信息管理相关函数，组合封装实现了医生的挂号信息增删改查基本功能。
  - 添加并实现了 19 个看诊信息管理相关函数，组合封装实现了医生的看诊信息增删改查基本功能。

### 4. 管理员功能重构优化

- 发现了原来管理员功能中的修改各个医疗记录的功能设计不合理且冗余，今天将 `Admin.h` 中原来的挂号记录管理模块和看诊记录管理模块进行了重构优化。

---

## 2026.4.18

### 1. 管理员管理模块重构与优化

- 修正并重构了 `Admin.h` 中检查记录管理、住院记录管理、用药记录管理、药品信息管理、医生信息管理、护士信息管理、药剂师信息管理、患者信息管理等模块，优化了函数设计，共计优化 152 个函数。

### 2. 注册函数返回值设计优化

- 在优化上述模块过程中，发现 `Doctor.h`、`Nurse.h`、`Pharmacist.h`、`Patient.h` 和 `Admin.h` 的注册函数返回值设计不合理，对这些注册函数进行了修正。

### 3. 医生检查记录管理功能完善

- 在 `Doctor.h` 中新增并实现了 16 个医生操作检查记录相关的功能函数，组合封装实现了医生的检查记录管理模块。

### 4. 医生检查记录管理相关 UI

- 在 `UI.h` 中添加并实现了医生检查记录管理菜单、医生检查记录查看方式选择菜单和医生检查记录修改菜单。

### 5. 结构体空值表示规范

- 制定新规定：对 `Registration.h`、`Consultation.h`、`Examination.h`、`Hospitalization.h`、`MedicationRecord.h`、`Medicine.h` 中各类结构体和类的 **String** 类型属性，赋值为 **"#"** 表示空（无值）。

### 6. 个人信息管理功能完善

- 在 `Doctor.h` 中添加并实现了医生的个人信息管理函数。
- 在 `Admin.h` 中补充了管理员的个人信息管理函数。
- 在 `UI.h` 中添加并实现了医生的个人信息管理菜单、医生的个人信息查看菜单、医生的个人信息修改菜单、管理员的个人信息管理菜单、管理员的个人信息查看菜单、管理员的个人信息修改菜单；完善了管理员的总功能菜单，新增了管理员个人信息管理功能和账户管理功能（激活/封锁账户）。

### 7. 账户激活/封锁功能泛型化

- 在 `Login.h` 中使用泛型编程实现了对各个身份账户的激活/封锁功能，并通过 `AccountManagement` 函数统一调度各身份的泛型调用，实现了管理员对所有账户的激活/封锁管理。

### 8. 主功能完善

- 在 `main.h` 中完善了管理员的全部功能，并初步完成了医生的模块。

---

## 2026.4.20

### 1. 患者功能管理菜单完善

- 在 `UI.h` 新增并实现了患者相关的各类功能菜单，包括：
  - 患者主功能管理菜单
  - 挂号、看诊、检查、用药、住院信息的管理菜单及其查看方式选择菜单

### 2. 患者信息管理功能扩展

- 在 `Patient.h` 新增并实现了以下功能：
  - 10 个挂号信息管理函数
  - 8 个看诊信息管理函数
  - 9 个检查信息管理函数
  - 10 个用药信息管理函数
- 初步完善了患者的挂号、看诊、检查、用药信息的管理功能。

### 3. 患者账户余额属性支持

- 在 `Patient.h` 新增 `balance` 属性，表示患者账户余额。
- 修正了 `SaveData.cpp` 和 `LoadData.cpp` 中患者信息的保存与加载函数，实现余额的持久化。

### 4. 患者住院信息管理功能完善

- 在 `UI.h` 新增并实现了患者住院信息管理菜单及查看方式选择菜单。
- 在 `Patient.h` 新增并实现了患者住院信息管理相关的 13 个函数，涵盖住院信息的查询、押金缴纳、出院结算等完整业务流程，完善了患者的住院信息管理功能。

---

## 2026.4.21

### 1. 患者功能完善

- 在 `Patient.h` 中新增并实现了患者个人信息管理功能，包括“查看个人信息”和“修改个人信息”两部分，进一步完善了患者的个人信息管理模块。
- 对昨日实现的患者住院信息管理功能进行了优化。

### 2. 登录功能扩展

- 在 `Login.h` 中新增并实现了患者和药剂师的登录函数，完善了患者和药剂师的登录功能。

### 3. 患者主功能框架

- 在 `main.cpp` 中完善了患者模块，初步搭建了患者主功能框架。

### 4. 用户注册模块

- 在 `main.cpp` 中初步完成了用户注册模块，实现了管理员、医生、护士、药剂师、患者的注册功能。

### 5. 患者个人信息管理菜单

- 在 `UI.h` 中新增并实现了患者个人信息管理菜单、个人信息查看菜单和个人信息修改菜单。

### 6. 小组成员代码集成

- 拉取并集成了小组成员的 `Nurse.h`、`Nurse.cpp`、`Pharmacist.h`、`Pharmacist.cpp` 到本地项目。

### 7. 药剂师主功能框架

- 在 `main.cpp` 中初步完善了药剂师模块，搭建了药剂师主功能框架。

### 8. 药剂师相关菜单与功能

- 在 `UI.h` 中新增并实现了药剂师管理菜单、用药信息管理菜单、用药信息查看方式选择菜单、个人信息管理菜单、个人信息查看菜单、个人信息修改菜单、药品信息管理菜单、药品信息查看方式选择菜单、药品信息修改菜单。
- 根据上述菜单，补充完善了 `Pharmacist.h` 中药剂师的用药信息管理、个人信息管理和药品信息管理等功能的实现。

### 9. 护士相关菜单与功能

- 在 `UI.h` 中新增并实现了护士管理菜单、住院信息管理菜单、住院信息查看方式选择菜单、个人信息管理菜单、个人信息查看菜单、个人信息修改菜单、检查信息管理菜单、检查信息查看方式选择菜单、检查信息修改菜单。
- 根据上述菜单，补充完善了 `Nurse.h` 中护士的住院信息管理和检查信息管理功能的实现。

### 10. 枚举类型修正

- 修正了检查记录和住院记录中的枚举类型，将其全部设置为从 1 开始。

---

## 2026.4.22

### 1. 护士床位信息与个人信息管理功能完善

- 在 `Nurse.h` 和 `Nurse.cpp` 中补充并完善了护士的床位信息管理功能及个人信息管理功能的实现。

### 2. 护士登录功能完善

- 在 `Login.h` 中补充并完善了护士的登录函数实现。

### 3. 护士主功能框架完善

- 在 main.cpp 中完善了护士模块，搭建了护士主功能框架。

### 4. 护士相关菜单修正

- 在 `UI.h` 中修正了护士的床位信息管理相关菜单和个人信息管理相关菜单。

### 5. 床位ID唯一性校验修复

- 在 `Admin.cpp` 中修复了管理员创建床位时未检测床位ID是否已存在的漏洞。

### 6. 调试配置优化

- 修改了 `launch.json` 文件中的 `cwd` 路径为 `"cwd": "${workspaceFolder}/build"`，确保调试时的工作目录正确，避免因目录错误导致的文件读取失败问题。
- 新增了 `console` 配置项 `"console": "externalTerminal"`，使调试时使用外部终端，便于查看输入输出，解决了之前无法输入的问题。

---

## 2026.4.24

### 1. 基础数据初始化

- 借助大语言模型批量生成了 100 条基础测试药品信息。
- 为系统内的每个科室分别初始化了 60 条床位信息（包含 20 条普通病房、20 条隔离病房、10 条 VIP 病房以及 10 条 ICU 病房），并将这些初始化数据成功持久化保存至对应文件中。

### 2. 死循环 Bug 修复

- 修复了 `main.cpp` 中由于缺失 `break` 语句而导致的一个严重 Bug：在管理员进行药品管理的科室选择循环中，由于没有跳出条件，导致程序陷入死循环无法返回上级菜单，现该问题已解决。

### 3. UI 界面优化升级

- 对 `UI.cpp` 中所有的控制台交互界面函数进行了统一的格式排版与视觉优化，使系统界面呈现更加美观清晰，进一步提升了用户的交互体验。

---

## 2026.4.25

### 1. 全面代码审核与 Bug 修复

对项目进行了全面的代码审核，发现并修复了 35+ 个 Bug，覆盖管理员、医生、护士、药剂师、患者以及登录注册系统：

**严重级别（Critical）：**

- 修复 `Login.cpp` 中 `viewAdminsByGender` 函数的无限循环 Bug：缺少 `current = current->next;` 导致遍历时陷入死循环
- 修复 `Login.cpp` 中 `pharmacistLogin` 函数硬编码绕过问题：`if (id == "0") return nullptr;` 导致 ID 为 "0" 的药剂师无法登录
- 修复 `SHA-256.cpp` 中的时序攻击漏洞：使用标准 `std::string` 比较密码 hash，攻击者可通过响应时间推断密码正确性，改为使用 volatile XOR 逐字节比较的时序安全比较函数
- 修复 `User.cpp` 中 ID 生成的未定义行为：`"0" + std::to_string(idCounter++).insert(...)` 在同一表达式中修改和读取变量，分离为两条独立语句
- 修复所有角色（Admin/Doctor/Nurse/Pharmacist/Patient）的登录成功后未重置 `loginAttempts` 计数器的问题，导致登录失败次数累计异常

**高级别（High）：**

- 修复 `Admin.cpp` 中约 50 个 modify 函数调用：错误地传递了链表头指针而非目标指针，导致修改操作作用于错误对象
- 修复 `Admin.cpp` 中 `modifyMedicationRecordReviewStatus`：错误使用 `target->status` 而非 `target->reviewStatus`
- 修复 `Admin.cpp` 中 `deletePharmacist` 和 `deletePatient`：传递头指针而非目标指针，导致删除了错误的记录
- 修复 `Admin.cpp` 中医生排班检查：从 `doc == nullptr` 改为检查是否有在岗医生
- 修复 `Admin.cpp` 中 5 处 ID 生成的未定义行为
- 修复 `Admin.cpp` 中管理员密码修改：错误地使用 `this->salt` 存储新盐值，改为 `this->storedHash`
- 修复 `Doctor.cpp` 中 `while(true)` 无限循环：改为 `while(currentReg != nullptr)` 防止崩溃
- 修复 `Doctor.cpp` 中 `createExaminationByConsultation`：错误使用 `currentCon->` 而非 `targetCon->`
- 修复 `Doctor.cpp` 中取消功能不可达：使用 `selectIntCheck(0, 2)` 正确处理值班状态选择
- 修复 `Doctor.cpp` 中处方显示错误：将 "药品名称" 修正为 "药品ID"（3 处）
- 修复 `Doctor.cpp` 中密码修改流程问题
- 修复 `Doctor.cpp` 中费用计算错误：从 `this->title` 改为 `doc->title`
- 修复 `Doctor.cpp` 中 3 处 ID 生成的未定义行为

**中等级别（Medium）：**

- 修复 `Nurse.cpp` 中 ID 生成的未定义行为
- 修复 `Pharmacist.cpp` 中 `getMedicinesByStatus` 的变量遮蔽问题：内部变量覆盖外部变量导致状态过滤失效
- 修复 `Pharmacist.cpp` 中 `setMedicationRecordReviewStatus`：缺少 `!con->isDeleted` 检查，可能操作已删除记录
- 修复 `Pharmacist.cpp` 中 `isPrecriptionReviewed` 设置逻辑：仅在审核通过时设置，拒绝时不应设置
- 修复 `Pharmacist.cpp` 中 2 处 ID 生成的未定义行为
- 修复 `Patient.cpp` 中密码修改：`this->salt` 改为 `this->storedHash`
- 修复 `Patient.cpp` 中 `selectIntCheck` 范围：移除 `-1` 使其与枚举值匹配
- 修复 `Admin.cpp` 和 `main.cpp` 中的拼写错误：`AdminPersionalInfo` 改为 `AdminPersonalInfo`

**低级别（Low）：**

- 修复 `Login.cpp` 中 `deleteAdmin`：从物理删除改为逻辑删除（设置 `setIsDeleted`），保持与其他删除操作的一致性
- 修正所有角色密码修改流程中的密码确认逻辑

### 2. 终端交互体验优化

**控制台颜色系统（`UI.h` / `UI.cpp`）：**

- 新增 `ConsoleColor` 枚举（RED, GREEN, YELLOW, CYAN, WHITE, DEFAULT）
- 实现 `setConsoleColor()` / `resetConsoleColor()`：支持 Windows API (`SetConsoleTextAttribute`) 和 ANSI 转义序列双模式，确保跨平台兼容
- 实现 4 个快捷打印函数：
  - `printTitle()`：打印带分隔线的标题
  - `printSuccess()`：绿色成功消息
  - `printError()`：红色错误消息
  - `printWarning()`：黄色警告消息
- 为 `printMenuBorder()` 和 `printMenuTitle()` 添加颜色增强

**分页显示功能（`UI.h` / `UI.cpp`）：**

- 实现 `printWithPagination()`：分页打印长列表，支持上一页/下一页/跳转到指定页/退出等导航操作
- 使用 `(std::min)` 避免 Windows `min` 宏冲突

**面包屑导航（`UI.h` / `UI.cpp`）：**

- 增强 `pause()` 函数：支持 `breadcrumb` 参数，在暂停提示中显示当前导航路径
- 管理员密码修改流程优化：改进密码确认步骤，先比较密码再执行修改

**隐藏密码输入（`UI.h` / `UI.cpp`）：**

- 实现 `inputHiddenPwdCheck()`：使用 Windows `ReadFile` API 实现终端密码隐藏输入（不回显），用于登录场景
- 注册时仍使用普通密码输入（用户需要确认注册的密码内容）

### 3. 操作日志系统（`UI.h` / `UI.cpp`）

- 实现 `LogManager` 单例类：
  - `info()` / `warn()` / `error()`：记录不同级别的操作日志
  - `logOperation()`：记录结构化操作日志（用户ID、角色、操作类型、详细信息）
  - 日志文件持久化存储到 `Data/OperationLog/` 目录
  - 线程安全（使用 `std::mutex` 保护写入）
  - 日志格式：`[YYYY-MM-DD hh:mm:ss] [LEVEL] message`

### 4. 代码架构优化

- 将 `LogManager` 设计为单例模式，全局统一的日志入口
- 新增 `Data/OperationLog/` 目录用于存储操作审计日志
- `UI.h` 新增必要的头文件包含：`<fstream>`, `<mutex>`, `"../Head/GetTime.h"`

---

## 2026.4.26

### 1. 4.25 功能全面接入

**彩色打印函数接入（5 个业务文件 + main.cpp）：**

- 将 Admin.cpp、Doctor.cpp、Nurse.cpp、Pharmacist.cpp、Patient.cpp、Login.cpp 中所有裸 `std::cout` 成功/错误/警告消息替换为 `printSuccess()`（绿色）、`printError()`（红色）、`printWarning()`（黄色）
- 共计替换 250+ 处消息

**操作日志系统接入：**

- 在所有角色（Admin/Doctor/Nurse/Pharmacist/Patient）登录成功时添加 `LogManager::logOperation()` 记录
- 在所有角色注册成功时添加日志记录
- 在 `main.cpp` 系统退出时添加 `LogManager::info("系统退出，所有数据已保存")`
- 日志实时写入 `Data/OperationLog/his_YYYY_MM_DD.log`

**面包屑导航接入：**

- 为 300+ 处 `pause()` 调用添加了面包屑路径参数，例如 `"管理员 > 医疗记录管理 > 挂号记录管理"`、`"医生 > 看诊管理"`、`"护士 > 床位管理"` 等
- 覆盖所有角色的全部业务操作模块

**分页显示接入：**

- 在 `Login.cpp` 的 `viewAllAdmins`、`viewAdminsByName`、`viewAdminsByGender`、`viewAdminsByAgeGroup`、`viewAdminsByContactInfo` 函数中，将直接打印改为收集到 `vector<string>` 后用 `printWithPagination()` 分页打印
- 超过 10 条记录时自动分页，支持上一页/下一页/跳转/退出等导航操作

---

### 2. 关键 Bug 修复（6 项）

**严重级别（Critical）：**

- 修复 `Admin.cpp` 中 `addHospitalization` 函数的 `bedFound = true;` 死代码 Bug：该语句位于 `break;` 之后，永远不会执行，导致床位分配始终报告失败，无法正常添加住院记录
- 修复 `Admin.cpp` 中 `viewConsultationByID` 函数的无限循环 Bug：while 循环缺少 `current = current->next;` 推进语句，当目标 ID 不在链表头部时会陷入死循环
- 修复 `Admin.cpp` 中用药记录管理模块的 11 处 "审核状态" 显示错误：将 `statusStr`（缴费/发药状态）改为 `reviewStatusStr`（审核状态），涉及 `viewAllMedicationRecords`、`viewMedicationRecordsByPatient`、`viewMedicationRecordsByDoctor` 等多个查看和修改函数
- 修复 `Pharmacist.cpp` 中 `printMedicationRecord` 函数的同一问题：`reviewStatusStr` 变量已声明但未使用，实际输出的是 `statusStr`（缴费状态）
- 修复 `Login.cpp` 中全部 5 个登录函数（admin/doctor/nurse/pharmacist/patient）不检查 `isDeleted` 标志的问题：已逻辑删除的账户仍可正常登录，改为在查找循环中添加 `!isDeleted` 条件过滤
- 修复 `Patient.cpp` 中 `applyForDischarge` 函数的日期计算公式错误：原来使用 `月 * 30 + 日` 的粗略算法（如 1月31日 到 2月28日 会算成 27 天而非 28 天），改用 `std::mktime` + `std::difftime` 进行精确的日历日期差计算

**高级别（High）：**

- 修复 `main.cpp` 中药剂师退出后缺少 `pause()` 的问题：所有角色退出后都有暂停提示，唯独药剂师缺失，导致用户看不到退出信息
- 修复 `Login.cpp` 中全部 5 个登录函数缺少密码重试机制的问题：原逻辑密码错误一次就返回角色选择菜单，用户需重新导航，改为 3 次重试循环，每次提示剩余机会次数
- 修复 `LoadData.cpp` 中 `loadBedInfos` 函数的 `std::stoi` 缺少 try-catch 保护的问题：与其他所有 load 函数不一致，损坏的 count 行会导致未捕获异常崩溃
- 修复 `Admin.cpp` 中 `viewPatientsByContactInfo` 函数的邮箱搜索 `found` 变量遮蔽问题：内部重新声明了 `bool found` 导致外部变量始终为 false，即使找到匹配也会报告"未找到"
- 修复 `Admin.cpp` 中 4 处看诊记录详情显示循环（`viewAllConsultations`、`viewConsultationsByDoctor`、`viewConsultationsByPatient`、`viewConsultationByRegistrationID`）中的 `current->` 误用 Bug：在 `for (Consultation *c : temp)` 循环中引用了已变为 nullptr 的 `current` 指针而非循环变量 `c`，属于未定义行为

**中等级别（Medium）：**

- 修复 `UI.cpp` 中 `LogManager::writeLog` 函数的线程安全问题：已 include `<mutex>` 但未实际使用，添加 `std::lock_guard<std::mutex>` 保护日志写入
- 修复 `Doctor.cpp` 中处方数量上限 `INT_MAX` 的问题：理论上用户可输入 21 亿颗药，改为合理的上限 10000
- 修复 `Patient.cpp` 中多处 C 风格 `(int)` 枚举转换：统一改为 `static_cast<int>()` 提升类型安全性
- 在 `main.cpp` 系统退出流程中添加了 12 条链表（Admin/Doctor/Nurse/Pharmacist/Patient/Registration/Consultation/Examination/Hospitalization/MedicationRecord/Medicine/bedInfo）的完整内存清理逻辑，消除程序退出时的内存泄漏

---

### 3. 终端交互体验优化

- 为 `pause()` 函数全面接入面包屑导航参数，用户在任意深层菜单操作后都能看到完整的导航路径提示（如 "管理员 > 医疗记录管理 > 看诊记录管理"）
- 为长列表查看功能接入分页显示，管理员查看用户列表时超过 10 条记录自动分页

---

## 2026.4.27

### 1. 后端 API 化 — 全部完成

将控制台 HIS 系统完整转换为 REST API 后端服务器，所有 5 种角色的核心业务功能均已完成 API 端点。**数据持久化方式不变**，继续使用现有的 txt CSV 文件（`LoadData`/`SaveData` 模块完全复用）。

#### 1.1 引入第三方依赖

- **cpp-httplib** (v0.18.3)：header-only 的 C++ HTTP 服务器库，通过 jsdelivr CDN 下载至 `Head/httplib.h`（10255 行），支持 Winsock2 在 Windows/MSVC 上运行
- **nlohmann/json** (v3.11.3)：header-only 的 C++ JSON 库，下载至 `Head/json.hpp`（24765 行），用于所有 API 请求/响应的 JSON 序列化

#### 1.2 新增文件清单

| 文件 | 用途 |
|------|------|

| `Head/ApiResponse.h` | 统一 API 响应格式 `{code, message, data}` |
| `Head/JsonHelper.h` / `Source/JsonHelper.cpp` | 12 种数据结构/角色的 JSON 序列化 + 枚举转字符串 |
| `Head/JWTAuth.h` / `Source/JWTAuth.cpp` | JWT 实现（Base64 URL + HMAC-SHA256 + 24h 时效） |
| `Head/ApiServer.h` / `Source/ApiServer.cpp` | DataManager 单例 + 全部 REST 路由注册（~1500 行） |
| `server_main.cpp` | REST 服务器入口（加载数据、注册路由、监听 8080 端口） |

#### 1.3 架构设计

- **DataManager 单例**：线程安全（`std::mutex`），启动时 `LoadData` 加载全部数据，每次写操作后 `SaveData` 持久化
- **JWT 认证**：`Base64(header).Base64(payload).Base64(HMAC-SHA256签名)`，payload 含 `{userID, role, iat, exp}`，24h 有效
- **统一响应**：`{code, message, data}`，错误码 200/400/401/403/404/500
- **CORS**：所有响应添加 `Access-Control-Allow-Origin: *`

#### 1.4 API 端点总览（共 60+ 个端点）

**认证（2）：** `POST /api/auth/login`（统一登录）、`POST /api/auth/register`（统一注册，5 种角色含特有属性）

**管理员（30+）：**

- 人员管理：`GET/PUT/DELETE /api/admin/{doctors,nurses,pharmacists,patients}` — CRUD + 过滤
- 医疗记录：`GET/PUT/DELETE /api/admin/{registrations,consultations,examinations,hospitalizations,medication-records}` — 查看/状态修改/逻辑删除
- 药品管理：`GET/PUT/DELETE /api/admin/medicines` — 含库存/价格/状态修改
- 床位管理：`GET/DELETE /api/admin/beds`
- 账号管理：`PUT /api/admin/account/:id/status` — 激活/封锁所有角色
- 管理员列表：`GET /api/admin/admins`

**医生（8）：** 挂号/看诊/检查记录的查看和修改 + 个人信息

**护士（8）：** 住院记录管理 + 体征录入 + 床位列表 + 个人信息

**药剂师（8）：** 用药记录审核/发药（自动扣库存）+ 药品库存调整 + 个人信息

**患者（12）：** 预约挂号 + 余额支付挂号费/检查费/药费 + 充值 + 个人信息查看/修改 + 各类记录查看

**通用（2）：** `GET /api/departments`（科室列表）、`GET /api/fee-standards`（费用标准）

#### 1.5 构建与运行

```bash
# 构建服务器
cd build && cmake --build . --config Debug --target his_server

# 运行（从 build 目录启动）
cd build && ./Debug/his_server.exe
# 服务器监听 http://localhost:8080，Ctrl+C 优雅退出
```

**测试示例**：

```bash
# 获取科室列表（无需认证）
curl http://localhost:8080/api/departments

# 登录
curl -X POST http://localhost:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d "{\"userID\":\"000001\",\"password\":\"123456\",\"role\":1}"

# 查看医生列表（需要管理员 token）
curl http://localhost:8080/api/admin/doctors \
  -H "Authorization: Bearer <token>"
```

#### 1.6 关键设计决策

1. **零侵入**：不修改任何现有文件，API 层作为独立新代码添加
2. **业务逻辑重写**：现有函数混合 console I/O，API 层直接操作链表数据结构
3. **线程安全**：`DataManager` 用 `std::mutex` + `lock_guard` 保护所有操作
4. **即时持久化**：每次写操作后 `saveAll()` 写回 txt 文件
5. **ID 生成一致**：复用 6 位 ID 规则（首位角色 + 5 位递增）

### 2. 前端 Web 界面 — Vue 3 + Element Plus 全部完成

基于后端 REST API，使用 Vue 3 + Element Plus 构建了完整的前端 Web 管理界面，覆盖 5 种角色全部业务功能。

#### 2.1 技术选型

| 技术 | 版本 | 用途 |
|------|------|------|

| Vue 3 | ^3.5 | 前端框架（Composition API + `<script setup>`） |
| Vite | ^6.0 | 构建工具（开发服务器 + 生产构建） |
| Element Plus | ^2.9 | UI 组件库（中文 locale，el-table/el-form/el-dialog 等） |
| Vue Router | ^4.5 | 路由管理（角色分流 + 路由守卫） |
| Pinia | ^2.3 | 状态管理（用户登录态、JWT token） |
| Axios | ^1.7 | HTTP 客户端（JWT 拦截器 + 错误处理） |

**选择 Element Plus 而非 Ant Design 的原因**：

- Element Plus 的中文生态更成熟（内置 `zh-cn` locale，日期/分页等组件原生中文支持）
- `el-table`/`el-form`/`el-dialog` 等组件 API 设计简洁，适合表格密集型管理系统
- `@element-plus/icons-vue` 提供丰富的图标，无需额外引入图标库

#### 2.2 项目创建步骤

**步骤 1:创建项目目录结构**

```bash
mkdir -p frontend/src/{api,store,router,styles,components}
mkdir -p frontend/src/views/{admin,doctor,nurse,pharmacist,patient}
```

**步骤 2:创建 `package.json`**

手动编写 `package.json`（不使用 `npm create vue` 交互式脚手架，便于理解和复现）：

```json
{
  "name": "his-frontend",
  "private": true,
  "version": "1.0.0",
  "type": "module",
  "scripts": {
    "dev": "vite",
    "build": "vite build",
    "preview": "vite preview"
  },
  "dependencies": {
    "vue": "^3.5.13",
    "vue-router": "^4.5.0",
    "pinia": "^2.3.0",
    "axios": "^1.7.9",
    "element-plus": "^2.9.1",
    "@element-plus/icons-vue": "^2.3.1"
  },
  "devDependencies": {
    "@vitejs/plugin-vue": "^5.2.1",
    "vite": "^6.0.0"
  }
}
```

**步骤 3：创建 `vite.config.js`（含 API 代理）**

```js
import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

export default defineConfig({
  plugins: [vue()],
  server: {
    port: 3000,
    proxy: {
      '/api': {
        target: 'http://localhost:8080',  // 后端 C++ 服务器地址
        changeOrigin: true
      }
    }
  }
})
```

**关键点**：Vite 开发服务器的 `proxy` 配置将 `/api` 开头的请求代理到后端 `localhost:8080`，开发时无需手动处理 CORS。生产环境则通过 Nginx 反向代理实现。

**步骤 4：创建 `index.html`（入口 HTML）**

标准 Vue 3 SPA 入口，`<script type="module">` 指向 `src/main.js`。

**步骤 5：创建 `src/main.js`（应用入口）**

```js
import { createApp } from 'vue'
import { createPinia } from 'pinia'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import zhCn from 'element-plus/es/locale/lang/zh-cn'
import * as ElementPlusIconsVue from '@element-plus/icons-vue'
import App from './App.vue'
import router from './router'

const app = createApp(App)
// 全局注册所有 Element Plus 图标
for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
  app.component(key, component)
}
app.use(createPinia()).use(router).use(ElementPlus, { locale: zhCn }).mount('#app')
```

**步骤 6:安装依赖**

```bash
cd frontend
npm install
```

#### 2.3 API 层设计（`src/api/`）

**核心思路**：封装 axios 实例，自动附加 JWT token，统一错误处理。

**`src/api/index.js`（axios 实例 + 拦截器）**：

```js
import axios from 'axios'
import { useUserStore } from '../store/user'
import router from '../router'

const request = axios.create({
  baseURL: '/api',
  timeout: 10000
})

// 请求拦截器：自动附加 JWT token
request.interceptors.request.use(config => {
  const store = useUserStore()
  if (store.token) {
    config.headers.Authorization = `Bearer ${store.token}`
  }
  return config
})

// 响应拦截器：401 自动跳转登录页
request.interceptors.response.use(
  response => response.data,
  error => {
    if (error.response?.status === 401) {
      const store = useUserStore()
      store.logout()
      router.push('/login')
    }
    return Promise.reject(error.response?.data || error)
  }
)

export default request
```

**API 模块按角色拆分**：

| 文件 | 封装的 API |
|------|----------- |

| `api/auth.js` | `login()`、`register()` |
| `api/common.js` | `getDepartments()`、`getFeeStandards()` |
| `api/admin.js` | 管理员全部 30+ 个端点（人员 CRUD、记录管理、账号管理） |
| `api/doctor.js` | 医生 8 个端点（挂号/看诊/检查 + 个人信息） |
| `api/nurse.js` | 护士 8 个端点（住院管理、体征录入、床位） |
| `api/pharmacist.js` | 药剂师 8 个端点（审核/发药/库存） |
| `api/patient.js` | 患者 12 个端点（挂号/支付/充值/个人信息） |

**为什么按角色拆分而不是按资源拆分**：
后端 API 本身按角色前缀隔离（`/api/admin/*`、`/api/doctor/*` 等），前端按角色拆分保持一致性，每个角色对应的页面只需 import 自己角色的 API 文件，代码更清晰。

#### 2.4 状态管理（`src/store/user.js`）

使用 Pinia 的 Composition API 风格（`defineStore` + `setup` 函数）：

```js
export const useUserStore = defineStore('user', () => {
  const token = ref(localStorage.getItem('token') || '')
  const userID = ref(localStorage.getItem('userID') || '')
  const username = ref(localStorage.getItem('username') || '')
  const role = ref(parseInt(localStorage.getItem('role') || '0'))

  function setLogin(data) {
    token.value = data.token
    userID.value = data.userID
    // ... 同时写入 localStorage 实现刷新页面不丢失登录态
  }

  function logout() {
    // 清空所有状态 + localStorage
  }

  return { token, userID, username, role, isLoggedIn, roleName, setLogin, logout }
})
```

**为什么用 localStorage 而不是 sessionStorage**：
用户在同一标签页刷新时保持登录。JWT 本身有过期时间（24h）保护安全性。

#### 2.5 路由设计（`src/router/index.js`）

**路由守卫**：未登录用户访问任何页面 → 重定向到 `/login`；已登录用户访问 `/login` → 重定向到 `/dashboard`。

**路由结构**：

```text
/login              → Login.vue（登录页）
/register           → Register.vue（注册页）
/                   → Layout.vue（主布局，requiresAuth）
  /dashboard        → admin/Dashboard.vue（首页概览）
  /admin/doctors    → admin/Doctors.vue（管理员 - 医生管理）
  ... (管理员 11 个子页面)
  /doctor/registrations → doctor/Registrations.vue（医生 - 挂号列表）
  ... (医生 4 个子页面)
  /nurse/hospitalizations → nurse/Hospitalizations.vue（护士 - 住院管理）
  ... (护士 4 个子页面)
  /pharmacist/medication-records → pharmacist/MedicationRecords.vue（药剂师 - 用药审核）
  ... (药剂师 3 个子页面)
  /patient/registrations → patient/Registrations.vue（患者 - 预约挂号）
  ... (患者 6 个子页面)
```

**所有路由组件使用动态导入** `() => import(...)` 实现 Vue Router 的懒加载，减小首屏体积。

#### 2.6 主布局设计（`src/views/Layout.vue`）

采用 Element Plus 的 `el-container` 布局：

```text
┌──────────────────────────────────────────────────┐
│  Header（面包屑 + 用户信息 + 退出按钮）            │
├─────────┬────────────────────────────────────────┤
│         │                                        │
│ Sidebar │  Main Content（router-view）            │
│ (菜单)  │                                        │
│         │                                        │
│         │                                        │
└─────────┴────────────────────────────────────────┘
```

**侧边栏菜单按角色动态显示**：

- 管理员看到：人员管理、医疗记录、资源管理 三大菜单组
- 医生看到：挂号列表、看诊管理、检查记录、个人信息
- 护士看到：住院管理、体征录入、床位管理、个人信息
- 药剂师看到：用药审核、药品库存、个人信息
- 患者看到：预约挂号、看诊记录、检查记录、用药记录、住院记录、个人信息

**实现关键**：`v-if="store.role === X"` 控制菜单项的显示，`router` 属性让 `el-menu` 自动根据路由高亮当前菜单项。

#### 2.7 登录/注册页面

**Login.vue**：

- `el-form` + `el-form-item` 表单布局
- `el-select` 选择角色（管理员/医生/护士/药剂师/患者）
- 表单校验规则（`el-form` 的 `rules` 属性）
- 登录成功 → `store.setLogin(res.data)` 保存 JWT → `router.push('/dashboard')`

**Register.vue**：

- 根据选择的角色动态显示额外字段（医生显示科室+职称，患者显示地址等）
- 密码确认校验（自定义 validator 比较两次输入）
- 注册成功后自动登录并跳转

#### 2.8 管理员页面（11 个子页面）

**通用页面模式**（以 Doctors.vue 为代表）：

1. **搜索栏**：`el-card` 包裹 `el-form :inline="true"`，支持按科室/姓名/职称筛选
2. **数据表格**：`el-table` + `el-table-column`，支持 `v-loading` 加载状态
3. **状态标签**：`el-tag` 显示在岗/锁定等状态，`:type` 动态切换颜色
4. **操作按钮**：编辑、封锁/激活、删除，固定在表格右侧（`fixed="right"`）
5. **分页**：前端分页（`list.slice(start, start + pageSize)`），显示总数和页码
6. **编辑弹窗**：`el-dialog` + `el-form`，修改后调用 `PUT` API 并刷新列表

**各页面功能**：

| 页面 | 搜索/过滤 | 表格列 | 编辑弹窗 | 特殊功能 |
|------|---------- | -------|--------- |--------- |

| Doctors.vue | 科室/姓名/职称 | ID/姓名/性别/年龄/科室/职称/电话/在岗/状态 | 姓名/性别/年龄/电话/科室/职称/专长 | 账号封锁/激活 |
| Nurses.vue | 科室 | ID/姓名/性别/年龄/科室/级别/电话/在岗/状态 | 姓名/性别/年龄/电话/科室/级别 | 账号封锁/激活 |
| Pharmacists.vue | - | ID/姓名/性别/年龄/科室/级别/电话/状态 | 姓名/性别/年龄/电话/科室/级别 | 账号封锁/激活 |
| Patients.vue | - | ID/姓名/性别/年龄/电话/余额/状态 | 姓名/性别/年龄/电话/紧急联系人 | 账号封锁/激活 |
| Registrations.vue | 科室/患者ID/医生ID/状态 | ID/患者/医生/科室/费用/状态/时间 | - | 状态修改弹窗 |
| Consultations.vue | 科室 | ID/患者/医生/科室/诊断/状态/时间 | - | 详情弹窗（el-descriptions） |
| Examinations.vue | - | ID/患者/医生/项目/费用/状态/时间 | - | 逻辑删除 |
| Hospitalizations.vue | - | ID/患者/医生/护士/病房/床位/状态/时间 | - | 逻辑删除 |
| MedicationRecords.vue | - | ID/患者/医生/费用/缴费状态/审核状态/时间 | - | 逻辑删除 |
| Medicines.vue | 科室 | ID/名称/规格/生产商/进价/售价/库存/安全库存/状态 | 全字段编辑 | - |
| Beds.vue | 科室 | ID/科室/病房类型/床位号/状态/患者/日费用 | - | 逻辑删除 |

**Dashboard.vue**：使用 `el-statistic` 组件展示 8 个统计数字（医生/护士/患者/挂号/看诊/检查/住院/药品总数），`Promise.all` 并行请求。

#### 2.9 医生页面（4 个子页面）

| 页面 | 功能 |
|------|------|

| Registrations.vue | 查看分配给自己的挂号列表（只读） |
| Consultations.vue | 查看看诊列表 + 编辑弹窗（主诉/现病史/既往史/家族史/初步诊断/建议住院/状态/备注） |
| Examinations.vue | 查看检查列表 + 编辑弹窗（报告摘要/状态/备注） |
| Profile.vue | 个人信息展示（`el-descriptions` 组件） |

#### 2.10 护士页面（4 个子页面）

| 页面 | 功能 |
|------|------|

| Hospitalizations.vue | 查看所有住院记录 + 编辑弹窗（床位号/状态修改） |
| Examinations.vue | 查看检查列表 + **体征录入弹窗**（体温/血压/心率/呼吸/血氧/身高/体重/血糖），自动计算 BMI |
| Beds.vue | 按科室筛选查看床位状态 |
| Profile.vue | 个人信息展示 |

**体征录入弹窗**是护士端的核心功能，对应后端 `PUT /api/nurse/examinations/:id/vitals` 端点，一次性提交 10 项体征数据。

#### 2.11 药剂师页面（3 个子页面）

| 页面 | 功能 |
|------|------|

| MedicationRecords.vue | 用药记录审核 + 发药。通过/驳回按钮（`reviewStatus=2/3`），发药按钮（需审核通过+已缴费，自动扣库存） |
| Medicines.vue | 药品库存管理。入库/出库按钮弹窗输入数量，调用 `PUT /api/pharmacist/medicines/:id/stock` |
| Profile.vue | 个人信息展示 |

#### 2.12 患者页面（6 个子页面）

| 页面 | 功能 |
|------|------|

| Registrations.vue | **预约挂号**（选科室→选医生→确认）+ 查看挂号列表 + **支付挂号费** |
| Consultations.vue | 查看看诊记录 + 详情弹窗 |
| Examinations.vue | 查看检查记录 + **支付检查费** |
| MedicationRecords.vue | 查看用药记录 + **支付药费**（需审核通过） |
| Hospitalizations.vue | 查看住院记录（只读） |
| Profile.vue | **个人信息编辑** + **账户充值**（`el-input-number` 输入金额，确认后调用 recharge API） |

**预约挂号流程**：

1. 点击"新建挂号" → 弹窗选择科室
2. 选择科室后自动加载该科室医生列表（调用 `GET /api/admin/doctors?department=xxx`）
3. 选择医生 → 确认 → 调用 `POST /api/patient/registrations` 创建挂号
4. 新挂号状态为"已预约"，可点击"支付"按钮从余额扣除挂号费

**支付流程**：

1. 点击"支付" → `ElMessageBox.confirm` 确认
2. 调用对应支付 API（挂号/检查/药费各有独立端点）
3. 后端检查余额是否充足 → 扣款 → 修改状态为"已缴费"

#### 2.13 构建与运行

```bash
# 安装依赖
cd frontend && npm install

# 开发模式（前端 3000 + 后端 8080，Vite 自动代理）
cd frontend && npm run dev
# 浏览器访问 http://localhost:3000

# 生产构建
cd frontend && npm run build
# 输出到 frontend/dist/，可由 Nginx 托管
```

**联调启动步骤**：

1. 先启动 C++ 后端：`cd build && ./Debug/his_server.exe`（监听 8080）
2. 再启动前端开发服务器：`cd frontend && npm run dev`（监听 3000）
3. 浏览器打开 `http://localhost:3000` → 进入登录页

#### 2.14 新增文件清单

```text
frontend/
├── index.html                    # SPA 入口
├── package.json                  # 依赖声明
├── vite.config.js                # Vite 配置（含 API 代理）
├── src/
│   ├── main.js                   # 应用入口（注册 Vue/Pinia/Router/ElementPlus）
│   ├── App.vue                   # 根组件
│   ├── styles/global.css         # 全局样式
│   ├── api/
│   │   ├── index.js              # axios 实例 + JWT 拦截器
│   │   ├── auth.js               # 登录/注册 API
│   │   ├── common.js             # 科室列表/费用标准
│   │   ├── admin.js              # 管理员 API（30+ 端点）
│   │   ├── doctor.js             # 医生 API（8 端点）
│   │   ├── nurse.js              # 护士 API（8 端点）
│   │   ├── pharmacist.js         # 药剂师 API（8 端点）
│   │   └── patient.js            # 患者 API（12 端点）
│   ├── store/
│   │   └── user.js               # Pinia 用户状态（JWT/角色/登录态）
│   ├── router/
│   │   └── index.js              # Vue Router（角色分流 + 路由守卫）
│   └── views/
│       ├── Login.vue             # 登录页
│       ├── Register.vue          # 注册页
│       ├── Layout.vue            # 主布局（侧边栏 + 头部 + 内容区）
│       ├── admin/
│       │   ├── Dashboard.vue     # 管理员首页概览
│       │   ├── Doctors.vue       # 医生管理（CRUD + 搜索过滤）
│       │   ├── Nurses.vue        # 护士管理
│       │   ├── Pharmacists.vue   # 药剂师管理
│       │   ├── Patients.vue      # 患者管理
│       │   ├── Registrations.vue # 挂号记录（搜索 + 状态修改）
│       │   ├── Consultations.vue # 看诊记录（搜索 + 详情查看）
│       │   ├── Examinations.vue  # 检查记录
│       │   ├── Hospitalizations.vue # 住院记录
│       │   ├── MedicationRecords.vue # 用药记录
│       │   ├── Medicines.vue     # 药品管理（CRUD + 编辑弹窗）
│       │   └── Beds.vue          # 床位管理
│       ├── doctor/
│       │   ├── Registrations.vue # 医生-挂号列表
│       │   ├── Consultations.vue # 医生-看诊管理（编辑弹窗）
│       │   ├── Examinations.vue  # 医生-检查记录（编辑弹窗）
│       │   └── Profile.vue       # 医生-个人信息
│       ├── nurse/
│       │   ├── Hospitalizations.vue # 护士-住院管理
│       │   ├── Examinations.vue  # 护士-体征录入（10项体征表单）
│       │   ├── Beds.vue          # 护士-床位列表
│       │   └── Profile.vue       # 护士-个人信息
│       ├── pharmacist/
│       │   ├── MedicationRecords.vue # 药剂师-用药审核/发药
│       │   ├── Medicines.vue     # 药剂师-库存管理（入库/出库）
│       │   └── Profile.vue       # 药剂师-个人信息
│       └── patient/
│           ├── Registrations.vue # 患者-预约挂号（选科室→选医生→支付）
│           ├── Consultations.vue # 患者-看诊记录查看
│           ├── Examinations.vue  # 患者-检查记录+支付
│           ├── MedicationRecords.vue # 患者-用药记录+支付药费
│           ├── Hospitalizations.vue # 患者-住院记录查看
│           └── Profile.vue       # 患者-个人信息编辑+充值
```

**共计 39 个文件**（3 个配置 + 1 个 HTML + 9 个 JS 基础层 + 2 个公共页面 + 24 个角色页面）

#### 2.15 关键设计决策

1. **前端分页**：后端返回全量数据（`{list, total}`），前端 `slice()` 分页。原因是当前数据量较小（链表数据通常几十到几百条），前端分页避免增加后端复杂度
2. **角色菜单隔离**：`v-if="store.role === X"` 控制侧边栏菜单显示，路由守卫只检查是否登录，具体权限由后端 JWT 中的 `role` 字段保证
3. **API 代理而非 CORS**：开发时使用 Vite `proxy` 转发请求到后端，避免浏览器跨域限制；生产环境通过 Nginx 反向代理实现
4. **无 Mock 数据**：由于后端 API 已完成，前端直接对接真实 API，无需 Mock.js 中间层
5. **Element Plus 中文 locale**：`import zhCn from 'element-plus/es/locale/lang/zh-cn'` 确保 `el-pagination`、`el-table` 空数据等提示为中文
