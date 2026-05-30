# AGENTS.md — HIS 医院信息管理系统

> 本文件面向 AI 编程助手。读者应被假设为对该项目一无所知。

## 项目概述

**HIS（Hospital Information System）** 是一个适用于小型医院的轻量级医疗管理系统，为大学《程序设计基础课程设计》课程项目。系统采用 C++17 开发，基于 Windows 控制台的中文终端交互界面，数据持久化依赖 MySQL 8.0，并附带一个可选的 Python Flask AI 微服务用于数据分析与可视化。

系统管理 5 种用户角色（管理员、医生、护士、药剂师、患者）和 8 类医疗业务记录（挂号、看诊、检查、住院、用药记录、药品库存、床位、护理记录），支持科室划分、权限隔离、统计报表、AI 智能分析等功能。

## 技术栈与运行时架构

| 层级 | 技术 |
|------|------|
| 主程序 | C++17，MSVC（`/utf-8`），CMake 3.24+ |
| 数据库 | MySQL 8.0（C API `libmysql`） |
| AI 服务 | Python 3.8+，Flask，pymysql，pandas，matplotlib，numpy |
| 开发环境 | Windows + VS Code（`cppvsdbg` 调试器） |

### 运行时架构

```
┌─────────────────────────────────────────┐
│  HIS 控制台程序 (his.exe)                │
│  • C++17 终端 UI                         │
│  • 13 条内存双向链表                     │
│  • MySQL C API 读写                      │
│  • 信号处理 + 紧急保存                   │
└────────────┬────────────────────────────┘
             │ SQL (TCP)
┌────────────▼────────────────────────────┐
│  MySQL 8.0 Server (localhost:3307)      │
│  数据库: his_db（21 张表）               │
└─────────────────────────────────────────┘
             │ HTTP (可选)
┌────────────▼────────────────────────────┐
│  AI 微服务 (Python Flask, :5001)         │
│  • 需求预测 / 异常检测                   │
│  • 药品库存分析 / 床位优化               │
│  • 图表生成 (PNG)                        │
└─────────────────────────────────────────┘
```

## 目录结构

```
.
├── main.cpp                      # 程序入口：信号处理、数据库连接、主循环、保存与清理
├── CMakeLists.txt                # CMake 配置：C++17、/utf-8、libmysql 链接
├── Head/                         # 25 个头文件，分 4 个子目录
│   ├── Core/                     # 7 文件：基础类
│   │   ├── User.h                # 用户基类（通用属性、安全认证、枚举转换、费用计算）
│   │   ├── Database.h            # MySQL C API 封装（单例、事务、参数化查询）
│   │   ├── UI.h                  # 80+ 菜单函数、输入校验、终端颜色、日志系统
│   │   ├── Login.h               # 登录验证、账户封锁/激活管理
│   │   ├── SHA-256.h             # SHA-256 哈希算法
│   │   ├── GetTime.h             # 时间获取工具
│   │   └── EntityRepository.h    # 模板类：O(1) ID 查找、逻辑删除、链表管理
│   ├── Entities/                 # 7 文件：数据实体结构体
│   │   ├── Registration.h        # 挂号记录
│   │   ├── Consultation.h        # 看诊记录（含处方子表、检查项目子表）
│   │   ├── Examination.h         # 检查记录（含生命体征、附件子表）
│   │   ├── Hospitalization.h     # 住院记录 + bedInfo
│   │   ├── MedicationRecord.h    # 用药记录（含药品明细子表）
│   │   ├── Medicine.h            # 药品信息（库存、有效期、别名）
│   │   └── NursingRecord.h       # 护理记录（独立实体，之前嵌套在 User.h 中）
│   ├── Roles/                    # 5 文件：用户角色类
│   │   ├── Admin.h               # 管理员（账户管理、报表、全局管理）
│   │   ├── Doctor.h              # 医生（科室、职称、排班、工作量统计）
│   │   ├── Nurse.h               # 护士（等级、床位管理、护理记录）
│   │   ├── Pharmacist.h          # 药剂师（处方审核、发药、库存）
│   │   └── Patient.h             # 患者（身份证号、过敏史、余额、婚姻状态）
│   └── Modules/                  # 6 文件：功能模块
│       ├── DrugSafety.h          # 药品相互作用（成分级精确匹配，防假阳性）
│       ├── DataAnalysis.h        # Holt-Winters 预测、Z-score 异常检测
│       ├── AIQueryClient.h       # C++ HTTP 客户端（WinSock2，PNG 二进制安全下载）
│       ├── LoadData.h            # 13 个 MySQL→内存链表加载函数
│       └── SaveData.h            # 13 个内存链表→MySQL 保存函数（事务）
├── Source/                       # 18 个实现文件，分 3 个子目录
│   ├── Core/                     # 6 文件
│   │   ├── User.cpp, Database.cpp, UI.cpp, Login.cpp, SHA-256.cpp, GetTime.cpp
│   ├── Roles/                    # 8 文件（Admin 拆分为 4 文件）
│   │   ├── Admin.cpp             # 注册/登录 + include 聚合
│   │   ├── AdminRecords.cpp      # 7 类记录管理
│   │   ├── AdminReports.cpp      # 6 个统计报表
│   │   ├── AdminUsers.cpp        # 4 类用户管理 + 个人信息
│   │   ├── Doctor.cpp, Nurse.cpp, Pharmacist.cpp, Patient.cpp
│   └── Modules/                  # 5 文件
│       ├── DrugSafety.cpp, DataAnalysis.cpp, AIQueryClient.cpp, LoadData.cpp, SaveData.cpp
├── Data/
│   ├── DatabaseConfig.txt        # MySQL 连接配置（host, port, user, password, database）
│   ├── Schema/
│   │   ├── schema.sql            # 21 张表的 CREATE TABLE + 外键 + CHECK 约束
│   │   └── migrate_csv_to_mysql.py  # 历史 CSV 数据迁移脚本
│   ├── DrugData/
│   │   └── interactions.txt      # 30 组药品相互作用规则（文件读取）
│   └── OperationLog/             # 每日操作日志（his_YYYY_MM_DD.log）
├── ai_service/                   # Python AI 微服务 v2.0
│   ├── requirements.txt          # flask, flask-cors, numpy, pymysql, pandas, matplotlib
│   ├── app.py                    # Flask REST API（12 个端点）
│   ├── data_loader.py            # 从 MySQL 加载数据供分析使用
│   ├── analyzer.py               # 预测/异常检测/库存分析/床位优化算法
│   ├── charts.py                 # matplotlib 图表生成（4 类 PNG）
│   └── charts/                   # 生成的图表输出目录
├── Document/                     # 课程文档（题签、总结报告、开发日志、功能简图）
└── .vscode/                      # VS Code 任务与调试配置
    ├── tasks.json
    └── launch.json
```

## 构建与运行命令

### 前置条件

1. **MySQL 8.0** 运行在 `localhost:3307`，用户 `root`，密码 `123456`，数据库 `his_db`。
2. **MySQL C Client** 安装在 `E:/MySQL/8-0/`（`include/` + `lib/libmysql.lib` + `lib/libmysql.dll`）。
3. **CMake 3.24+** 与 **MSVC**（Visual Studio 2017 15.8+）。

### 首次初始化

```bash
# 1. 启动 MySQL 服务
net start MySQL80

# 2. 初始化数据库（仅需一次）
mysql -u root -p123456 -P 3307 < Data/Schema/schema.sql

# 3. 如需导入历史 CSV 数据
python Data/Schema/migrate_csv_to_mysql.py
```

### 构建主程序

```bash
# 配置（CMakeLists.txt 变更后需重新执行）
mkdir -p build && cd build && cmake ..

# 编译
cmake --build . --config Debug --target his

# 确保 DLL 在可执行文件同级目录
cp E:/MySQL/8-0/lib/libmysql.dll build/Debug/

# 运行（工作目录必须是 build/，因为使用 ../Data/ 相对路径）
cd build && ./Debug/his.exe
```

### 启动 AI 服务（可选）

```bash
cd ai_service
pip install -r requirements.txt
python app.py
# 服务运行在 http://localhost:5001
```

### VS Code 调试

使用配置 `"调试 HIS 程序"`（`launch.json`）：
- **预启动任务**：`CMake: 编译`
- **可执行文件**：`build/Debug/his.exe`
- **工作目录**：`build/`
- **控制台**：外部终端

> ⚠️ **注意**：`postDebugTask` 会在调试结束后执行 `Remove-Item -Recurse -Force build/*`，这会删除整个构建目录（包括 `libmysql.dll`），下次调试前需重新复制 DLL。

## 代码组织与模块划分

### 内存数据模型

程序运行时维护 **13 条双向链表**，全部驻留内存：

| 链表 | 节点类型 | ID 前缀 | 说明 |
|------|----------|---------|------|
| 管理员 | `Admin` | `0` + 5 位数字 | 全局管理 |
| 医生 | `Doctor` | `1` + 5 位数字 | 含职称、科室、排班 |
| 护士 | `Nurse` | `2` + 5 位数字 | 含等级、护理计数 |
| 药剂师 | `Pharmacist` | `3` + 5 位数字 | 含审核/发药计数 |
| 患者 | `Patient` | `4` + 5 位数字 | 含余额、过敏史、住院标记 |
| 挂号记录 | `Registration` | `reg` + 6 位数字 | |
| 看诊记录 | `Consultation` | `con` + 6 位数字 | 含处方子表、检查项目子表 |
| 检查记录 | `Examination` | `exa` + 6 位数字 | 含附件子表 |
| 住院记录 | `Hospitalization` | `hos` + 6 位数字 | |
| 用药记录 | `MedicationRecord` | `mrd` + 6 位数字 | 含药品明细子表 |
| 药品信息 | `Medicine` | `med` + 6 位数字 | 库存、有效期 |
| 床位信息 | `bedInfo` | `N-03-P-001-01` 格式 | 科室-楼层-病房-床位 |
| 护理记录 | `NursingRecord` | `nur` + 6 位数字 | 由 `EntityRepository<NursingRecord>` 管理 |

每条链表的节点均含 `prev`/`next` 指针。新节点采用**头插法**。删除操作为**逻辑删除**（设置 `isDeleted = true`）。

### 类继承体系

```
User（虚析构函数）
├── Admin
├── Doctor
├── Nurse
├── Pharmacist
└── Patient
```

### 关键子系统

- **Database**（`Database.h/cpp`）：MySQL C API 单例封装。提供 `connect()`、`query()`、`execute()`、参数化查询（`queryPrepared`/`executePrepared`）、事务（`beginTransaction`/`commit`/`rollback`）及 NULL 安全的行读取辅助函数。
- **持久化层**（`LoadData.h/cpp`、`SaveData.h/cpp`）：13 对加载/保存函数。加载时从 MySQL `SELECT` 到链表；保存时用 `INSERT ... ON DUPLICATE KEY UPDATE` 同步回数据库，每条链表一个事务，出错自动回滚。
- **认证层**（`Login.h/cpp`、`SHA-256.h/cpp`）：带盐 SHA-256 哈希，10000 次迭代（`verifyPasswordCompat()` 向后兼容旧 1000 次迭代密码）。连续失败 5 次锁定账户。管理员注册 API Key 三级优先级：环境变量 `HIS_ADMIN_API_KEY` → 配置文件 `Data/AdminAPIKey.txt` → 默认 `88888888`。
- **UI 层**（`UI.h/cpp`）：80+ 菜单函数、20+ 输入校验函数、`LogManager` 单例日志、Unicode 制表符边框、CJK 宽度感知、终端彩色输出。安全清屏（Win32 API / ANSI 转义，无 `system()` 调用）。6 个 `printXxxCard()` 函数集中管理实体显示。`displayChainByDept<T>` 和 `displayChainByFilter<T>` 通用链表遍历模板。
- **EntityRepository**（`EntityRepository.h`）：泛型模板，为链表提供 O(1) ID 查找（`unordered_map` 索引）、逻辑删除、过滤查询。`getIdField()` 返回 `const std::string&`（ID 不可变）。模板特化管理 3 条链：`NursingRecord`、`Patient`、`Registration`。
- **AI 查询客户端**（`AIQueryClient.h/cpp`）：基于 WinSock2 的 HTTP 客户端，连接 `127.0.0.1:5001`，提供 `getPredictions()`、`getAnomalies()`、`getMedicines()`、`getBedOptimization()`、`getDashboard()`、`downloadChart()` 等方法。`downloadChart()` 使用二进制安全接收（Content-Length 解析 + `std::ios::binary` 写入）。
- **数据分析**（`DataAnalysis.h/cpp`）：Holt-Winters 需求预测、Z-score 异常检测、床位利用率分析。
- **药品安全**（`DrugSafety.h/cpp`）：基于 `Data/DrugData/interactions.txt` 的 30 组药品相互作用规则引擎。`tokenizeDrugName()` 实现成分级精确匹配（按分隔符拆分药品名），消除子字符串假阳性（如"阿莫西林克拉维酸钾"不会误匹配"阿莫西林"）。

## 开发约定

### 命名规范

- **类名**：`PascalCase`（如 `EntityRepository`、`LogManager`）
- **函数名**：`camelCase`（如 `adminSignUp`、`manageDoctors`）
- **变量/成员**：`camelCase`（如 `loginAttempts`、`isDeleted`）
- **全局计数器**：`g_` 前缀仅用于信号处理中的紧急保存指针（如 `g_adminHead`），其余全局 ID 计数器不使用前缀
- **宏常量**：全大写（如 `failedLoginLimit`、`hashIterations`）
- **枚举**：`PascalCase`，枚举值全大写或 `PascalCase`

### 空值约定

- 字符串字段的**空值哨兵**为 `"#"`。在数据库层通过 `dbStr()` / `dbDateTime()` 辅助函数映射为 SQL `NULL`。
- 所有货币金额以**分（int）**存储，显示时除以 100.0。禁止用 `double` 进行金额比较。

### 注释与文档语言

- **所有用户可见字符串、注释、文档使用简体中文**。
- 代码标识符（类名、函数名、变量名）使用英文。
- 新增面向用户的文本必须保持中文。

### 文件组织

- 声明放 `Head/Core/` / `Head/Entities/` / `Head/Roles/` / `Head/Modules/`，实现放 `Source/Core/` / `Source/Roles/` / `Source/Modules/`。
- `main.cpp` 仅包含入口逻辑、信号处理、全局计数器定义、主循环，不放置业务实现。
- `CMakeLists.txt` 使用 `file(GLOB)` 自动收集 `Source/Core/*.cpp`、`Source/Roles/*.cpp`、`Source/Modules/*.cpp` 和 `main.cpp`。新增 `.cpp` 文件后需要重新运行 `cmake ..`。

### 信号处理与紧急保存

- 程序注册了 `SIGINT`、`SIGABRT`、`SIGTERM` 的信号处理函数。
- 收到信号时调用 `emergencySave()`，尝试将内存链表写回文件（**注意**：MySQL C API 非信号安全，紧急保存可能不可靠）。
- 全局双指针（如 `static Admin **g_adminHead`）指向局部 `adminHead` 变量的地址，用于信号处理器访问当前链表头。

## 测试策略

**本项目没有自动化测试套件。** 验证方式如下：

1. **编译测试**：确保 `cmake --build build --config Debug --target his` 无警告/错误。
2. **手动交互测试**：运行程序，逐菜单验证：
   - 首次启动无管理员时强制创建管理员（需 API Key `88888888`）
   - 各角色登录、注册、登出流程
   - 各角色的 CRUD 操作（增删改查）
   - 管理员报表生成
   - AI 服务连接与图表下载（需先启动 `python app.py`）
3. **数据库一致性测试**：退出程序后重新启动，验证上次操作的数据是否正确加载。
4. **信号测试**：运行中按 `Ctrl+C`，观察是否触发紧急保存提示。

## 安全注意事项

- **密码存储**：使用带盐 SHA-256，10000 次迭代（向后兼容旧 1000 次迭代密码），格式为 `salt$hash`。严禁明文存储。
- **账户锁定**：连续 5 次登录失败后自动锁定账户，需管理员手动激活。
- **SQL 注入防护**：使用 `Database::queryPrepared()` / `executePrepared()` 进行参数化查询，移除 `CLIENT_MULTI_STATEMENTS` 标志。`dbDateTime()` 使用 `\x01` 前缀约定安全注入 SQL NULL。
- **API Key 管理**：管理员注册密钥三级优先级：`HIS_ADMIN_API_KEY` 环境变量 → `Data/AdminAPIKey.txt` → 默认值。AI 服务同理（`HIS_API_KEY`）。
- **路径安全**：`Data/DatabaseConfig.txt` 包含数据库明文密码（`123456`），在生产环境中必须更换并限制文件权限。
- **DLL 依赖**：`libmysql.dll` 必须放在可执行文件同级目录，缺失会导致启动崩溃。
- **工作目录依赖**：程序使用相对路径 `../Data/DatabaseConfig.txt` 读取配置，必须在 `build/` 目录下运行。
- **AI 服务**：Flask 以 `debug=False`、`host='127.0.0.1'` 运行，所有 API 端点（除 `/api/health`）需 `X-API-Key` 认证。`clearScreen()` 使用 Win32 API/ANSI 转义，不再调用 `system()`。
- **清屏安全**：`clearScreen()` 使用 `FillConsoleOutputCharacter` + `SetConsoleCursorPosition`（Windows）/ ANSI `\033[2J\033[1;1H`（Linux），移除 `system("cls")` / `system("clear")` 调用。

## 数据库配置

`Data/DatabaseConfig.txt` 格式（5 行，键值对）：

```
host=127.0.0.1
port=3307
user=root
password=123456
database=his_db
```

MySQL 数据库 `his_db` 共 **21 张表**：14 张主表 + 7 张子表（处方明细、检查项目、附件、用药明细、药品别名、生命体征、关联记录 ID）。外键约束使用 `ON DELETE SET NULL`（用户引用）和 `ON DELETE CASCADE`（子表）。

## 常见陷阱

- **`libmysql.dll` 缺失**：构建后务必复制到 `build/Debug/`，否则程序启动即崩溃。
- **VS Code `postDebugTask` 清理**：调试结束后 `build/` 会被清空，需重新配置 CMake 并复制 DLL。
- **CMake `file(GLOB)` 的局限性**：新增 `Source/Core/`、`Source/Roles/`、`Source/Modules/` 下的 `.cpp` 后必须重新执行 `cmake ..`，否则新文件不会被编译。
- **`patient->isHospitalized` 标志位**：该标志仅在护士执行 `assignBed` 时置为 `true`，不是在创建住院记录时。
- **AI 服务离线**：管理员菜单选项 9 已做优雅处理，服务未启动时会提示用户并继续运行。
- **CSV 文件已移除**：早期版本使用 CSV 持久化，现已完全迁移至 MySQL。`Data/UserData/` 和 `Data/RecordData/` 目录已删除。
