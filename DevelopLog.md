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
