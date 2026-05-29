# HIS 改进 — 第 2/3 阶段：实体仓库扩展

> **喂给大模型时直接粘贴本文件全部内容。严格按任务顺序执行，每步编译验证。**
>
> **本阶段工时：** 约 6 小时 | **风险：中**（涉及模板特化，需仔细处理编译器行为）
> **前提：** 阶段一已完成且构建通过（`cd build && cmake --build . --config Debug --target his` 零错误）

---

## 你的角色与行为准则

你是一名 C++17 工程师。你的任务是将项目现有的 `EntityRepository<T>` 泛型模板扩展到 Patient 和 Registration 两条链表，实现自动生命周期管理。

**行为准则：**
1. 任何文件修改前，必须先用 Read 工具读取目标文件的相关区域
2. 每次 Edit 只改当前任务要求的代码
3. **每完成一个任务立即编译验证**
4. 如果编译失败，根据错误信息精确修复，不要猜测

## 不可违反的约束

1. C++17 控制台 + MySQL 8.0 + Python Flask。不许引入任何外部框架
2. 用户界面保持控制台终端，所有用户可见文本用简体中文
3. 业务流程、状态枚举值、ID 格式全部保持
4. **不改 EntityRepository 核心逻辑**——只移动 `getIdField` 的位置 + 添加特化
5. 本阶段只将 Patient 和 Registration 接入 Repository，**不碰其他 11 条链**

---

## 背景：EntityRepository 模板当前状态

`Head/Core/EntityRepository.h` 是一个泛型模板类（145 行），为链表提供：
- O(1) ID 查找（`std::unordered_map` 索引）
- 头插法 `add()`（自动生成 ID）
- 逻辑删除 `remove()`
- 过滤查询 `filter(std::function<bool(const T*)>)`
- 析构时自动 `clear()` 所有节点

**当前仅用于 NursingRecord 链：**
```cpp
// main.cpp 中：
EntityRepository<NursingRecord> nursingRecordRepo(nursingRecordCount, "nur", 6);
// ...
nursingRecordRepo.setHead(nursingRecordHead);  // 接管所有权
// 程序退出时 nursingRecordRepo 析构 → clear() → 自动 delete 所有节点
```

**限制：** 模板内部通过 `getIdField(T*)` 获取实体 ID 字段，默认返回 `entity->recordID`。但对于 Patient（`patientID`）和 Registration（`registrationID`），字段名不同，需要模板特化。

---

## 任务 2.1：理解现状（只读，不改任何文件）

**目标：** 确保你理解了以下关键信息后再动手修改。

### 需要读取的文件

1. **`Head/Core/EntityRepository.h`** — 全文阅读。重点关注：
   - 约第 14-18 行：private 成员（`head`, `idCounter`, `idPrefix`, `idDigits`, `idMap`）
   - 约第 20-30 行：`rebuildIndex()` — 内部调用 `getIdField(cur)` 建立索引
   - 约第 33 行：`static std::string& getIdField(T *entity) { return entity->recordID; }` — **当前是 private**
   - 约第 35-130 行：public 接口（构造、析构、findById、add、remove、getAll、filter、clear 等）

2. **`main.cpp` 约第 30-32 行：**
   ```cpp
   int nursingRecordCount = 0;
   EntityRepository<NursingRecord> nursingRecordRepo(nursingRecordCount, "nur", 6);
   ```
   看 NursingRecord 的 Repository 如何声明和初始化。

3. **`main.cpp` 约第 140-141 行：**
   ```cpp
   NursingRecord *nursingRecordHead = loadNursingRecords(nursingRecordCount);
   nursingRecordRepo.setHead(nursingRecordHead);
   ```
   看加载后如何 `setHead()` 交给 Repository 管理。

4. **`main.cpp` 约第 541 行：**
   ```cpp
   // nursingRecordHead 由 nursingRecordRepo 析构时自动清理
   ```
   看旧的 `while(ptr){delete ptr; ptr=next;}` 清理循环如何被 Repository 替代。

### 确认理解

- `getIdField` 是 private 方法，需要在类外做特化 → 必须先移为 public
- `setHead()` 接管链表所有权后，不能再手动 `delete` 节点
- Repository 构造函数参数：`(int &counter, string idPrefix, int idDigits)` — 引用计数器，自动 ID 生成

---

## 任务 2.2：将 getIdField 移到 public + 添加 Patient/Registration 特化

**文件：** `Head/Core/EntityRepository.h`（两处修改）

### 第一步：将 getIdField 从 private 移到 public

找到约第 33 行的 `getIdField`：
```cpp
private:
    T *head = nullptr;
    int &idCounter;
    std::string idPrefix;
    int idDigits;
    std::unordered_map<std::string, T *> idMap;

    void rebuildIndex() { ... }

    // 默认 ID 字段访问器 — 子类可特化
    static std::string &getIdField(T *entity) { return entity->recordID; }

public:
    EntityRepository(...) { ... }
```

将 `getIdField` 这一行**剪切**，粘贴到 public 区域（建议放在 `clear()` 方法声明之后，约第 130 行的位置）。修改后结构应为：

```cpp
private:
    T *head = nullptr;
    int &idCounter;
    std::string idPrefix;
    int idDigits;
    std::unordered_map<std::string, T *> idMap;

    void rebuildIndex() { ... }
    // getIdField 已移走

public:
    EntityRepository(...) { ... }
    ~EntityRepository() { clear(); }
    // ... 其他 public 方法 ...
    void clear() { ... }

    // ID 字段访问器 — 默认返回 recordID，各实体类型可特化
    static std::string &getIdField(T *entity) { return entity->recordID; }
```

**注意：** 移动时不要改变函数体的任何字符。确保 private 中其他 5 个成员不受影响。

### 第二步：在 `#endif` 前添加模板特化

在文件末尾 `#endif // ENTITY_REPOSITORY_H` **之前**，添加两个显式特化：

```cpp
// ==================== 实体类型 ID 字段特化 ====================

// Patient 使用 patientID 作为 ID 字段（而非默认的 recordID）
template<>
inline std::string& EntityRepository<Patient>::getIdField(Patient *entity) {
    return entity->patientID;
}

// Registration 使用 registrationID 作为 ID 字段
template<>
inline std::string& EntityRepository<Registration>::getIdField(Registration *entity) {
    return entity->registrationID;
}

#endif // ENTITY_REPOSITORY_H
```

**⚠️ 关键点：** 使用 `inline` 关键字避免多重定义链接错误。`template<>` 表示这是显式（全）特化。

### 编译验证

```
cd build && cmake --build . --config Debug --target his
```

**如果编译报错 "explicit specialization is not in a namespace enclosing"：** 检查特化代码是否写在了类定义体内部。必须写在 `};` 闭合花括号之后、`#endif` 之前。

**如果编译报错 "cannot access private member"：** getIdField 没有成功移到 public 区域。重新执行第一步。

---

## 任务 2.3：将 EntityRepository 应用到 Patient 链

**文件：** `main.cpp`（三处修改）

### 背景

`main.cpp` 中 Patient 链的当前生命周期：
```cpp
// 第 19 行：全局计数器
int patientIDCount = 0;

// 第 131 行：加载
Patient *patientHead = loadPatientData(patientIDCount);

// 第 533 行：清理（手动 while 循环 delete）
while (patientHead) { Patient *n = patientHead->next; delete patientHead; patientHead = n; }
```

需要改为 Repository 管理（自动清理，无需手动 delete）。

### 第一步：确认头文件

检查 `main.cpp` 顶部是否已有 `#include "Core/EntityRepository.h"`（第 6 行附近，应已存在）。如果不存在，添加：
```cpp
#include "Core/EntityRepository.h"
```

### 第二步：声明 Repository 实例

在全局变量区域，约第 30 行 `EntityRepository<NursingRecord>` 声明之后，添加：
```cpp
EntityRepository<Patient> patientRepo(patientIDCount, "4", 5);
//                                     ^引用全局计数器  ^ID前缀  ^数字位数
```

放在 `patientIDCount` 声明（约第 19 行）之后，`nursingRecordRepo` 声明附近。

### 第三步：将加载的链表头交给 Repository

约第 131 行，在 `Patient *patientHead = loadPatientData(patientIDCount);` **之后**，添加：
```cpp
patientRepo.setHead(patientHead); // 交给 EntityRepository 管理生命周期
```

参考上下文中 NursingRecord 的 `nursingRecordRepo.setHead(nursingRecordHead)` 模式。

### 第四步：移除手动清理

约第 533 行，找到：
```cpp
while (patientHead) { Patient *n = patientHead->next; delete patientHead; patientHead = n; }
```

改为注释：
```cpp
// patientHead 由 patientRepo 析构时自动清理（EntityRepository::~EntityRepository → clear()）
```

**注意：**
- 不要删除这行周围的 try-catch 结构
- 不要删除其他链（admin/doc/nurse 等）的手动清理
- Patient 的业务代码（`Source/Roles/Patient.cpp`）**不需要任何修改**——它们继续通过 `patientHead` 裸指针遍历链表

### 编译验证

---

## 任务 2.4：将 EntityRepository 应用到 Registration 链

**文件：** `Head/Core/EntityRepository.h`（确认特化存在）+ `main.cpp`（三处修改）

与任务 2.3 完全相同的模式，只是换成了 Registration：

### 第一步：确认特化

任务 2.2 已添加 `EntityRepository<Registration>::getIdField` 特化。确认它存在于 `Head/Core/EntityRepository.h` 的 `#endif` 之前。

### 第二步：声明 Repository

在全局变量区域添加：
```cpp
EntityRepository<Registration> regRepo(registrationCount, "reg", 6);
```

### 第三步：setHead

在 `Registration *regHead = loadRegistrations(registrationCount);` 之后添加：
```cpp
regRepo.setHead(regHead);
```

### 第四步：移除手动清理

将：
```cpp
while (regHead) { Registration *n = regHead->next; delete regHead; regHead = n; }
```
改为注释：`// regHead 由 regRepo 析构时自动清理`

### 编译验证

---

## 阶段二验收

```
cd build && cmake --build . --config Debug --target his
```
必须零错误零警告。

手动验证（以下每步都必须通过）：
1. 正常启动程序 → 能查看患者列表 → **Patient 加载正常**
2. 查看挂号和看诊记录 → **Registration 加载正常**
3. 以患者身份登录 → 挂号 → 支付 → 正常退出
4. 重新启动程序 → 刚才创建的挂号记录仍然存在 → **保存正常**
5. 正常退出 → 无内存泄漏崩溃 → **Repository 析构清理正常**

**完成后请明确报告：** "阶段二全部完成。Patient 和 Registration 已接入 EntityRepository，构建通过，手动验证通过。可以进入阶段三。"
