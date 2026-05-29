# HIS 改进 — 第 3/3 阶段：消除重复代码

> **喂给大模型时直接粘贴本文件全部内容。严格按任务顺序执行，每步编译验证。**
>
> **本阶段工时：** 约 6 小时 | **风险：高**（任务 3.3 必触发大量级联编译错误）
> **前提：** 阶段一、阶段二已完成且构建通过

---

## 你的角色与行为准则

你是一名 C++17 工程师。你要消除 HIS 项目中的代码重复和头文件循环依赖。

**关键：** 任务 3.3 是本阶段乃至全部三个阶段中最困难的一个。移除 User.h 的 6 个 include 后，**预计触发 20-50 个 "未定义类型" 编译错误**。这不是失败——这是预期行为。你需要逐个文件补 include，每次编译后检查错误列表，系统性地修复。

## 不可违反的约束

1. C++17 控制台 + MySQL 8.0 + Python Flask
2. 用户界面保持控制台，简体中文
3. 业务流程、状态枚举、ID 格式全部保持
4. 任务 3.3 遇到级联编译错误时，**逐个文件补 include，绝不回退到循环依赖**
5. 任务 3.1 和 3.2 每完成一个编译验证后再做下一个

---

## 任务 3.1：提取 Admin.cpp 中 7 类记录管理的公共遍历逻辑

**文件：** `Head/Core/UI.h`（新增模板函数）+ `Source/Roles/Admin.cpp`（替换 while 循环）
**难度：** 中等 | **风险：** 低

### 背景

`Source/Roles/Admin.cpp` 中 7 个记录管理函数（`manageRegistrations`、`manageConsultations`、`manageExaminations`、`manageHospitalizations`、`manageMedicationRecords`、`manageMedicines`、`manageBedInfo`）的"查看"子菜单包含大量重复的链表遍历代码。以"按科室查看"为例，每个函数里都有一套这样的模式：

```cpp
T *current = head;
bool found = false;
while (current != nullptr) {
    if (!current->isDeleted && current->department == department) {
        printXxx(current);
        found = true;
    }
    current = current->next;
}
if (!found) std::cout << "无记录。" << std::endl;
```

这 7 个函数中每个都有至少 3-5 种查看方式（全部/按ID/按科室/按状态/按时间），每种都是同样模式的 while 循环，总计约 30+ 处重复。

### 策略

不在这阶段做大重构。只提取最常用的模式——"按科室过滤并显示所有记录"——为一个模板函数。

### 具体修改

**第一步：在 `Head/Core/UI.h` 中新增模板函数**

找到 `pause()` 函数声明附近（约第 30 行附近），在它前面添加：

```cpp
// ====================== 通用链表遍历辅助 ======================

/// 遍历链表并调用打印函数，仅显示未删除且匹配科室的记录
/// @param head    链表头指针
/// @param dept    目标科室（"全院" 显示所有科室）
/// @param printer 打印回调，签名为 void(T*)
template<typename T>
void displayChainByDept(T *head, const std::string &dept,
                        std::function<void(T*)> printer) {
    T *cur = head;
    bool found = false;
    while (cur) {
        if (!cur->isDeleted && (dept == "全院" || cur->department == dept)) {
            printer(cur);
            found = true;
        }
        cur = cur->next;
    }
    if (!found)
        std::cout << "  无记录。" << std::endl;
}
```

**第二步：在 `Source/Roles/Admin.cpp` 中替换"按科室查看所有"**

找到 `manageRegistrations` 函数中的"查看全部（按科室过滤）"分支。将原有的完整 while 循环替换为：

```cpp
displayChainByDept<Registration>(regHead, department, [](Registration *r) {
    printRegistration(r);  // 使用已有的打印函数
});
```

**第三步：验证 + 扩展**

- 先替换 1 个函数 → 编译验证 → 运行程序确认该菜单项工作正常
- 再替换第 2 个函数（如 `manageConsultations`）→ 编译验证 → 运行确认
- 依次替换 7 个函数中所有"按科室查看全部"的 while 循环

**不替换的内容：** 按 ID 查询、按状态筛选、按时间范围筛选等——这些有额外逻辑的查询保持原样（当前阶段不改）。

### 编译验证

```
cd build && cmake --build . --config Debug --target his
```

---

## 任务 3.2：消除 Doctor.cpp 中两个函数的重复代码

**文件：** `Head/Roles/Doctor.h`（新增私有方法声明）+ `Source/Roles/Doctor.cpp`（提取公共逻辑）
**难度：** 低 | **风险：** 低

### 背景

`Source/Roles/Doctor.cpp` 中有两个函数存在约 90% 的代码重复：

| 函数 | 约行号 | 用途 |
|------|--------|------|
| `addConsultationExamination` | 910-951 | 在看诊过程中添加检查项目 |
| `initConsultationExamination` | 1133-1175 | 初始化看诊时添加检查项目 |

两个函数的核心逻辑相同：
1. 显示已有的检查项目列表
2. 用户输入新的检查项目名称
3. 查重（遍历 `target->examinationlist`）
4. 如果重复，报错；否则追加到列表

唯一区别：`initConsultationExamination` 末尾多一个空的 `return;` 语句。

### 具体修改

**第一步：在 `Head/Roles/Doctor.h` 中新增私有方法声明**

找到 Doctor 类的 `private:` 区域（约第 110 行），在现有私有方法声明之后添加：
```cpp
    // 向看诊记录添加检查项目（查重后追加）
    void addExaminationItem(Consultation *target, const std::string &itemName);
```

**第二步：在 `Source/Roles/Doctor.cpp` 中实现公共方法**

在文件末尾（或其他私有方法实现附近）添加：
```cpp
void Doctor::addExaminationItem(Consultation *target, const std::string &itemName) {
    // 查重
    for (const auto &item : target->examinationlist) {
        if (item == itemName) {
            printError("该检查项目已存在于此看诊记录中！");
            return;
        }
    }
    // 追加
    target->examinationlist.push_back(itemName);
    printSuccess("检查项目 \"" + itemName + "\" 已添加。");
}
```

**第三步：修改 `addConsultationExamination`**

找到约 910-951 行的函数体。识别出其中"查重 + 追加"的代码段（通常是一个 for 循环检查 + `push_back`），将其替换为：
```cpp
addExaminationItem(target, itemName);
```

保留"显示已有检查列表"和"用户输入"的代码不变，只替换查重+追加那段。

**第四步：修改 `initConsultationExamination`**

同样操作。替换后这个函数会变得更短——大部分逻辑由 `addExaminationItem` 处理了。

### 编译验证

---

## 任务 3.3：清理 User.h 中的循环依赖 ⚠️ 最困难

**文件：** `Head/Core/User.h`（移除 include）+ `Source/Core/User.cpp`（补 include）+ 约 12 个其他文件（逐个补 include）
**难度：** 高 | **风险：** 高 — 预计触发 20-50 个编译错误

### 背景

`Head/Core/User.h` 当前包含了 7 个头文件：

```cpp
#include "Entities/Registration.h"
#include "Entities/Consultation.h"
#include "Entities/Examination.h"
#include "Entities/Hospitalization.h"
#include "Entities/MedicationRecord.h"
#include "Entities/Medicine.h"
#include "Core/UI.h"
```

**问题：** User 是基类，所有 5 个角色子类都继承它，所有业务代码都包含 User.h。这 7 个 include 通过 User.h 间接传播到项目的几乎所有 .cpp 文件。修改任何 Entity 结构体都会触发全量重编译（40+ 个文件）。

**目标：** 将这 7 个 include 替换为最少量的前向声明，消除循环依赖链。

### 具体修改

**修改 `Head/Core/User.h`：**

将这 7 行：
```cpp
#include "Entities/Registration.h"
#include "Entities/Consultation.h"
#include "Entities/Examination.h"
#include "Entities/Hospitalization.h"
#include "Entities/MedicationRecord.h"
#include "Entities/Medicine.h"
#include "Core/UI.h"
```

替换为：
```cpp
// 前向声明（替代 6 个 Entity 头文件，消除循环依赖）
struct Registration;
struct Consultation;
struct Examination;
struct Hospitalization;
struct MedicationRecord;
struct Medicine;

#include "Core/UI.h"  // UI 是类，有内联方法被 User.h 使用，必须保留完整 include
```

**说明：** C++ 中 `struct X;` 和 `class X;` 的前向声明等价——只要代码中只使用 X 的指针、引用或声明参数类型，前向声明就足够了。User.h 中只在函数声明里以 `Registration *`、`ConsultationStatus` 等方式引用这些类型，前向声明完全满足。

**修改 `Source/Core/User.cpp`：**

在文件顶部添加 6 个 Entity 头文件的完整 include（因为 User.cpp 使用了这些类型的枚举转换函数和成员）：
```cpp
#include "Entities/Registration.h"
#include "Entities/Consultation.h"
#include "Entities/Examination.h"
#include "Entities/Hospitalization.h"
#include "Entities/MedicationRecord.h"
#include "Entities/Medicine.h"
```

放在现有 `#include "Core/User.h"` 之后、其他 include 之前。

**编译。** 此时预计出现 20-50 个 "未定义类型" / "incomplete type" 错误。

### 处理级联编译错误的系统化方法

**错误特征：**
- MSVC: `C2027: 使用了未定义类型 "Registration"`
- MSVC: `C2079: "xxx" 使用了未定义的 class "Consultation"`
- GCC/Clang: `error: invalid use of incomplete type 'struct Examination'`
- 编译输出中文件名 + 行号明确指出哪个文件缺少 include

**修复流程（重复执行直到零错误）：**

1. 编译一次，收集所有错误信息
2. 按文件名分组——同一个文件可能缺多个 include
3. 对每个报错文件，在顶部添加它直接需要的头文件

**以下是各个文件最可能需要补的 include（根据实际依赖关系整理）：**

| 报错文件 | 缺少的类型 | 需添加的 include |
|----------|-----------|-----------------|
| `Source/Roles/Admin.cpp` | Registration, Consultation, Examination, Hospitalization, MedicationRecord, Medicine | `#include "Entities/Registration.h"` 等全部 6 个 |
| `Source/Roles/Doctor.cpp` | Registration, Consultation, Examination, Medicine | 需逐个确认 |
| `Source/Roles/Nurse.cpp` | Hospitalization, Examination, Consultation | 需逐个确认 |
| `Source/Roles/Pharmacist.cpp` | Medicine, MedicationRecord, Consultation | 需逐个确认 |
| `Source/Roles/Patient.cpp` | Registration, Consultation, Examination, Hospitalization, MedicationRecord | 需逐个确认 |
| `Source/Core/Login.cpp` | Admin, Doctor, Pharmacist (角色类在 Roles/ 中，不受影响) | 可能不需要额外的 Entity include |
| `Source/Modules/LoadData.cpp` | 全部 6 个 Entity | 很可能已有自己的 include |
| `Source/Modules/SaveData.cpp` | 全部 6 个 Entity | 很可能已有自己的 include |
| `Source/Core/UI.cpp` | Registration, Consultation, etc. | 需逐个确认 |
| `Source/Modules/DataAnalysis.cpp` | Hospitalization, Registration, Medicine | 需逐个确认 |
| `Source/Modules/DrugSafety.cpp` | Medicine (DrugSafety.h 已有自己的 include) | 可能不需要 |
| `Source/Modules/AIQueryClient.cpp` | 不直接使用 Entity | 可能不需要 |
| `main.cpp` | 全部类型（main.cpp 有大量业务代码） | 需逐个确认 |

**修复每批后的操作：**
1. `cd build && cmake --build . --config Debug --target his`
2. 如果错误数量减少，继续下一批
3. 如果出现新类型的错误（如链接错误），记录并分析
4. **绝对禁止：** 回到 User.h 把 7 个 include 加回去。那是退步。

**预期迭代次数：** 3-5 轮编译-修复循环。每轮错误数量约减少一半。

---

## 全局验收

```
cd build && cmake --build . --config Debug --target his
```
必须零错误零警告。

完整业务流程测试：
1. 启动 → 管理员登录 → 菜单中输字母 "abc" → 提示重新输入而非死循环（阶段一修复）
2. 新管理员注册 → API Key `88888888` → 注册成功（阶段一修复）
3. 医生登录 → 创建看诊 → 开处方（DrugSafety 过敏检查触发）→ 开检查单
4. 护士登录 → 查看住院患者 → assignBed → 创建护理记录 → 查看体征趋势
5. 药剂师登录 → 审核处方（无过敏冲突可批准，有冲突被阻止）→ 发药扣库存
6. 患者登录 → 预约挂号 → 支付 → 查看看诊记录 → 查看用药记录
7. 管理员 → 统计报表（SQL GROUP BY）→ AI 智能分析 → 生成图表 PNG
8. 正常退出 → 重新启动 → 所有数据完整

---

## 禁止事项

1. ❌ 不许引入 Java/Spring/Vue/React/Qt
2. ❌ 不许删除/重命名数据库表
3. ❌ 不许修改 ID 格式或业务状态枚举值
4. ❌ 不许删除 AI 服务代码
5. ❌ 不许引入 Boost、Qt 等外部依赖
6. ❌ 不许使用 "TODO" 或 "implement later" 占位
7. ❌ 不许在未编译验证的情况下连续提交多个改动
8. ❌ 不许修改用户可见的中文字符串为英文
9. ❌ **不许在任务 3.3 遇到级联编译错误时回退到循环依赖**——逐个文件补 include

---

**完成后请明确报告：** "三阶段全部完成。共修改 X 个文件，构建零错误，全部验收流程通过。"
