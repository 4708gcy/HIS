# HIS 改进 — 第 1/3 阶段：安全加固

> **喂给大模型时直接粘贴本文件全部内容。严格按任务顺序执行，每步编译验证。**
>
> **本阶段工时：** 约 3 小时 | **风险：低**（纯修补，不改变项目结构）
> **前提：** 项目当前已可编译运行

---

## 你的角色与行为准则

你是一名 C++17 工程师。你要为一个已上线的 HIS 医疗管理系统执行安全加固。

**行为准则：**
1. 任何文件修改前，必须先用 Read 工具读取目标文件的相关区域，确认当前代码状态与你的理解一致
2. 每次 Edit 只改当前任务要求的代码，不顺手重构无关逻辑
3. **每完成一个任务立即编译验证**：`cd build && cmake --build . --config Debug --target his`
4. 如果编译失败，停止后续任务，分析错误并修复。修复不了就回退该任务。

## 不可违反的约束

1. C++17 控制台 + MySQL 8.0 + Python Flask，不许引入 Java/Spring/Vue/React/Qt/Boost
2. 用户界面保持控制台终端，所有用户可见文本用简体中文
3. 21 张数据库表结构不变（只允许新增列，本阶段不涉及）
4. 业务流程、状态枚举值、ID 格式全部保持不变
5. 不许修改用户可见的中文字符串为英文

---

## 项目概况

```
HIS 医院信息系统 — C++17 控制台 + MySQL 8.0 + Python Flask AI v2.0

Head/ 分 4 子目录: Core/(基础7) Entities/(数据实体6) Roles/(角色5) Modules/(功能5) = 23 .h
Source/ 分 3 子目录: Core/(6) Roles/(5) Modules/(5) = 16 .cpp
Data/  MySQL配置 + Schema(21表) + DrugData(30组药物相互作用) + OperationLog
ai_service/  Python Flask v2.0 (12端点, pymysql+pandas+matplotlib, 4类PNG图表)

架构:     his.exe ←→ MySQL 8.0 (localhost:3307, his_db, 21表)
          his.exe ←→ Python AI Service (localhost:5001, HTTP) [可选]

数据流:   启动: MySQL SELECT → 13条内存双向链表 (raw pointer, prev/next)
          运行: 所有CRUD 操作内存链表
          退出: INSERT ON DUPLICATE KEY UPDATE → MySQL (每条链表一个事务, 出错回滚)
          崩溃: emergencySave() 尝试保存 (MySQL C API 非信号安全, 可能不可靠)

包含路径: CMake 设置了 include_directories(Head), 所以任何文件都可以
         #include "Core/User.h" / #include "Entities/Registration.h" 等方式引用头文件
```

---

## 任务 1.1：修复 std::cin 失败状态导致菜单无限循环

**严重程度：** 高 — 用户输入字母后程序进入不可恢复的死循环
**文件：** `Source/Core/UI.cpp`（主要）、`main.cpp`（配合）

### 问题描述

项目大量使用 `std::cin >> choice` 读取整数菜单选项（`main.cpp` 和 `UI.cpp` 中有几十处）。当用户误输入字母时：
1. `std::cin` 进入 `fail` 状态
2. 后续所有 `std::cin >>` 操作被跳过
3. `choice` 保持未初始化的旧值或 0
4. 菜单循环无限执行（因为 `choice` 始终不满足退出条件）

### 需要修改的具体函数

**在 `Source/Core/UI.cpp` 中找到并修改以下函数（至少 6 个）：**

| 函数 | 约位置 | 说明 |
|------|--------|------|
| `selectIntCheck(int min, int max)` | ~400 行 | 菜单选项输入，使用最频繁 |
| `inputIntCheck(...)` | ~310 行 | 通用整数输入 |
| `inputFeeCheck(...)` | ~330 行 | 费用输入 |
| `inputRecordIDCheck(...)` | ~350 行 | 记录 ID 输入 |
| 以及其他使用 `std::cin >> intVar` 模式的输入函数 | — | grep 搜索确认 |

**在 `main.cpp` 中找到直接使用 `std::cin >>` 的位置（约 3-5 处）**，将同样模式应用上去。

### 具体修改

**修改前（当前代码的模式）：**
```cpp
int selectIntCheck(int min, int max) {
    int choice;
    std::cin >> choice;
    std::cin.ignore(10000, '\n');
    if (choice < min || choice > max) {
        std::cout << "输入超出范围，请重新输入。" << std::endl;
        return selectIntCheck(min, max);  // 递归——fail状态下也会无限递归！
    }
    return choice;
}
```

**修改后：**
```cpp
int selectIntCheck(int min, int max) {
    int choice;
    while (true) {
        std::cout << "请输入选项 (" << min << "-" << max << "): ";
        if (std::cin >> choice) {
            // 成功读取整数
            std::cin.ignore(10000, '\n');
            if (choice >= min && choice <= max) {
                return choice;
            }
            std::cout << "输入超出范围，请重新输入。" << std::endl;
        } else {
            // 读取失败（用户输入了非数字）——清除错误状态
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "输入无效，请输入数字。" << std::endl;
        }
    }
}
```

**关键点：**
- `std::cin.clear()` 清除 fail 标志位
- `std::cin.ignore(10000, '\n')` 丢弃输入缓冲区中直到换行符的所有字符
- 不能用递归——用 `while(true)` 循环更安全
- 每个函数独立修改，不互相调用

**执行流程：**
1. 先 `grep -n "std::cin >>" Source/Core/UI.cpp` 找出所有位置
2. 逐个函数修改
3. 编译验证
4. 对 `main.cpp` 中的直接 `std::cin >>` 位置做同样修复

**编译验证：** `cd build && cmake --build . --config Debug --target his`

---

## 任务 1.2：API Key 配置化

**严重程度：** 中 — 硬编码密钥，编译后无法更改
**文件：** `Source/Core/User.cpp` + 新建 `Data/AdminAPIKey.txt`

### 问题描述

`Source/Core/User.cpp` 的 `signUp` 函数中，管理员注册的 API Key 校验直接硬编码了字符串 `"88888888"`：
```cpp
std::string apiKey = inputStringCheck("请输入管理员注册API密钥: ");
if (apiKey == "88888888")    // ← 硬编码！修改需要重新编译
```

应该从配置文件读取，这样修改密钥不需要重新编译。

### 具体修改

**第一步：创建配置文件**

新建 `Data/AdminAPIKey.txt`，内容仅一行（不含引号、不含换行后的空行）：
```
88888888
```

**第二步：修改 `Source/Core/User.cpp`**

找到 `signUp` 函数中 `inputStringCheck("请输入管理员注册API密钥: ")` 所在地位置。在其后面、`if (apiKey == "88888888")` 之前，插入以下代码：

```cpp
// 从配置文件读取期望的API密钥（默认值与之前保持一致）
std::string expectedKey = "88888888";
std::ifstream keyFile("../Data/AdminAPIKey.txt");
if (keyFile.is_open()) {
    std::getline(keyFile, expectedKey);
    // 去除可能的前后空白字符
    if (!expectedKey.empty()) {
        expectedKey.erase(0, expectedKey.find_first_not_of(" \t\r\n"));
        expectedKey.erase(expectedKey.find_last_not_of(" \t\r\n") + 1);
    }
    keyFile.close();
} else {
    // 配置文件不存在时使用默认值，并输出警告（方便开发调试）
    std::cerr << "[警告] Data/AdminAPIKey.txt 未找到，使用默认API密钥。" << std::endl;
}
```

**第三步：替换硬编码比较**

将原来的：
```cpp
if (apiKey == "88888888")
```
替换为：
```cpp
if (apiKey == expectedKey)
```

**第四步：确认头文件**

检查 `Source/Core/User.cpp` 顶部是否已经 `#include <fstream>`。如果没有，在已有的 `#include` 区域添加。

**编译验证。**

---

## 任务 1.3：AI 服务安全修复

**严重程度：** 中 — Flask debug 模式暴露 Werkzeug 调试器（可远程执行代码）
**文件：** `ai_service/app.py`

### 问题描述

当前 `app.py` 末尾：
```python
app.run(host='0.0.0.0', port=5001, debug=True)
```
三个问题：
1. `debug=True` — 暴露 Werkzeug 调试器，可执行任意 Python 代码
2. `host='0.0.0.0'` — 绑定所有网络接口，局域网内其他机器可访问
3. 无 API 认证 — 所有 12 个端点完全公开

### 具体修改

**第一步：修复 host 和 debug**

将末尾改为：
```python
app.run(host='127.0.0.1', port=5001, debug=False)
```

**第二步：添加 API Key 认证中间件**

在 `from flask import Flask, jsonify, request` 这一行**之后**（不是替换，是在下面新增），添加：

```python
import os
from functools import wraps

API_KEY = os.environ.get('HIS_API_KEY', 'his-default-key-change-me')

def require_api_key(f):
    """API Key 认证装饰器。请求头中必须包含 X-API-Key。"""
    @wraps(f)
    def decorated(*args, **kwargs):
        key = request.headers.get('X-API-Key', '')
        if key != API_KEY:
            return jsonify({'error': 'Unauthorized — 缺少有效的 API Key'}), 401
        return f(*args, **kwargs)
    return decorated
```

**第三步：保护所有 API 端点**

为每个 `/api/*` 路由添加 `@require_api_key` 装饰器。`/api/health` 可以不加（保留公开访问用于健康检查）。

修改前：
```python
@app.route('/api/predictions')
def predictions():
    ...
```

修改后：
```python
@app.route('/api/predictions')
@require_api_key
def predictions():
    ...
```

**需要保护的路由列表（逐个添加 `@require_api_key`）：**
- `/api/stats/monthly`
- `/api/predictions`
- `/api/anomalies`
- `/api/medicines`
- `/api/bed-optimization`
- `/api/forecast`
- `/api/dashboard`
- `/api/charts/predictions`
- `/api/charts/bed-utilization`
- `/api/charts/anomalies`
- `/api/charts/medicines`

**注意：** `@require_api_key` 放在 `@app.route(...)` 的**下一行**，`def` 的**上一行**。

**验证：** `cd ai_service && python -c "import ast; ast.parse(open('app.py', encoding='utf-8').read()); print('OK')"`

---

## 任务 1.4：SHA-256 迭代次数提升

**严重程度：** 低 — 1000 次低于现代标准，但不影响现有账户
**文件：** `Head/Core/User.h`（仅改一个数字）

### 问题描述

`Head/Core/User.h` 第 31 行：
```cpp
#define hashIterations 1000
```

OWASP 推荐至少 10000 次。虽然对于本地课设项目风险极低，但提升到 10000 展示安全意识。

### 具体修改

将：
```cpp
#define hashIterations 1000
```
改为：
```cpp
#define hashIterations 10000
```

**注意：** 已有账户的密码**不受影响**。旧账户登录时仍然用 1000 次验证（因为迭代次数存储在密码哈希的格式中？不——迭代次数是编译时常量。所以需要确认：`SHA256Verify` 和 `SHA256Encrypt` 都使用同一个宏。改宏后，新注册/新修改的密码用 10000 次，旧密码验证时会用 10000 次去验证 1000 次生成的哈希，导致旧账户无法登录。）

**⚠️ 等一下——这是个潜在的破坏性变更！** 需要先检查 `SHA256Verify` 的实现确认是否会破坏旧密码。

**执行流程：**
1. 先读取 `Source/Core/User.cpp` 的 `SHA256Verify` 和 `SHA256Encrypt` 函数，确认两者都使用 `hashIterations` 宏
2. 如果两者用同一个宏，直接改数字会导致旧密码无法验证
3. **如果会破坏兼容性：** 改为在注册/修改密码时使用新迭代次数，验证时先用新迭代次数尝试，失败后回退到旧迭代次数。具体做法：
```cpp
// SHA256Verify 中的回退逻辑：
bool ok = SHA256::SHA256Verify(inputPassword, storedHash, hashIterations);
if (!ok) {
    // 兼容旧密码（用旧迭代次数重试）
    ok = SHA256::SHA256Verify(inputPassword, storedHash, 1000);
}
```
4. 如果代码结构中无法加回退，则**跳过此任务**（标记为"无法安全执行"）

**编译验证。**

---

## 阶段一验收

```
cd build && cmake --build . --config Debug --target his
```
必须零错误零警告。

手动验证：
1. 启动程序 → 在任意菜单中输入字母 "abc" → 应提示"输入无效，请输入数字"而非死循环
2. 新管理员注册 → 输入 API Key `88888888` → 注册成功（如果配置文件存在）
3. 启动 Python AI 服务 `cd ai_service && python app.py` → 确认正常运行
4. 正常退出 → 重新启动 → 数据完整

**完成后请明确报告：** "阶段一全部完成，构建通过，N 个任务成功，M 个跳过。可以进入阶段二。"
