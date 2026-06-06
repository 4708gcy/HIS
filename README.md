<p align="center">
  <h1 align="center">🏥 HIS — 医院信息系统</h1>
  <p align="center">
    <em>C++17 控制台应用 + MySQL 8.0 持久化 + FastAPI AI 智能分析微服务</em>
  </p>
  <p align="center">
    <img src="https://img.shields.io/badge/C++-17-blue.svg" />
    <img src="https://img.shields.io/badge/MySQL-8.0-orange.svg" />
    <img src="https://img.shields.io/badge/Python-3.11-green.svg" />
    <img src="https://img.shields.io/badge/FastAPI-v2.0-009688.svg" />
    <img src="https://img.shields.io/badge/LangChain-0.3-1C3C3C.svg" />
    <img src="https://img.shields.io/badge/License-Educational-lightgrey.svg" />
  </p>
</p>

---

## 📖 项目简介

HIS（Hospital Information System）是一个面向医院全业务流程的管理系统，采用 **C++17 控制台应用** 作为核心业务平台，**MySQL 8.0** 提供关系型数据持久化，并集成 **Python FastAPI AI 微服务** 提供 LLM 增强的智能数据分析与预测能力。

> 🎓 吉林大学《程序设计基础课程设计》项目 | 26,000+ 行 C++ / 1,800+ 行 Python

### ✨ 核心特性

- 🏥 **5 类用户角色** — 管理员、医生、护士、药剂师、患者，各角色独立功能菜单与权限
- 📋 **8 种业务记录** — 挂号、看诊、检查、住院、用药、药品库存、床位、护理记录完整生命周期管理
- 🗄️ **MySQL 持久化** — 21 张数据表，外键约束 + CHECK 约束，事务安全的 upsert 同步
- 🤖 **AI 智能分析** — FastAPI + LangChain 微服务，支持需求预测、异常检测、床位优化、RAG 知识库问答
- 🔐 **安全认证** — SHA-256 加盐哈希（10000 次迭代）、账户锁定、API Key 三级优先级、参数化查询防 SQL 注入
- 💊 **药物安全** — 30 对药物相互作用检测 + 患者过敏史交叉校验，成分级精确匹配

---

## 🏗️ 系统架构

```
┌──────────────────────────────────────────────────────────────────┐
│                     C++17 Console Application                    │
│                                                                  │
│  ┌────────┐ ┌────────┐ ┌────────┐ ┌────────────┐ ┌──────────┐  │
│  │ Admin  │ │ Doctor │ │ Nurse  │ │ Pharmacist │ │ Patient  │  │
│  └───┬────┘ └───┬────┘ └───┬────┘ └─────┬──────┘ └────┬─────┘  │
│      └──────────┴──────────┴─────────────┴─────────────┘         │
│                         │                                         │
│              ┌──────────┴──────────┐                             │
│              │  13 Doubly-Linked    │                             │
│              │  Lists + EntityRepo  │                             │
│              └──────────┬──────────┘                             │
│                         │                                         │
│              ┌──────────┴──────────┐                             │
│              │  MySQL C API         │                             │
│              │  (Transaction-Safe)  │                             │
│              └─────────────────────┘                             │
└─────────────────────────┬────────────────────────────────────────┘
                          │ HTTP/1.1 (WinSock2 / X-API-Key)
┌─────────────────────────┴────────────────────────────────────────┐
│              Python FastAPI AI Service v2.0                       │
│                                                                  │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────────────────┐ │
│  │ Predictions │  │  Anomalies   │  │  Bed Optimization       │ │
│  │ (Holt-      │  │  (Z-score +  │  │  (Utilization +         │ │
│  │  Winters +  │  │   LLM Read)  │  │   LLM Report)           │ │
│  │  LLM)       │  │              │  │                         │ │
│  └─────────────┘  └──────────────┘  └─────────────────────────┘ │
│  ┌─────────────┐  ┌──────────────┐                              │
│  │  Dashboard  │  │  RAG Engine  │     LangChain + OpenAI API   │
│  │  (综合报表)  │  │  (知识库问答) │     (Qwen / Aliyun Bailian) │
│  └─────────────┘  └──────────────┘                              │
│                                                                  │
│  faiss-cpu │ sentence-transformers │ matplotlib │ pymysql        │
└──────────────────────────────────────────────────────────────────┘
                          │
                   ┌──────┴──────┐
                   │  MySQL 8.0   │
                   │  his_db      │
                   │  (21 tables) │
                   └─────────────┘
```

---

## 🚀 快速开始

### 环境要求

| 依赖 | 版本要求 | 用途 |
|------|---------|------|
| **MySQL 8.0** | 8.0+ | 数据库服务 |
| **CMake** | 3.24+ | C++ 构建系统 |
| **MSVC** | VS 2017 15.8+ | C++17 编译器（Windows） |
| **Python** | 3.8+ | AI 分析微服务 |
| **MySQL C Client** | 8.0 | `libmysql.lib` + `libmysql.dll` |
| **Git** | 任意 | 版本管理 |

### 第一步：启动 MySQL 服务

```bash
# Windows 下启动 MySQL 服务
net start MySQL80

# 确认 MySQL 运行在 localhost:3307
mysql -u root -p123456 -P 3307 -e "SELECT VERSION();"
```

> ⚠️ 默认配置为 `localhost:3307`，用户 `root`，密码 `123456`。如需修改请编辑 `Data/DatabaseConfig.txt`。

### 第二步：初始化数据库

```bash
# 克隆仓库
git clone https://github.com/4708gcy/HIS.git
cd HIS

# 创建数据库和全部 21 张表
mysql -u root -p123456 -P 3307 < Data/Schema/schema.sql
```

执行后会创建 `his_db` 数据库，包含 14 张主表 + 7 张子表。

### 第三步：编译 C++ 程序

```bash
# 配置 CMake（首次或 CMakeLists.txt 变更后需要重新执行）
mkdir build
cd build
cmake ..

# 编译（Debug 模式）
cmake --build . --config Debug --target his

# 将 MySQL 客户端 DLL 复制到可执行文件目录
cp E:/MySQL/8-0/lib/libmysql.dll Debug/
```

> 💡 Release 模式编译：`cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . --config Release --target his`

### 第四步：启动 HIS 系统

```bash
# ⚠️ 必须在 build/ 目录下运行（配置文件使用相对路径 ../Data/）
cd build
./Debug/his.exe
```

首次启动时，由于数据库中没有管理员账户，系统会强制进入 **管理员注册流程**：

```
✓ 已连接 MySQL 数据库
没有管理员数据，请先创建管理员账号，否则系统无法启动！
===== 管理员注册 =====
请输入 API Key（用于验证管理员身份）: 88888888    ← 默认 API Key
请输入用户名: admin
请输入密码: ********
✅ 管理员注册成功! 您的用户ID是: 000001
```

> 🔑 默认 API Key 为 `88888888`。可通过环境变量 `HIS_ADMIN_API_KEY` 或文件 `Data/AdminAPIKey.txt` 自定义。

### 第五步（可选）：启动 AI 分析微服务

```bash
# 进入 AI 服务目录
cd ai_service

# 安装依赖（建议使用虚拟环境）
pip install -r requirements.txt

# 配置 LLM（可选 — 仅 LLM 策略需要）
# 编辑 config.yaml 中的 llm.api_key，或设置环境变量：
# set HIS_LLM_API_KEY=sk-xxxxx

# 启动服务
python main.py
```

服务启动后运行在 `http://127.0.0.1:5001`，可在管理员菜单中选择 **"9. AI 智能分析"** 调用。

> 🐳 Docker 部署：`docker build -t his-ai . && docker run -p 5001:5001 his-ai`

### 运行效果

系统登录后提供角色选择菜单：

```
╔══════════════════════════════════════╗
║          HIS 医院信息系统            ║
╠══════════════════════════════════════╣
║  1. 管理员登录                       ║
║  2. 医生登录                         ║
║  3. 护士登录                         ║
║  4. 药剂师登录                       ║
║  5. 患者登录                         ║
║  0. 退出系统                         ║
╚══════════════════════════════════════╝
```

---

## 🌐 AI 微服务端点 (v2.0)

所有端点（`/health` 和 `/` 除外）需要在请求头中携带 `X-API-Key`。

| 端点 | 方法 | 说明 | 策略参数 |
|------|------|------|---------|
| `POST /api/v2/predictions` | POST | 科室需求预测 | `traditional` / `llm` / `auto` |
| `POST /api/v2/anomalies` | POST | Z-score 异常检测 + LLM 解读 | `traditional` / `llm` / `auto` |
| `POST /api/v2/beds` | POST | 床位分配优化 + LLM 报告 | `traditional` / `llm` / `auto` |
| `POST /api/v2/dashboard` | POST | 综合仪表盘 + LLM 摘要 | `traditional` / `llm` / `auto` |
| `GET /api/v2/charts/{type}` | GET | PNG 图表（prediction/bed/anomaly/medicine） | — |
| `POST /api/v2/rag/chat` | POST | RAG 知识库问答 | — |
| `POST /api/v2/rag/upload` | POST | 上传文档到知识库（异步） | — |
| `GET /health` | GET | 健康检查 | — |

**请求示例：**

```bash
# 需求预测（LLM 策略）
curl -X POST http://127.0.0.1:5001/api/v2/predictions \
  -H "X-API-Key: his-default-key" \
  -H "Content-Type: application/json" \
  -d '{"months": 6, "strategy": "llm"}'

# RAG 知识库问答
curl -X POST http://127.0.0.1:5001/api/v2/rag/chat \
  -H "X-API-Key: his-default-key" \
  -H "Content-Type: application/json" \
  -d '{"query": "内科最近的住院趋势如何？", "top_k": 5}'
```

**统一响应格式：**

```json
{
  "request_id": "uuid",
  "data": { ... },
  "response_code": 200,
  "response_msg": "预测完成",
  "process_status": "completed",
  "processing_time": 1.234
}
```

---

## 📊 数据库设计

**his_db** 共 21 张表，覆盖完整的医院业务数据模型：

### 主表（14 张）

| 表名 | 说明 | 关键字段 |
|------|------|---------|
| `admins` | 管理员 | admin_id, stored_hash, salt |
| `doctors` | 医生 | doctor_id, department, title, specialty |
| `nurses` | 护士 | nurse_id, department, level |
| `pharmacists` | 药剂师 | pharmacist_id, department, level |
| `patients` | 患者 | patient_id, allergies, balance_cents |
| `registrations` | 挂号记录 | registration_id, status, fee_cents |
| `consultations` | 看诊记录 | consultation_id, diagnosis |
| `examinations` | 检查记录 | examination_id, report_summary |
| `hospitalizations` | 住院记录 | hospitalization_id, deposit_cents |
| `medication_records` | 用药记录 | record_id, review_status |
| `medicines` | 药品信息 | medicine_id, stock, price_cents |
| `bed_info` | 床位信息 | bed_id, status, ward_type |
| `nursing_records` | 护理记录 | record_id, vital_signs |
| `operation_logs` | 操作日志 | log_id, user_id, operation |

### 子表（7 张）

`prescription_items`、`examination_items`、`attachment_files`、`medication_lines`、`medicine_aliases`、`vital_signs`、`related_record_ids`

### 设计原则

- **外键约束：** 患者引用 `ON DELETE SET NULL`，子表记录 `ON DELETE CASCADE`
- **CHECK 约束：** 金额 `>= 0`、库存 `>= 0`、状态范围校验
- **逻辑删除：** 所有主表使用 `is_deleted` 标志，不物理删除
- **金额存储：** 所有金额以**分（INT）** 存储，避免浮点误差

---

## 📁 项目结构

```
HIS/
├── main.cpp                      # 入口：信号处理 → DB连接 → 数据加载 → 登录循环 → 保存退出
│
├── Head/                         # 头文件（25 个）
│   ├── Core/                     #   Database.h, Login.h, SHA-256.h, UI.h, User.h, EntityRepository.h, GetTime.h
│   ├── Entities/                 #   Registration, Consultation, Examination, Hospitalization,
│   │                             #   MedicationRecord, Medicine, NursingRecord
│   ├── Roles/                    #   Admin, Doctor, Nurse, Pharmacist, Patient
│   └── Modules/                  #   AIQueryClient, DataAnalysis, DrugSafety, LoadData, SaveData
│
├── Source/                       # 源文件（16 个）
│   ├── Core/                     #   Database.cpp, Login.cpp, SHA-256.cpp, UI.cpp, User.cpp, GetTime.cpp
│   ├── Roles/                    #   Admin.cpp (×4), Doctor.cpp, Nurse.cpp, Pharmacist.cpp, Patient.cpp
│   └── Modules/                  #   AIQueryClient.cpp, DataAnalysis.cpp, DrugSafety.cpp,
│                                 #   LoadData.cpp, SaveData.cpp
│
├── ai_service/                   # Python AI 微服务 v2.0
│   ├── main.py                   #   FastAPI 入口（uvicorn）
│   ├── schemas.py                #   Pydantic 请求/响应模型
│   ├── config.yaml               #   YAML 配置中心
│   ├── Dockerfile                #   Docker 部署
│   ├── requirements.txt          #   Python 依赖
│   ├── core/                     #   基础层：config, database pool, security, response, exceptions, llm_provider
│   ├── routers/                  #   路由层：predictions, anomalies, beds, dashboard, charts, rag
│   ├── services/                 #   服务层：predictor, analyzer, rag_engine, llm_client, prompt_builder, ...
│   ├── tests/                    #   pytest 单元测试
│   └── charts.py                 #   Matplotlib 图表生成器
│
├── Data/
│   ├── DatabaseConfig.txt        #   MySQL 连接配置
│   ├── AdminAPIKey.txt           #   管理员注册 API Key
│   ├── Schema/
│   │   └── schema.sql            #   建表脚本（21 张表）
│   ├── DrugData/
│   │   └── interactions.txt      #   30 对药物相互作用数据
│   └── OperationLog/             #   运行日志（his_YYYY_MM_DD.log）
│
├── CMakeLists.txt                #   CMake 构建配置
├── CLAUDE.md                     #   AI 辅助开发文档
└── README.md                     #   本文件
```

---

## 🔐 安全特性

| 特性 | 实现方式 |
|------|---------|
| **密码存储** | SHA-256 + 随机 16 字节盐 + 10,000 次迭代 |
| **向后兼容** | `verifyPasswordCompat()` 自动识别旧版 1,000 次迭代哈希 |
| **账户锁定** | 连续 5 次登录失败自动锁定，管理员手动解锁 |
| **SQL 注入防护** | 全部参数化查询（`queryPrepared`），禁用 `CLIENT_MULTI_STATEMENTS` |
| **API Key 三级优先级** | 环境变量 → 配置文件 → 默认值（`HIS_ADMIN_API_KEY` / `HIS_API_KEY`） |
| **时序攻击防护** | SHA-256 比较使用 volatile XOR 逐字节比较 |
| **AI 服务认证** | FastAPI `Depends(verify_api_key)`，`X-API-Key` 请求头 |

---

## 💊 药物安全系统

- **药物相互作用（DDI）检测：** 30 对已知药物相互作用数据，开药和审核双端集成
- **过敏史交叉校验：** 自动检查处方药物成分与患者过敏史的冲突
- **成分级精确匹配：** `tokenizeDrugName()` 按 `/`、`+`、`-` 等分隔符拆分复方药品名，逐成分精确匹配，避免"阿莫西林"误匹配"阿莫西林克拉维酸钾"

---

## 🧪 测试

```bash
# AI 服务单元测试
cd ai_service
pytest tests/test_services.py -v

# C++ 构建验证
cmake --build build --config Debug --target his
```

---

## 🛠️ 技术栈

| 层级 | 技术 |
|------|------|
| **核心业务** | C++17, MySQL C API, WinSock2, CMake |
| **数据库** | MySQL 8.0（21 表, FK 约束, CHECK 约束, 事务） |
| **AI 微服务** | Python 3.11, FastAPI, LangChain, Pydantic |
| **数据分析** | pandas, matplotlib, numpy, scikit-learn, jieba |
| **LLM & RAG** | LangChain-OpenAI, faiss-cpu, sentence-transformers |
| **安全** | SHA-256 + salt (10K iterations), API Key, 参数化查询 |
| **跨平台** | Windows (Win32 Console API) / Linux (ANSI escape) |
| **工程化** | Git, CMake, Docker, YAML 配置, pytest |

---

## 📄 许可证

本项目为吉林大学课程设计作业，代码仅用于教育和展示目的。

<p align="center">
  <sub>Built with ❤️ by <a href="https://github.com/4708gcy">郭承宇</a> @ Jilin University</sub>
</p>
