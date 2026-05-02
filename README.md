# C 课程设计 —— 医院信息管理系统 (HIS)

C++17 轻量级医院信息管理系统，支持**交互式控制台**和 **REST API + Vue 3 前后端分离**两种运行模式。覆盖管理员、医生、护士、药剂师、患者五种角色，实现挂号→看诊→检查→住院→用药全流程闭环管理。

---

## 公共约定

| 约定 | 规则 |

|------|------|
| **头文件** | PascalCase 命名（如 `Doctor.h`），配套同名 `.cpp` |
| **函数** | camelCase 命名（如 `addDoctor()`） |
| **ID** | 6 位数字，首位标识角色（`0` 管理员 / `1` 医生 / `2` 护士 / `3` 药剂师 / `4` 患者），后 5 位递增 |
| **数据结构** | 双向链表（`prev` / `next`），头插入 O(1) |
| **继承** | 所有角色类继承 `User`，共享同一条医疗记录链表 |
| **密码** | SHA-256 + 16 字符随机盐 + 1000 次迭代，存储格式 `salt$hash`，volatile XOR 时序安全比较，5 次失败锁定 |
| **空值** | 字符串空字段统一用 `"#"` 哨兵值 |
| **删除** | 逻辑删除（`isDeleted` 标志），不物理移除 |
| **时间** | 统一调用 `MyTime` 单例（`GetTime.h`），格式 `YYYY-MM-DD hh:mm:ss` |
| **科室** | 内科、外科、儿科、妇产科、急诊科 |

### 费用标准

| 类型 | 依据 | 价格 |

|------|------|------|
| 挂号费 | 医生职称 | 实习 10 / 住院医师 20 / 主治 30 / 副高 40 / 正高 50 元 |
| 检查费 | 项目名称 | 2–30 元（14 项，详见 `User.h`） |
| 住院费 | 病房类型 × 天数 | 普通 50 / 隔离 100 / VIP 200 / ICU 500 元/天 |

### 数据持久化

纯文本 CSV 格式，存储于 `Data/` 目录（从 `build/` 启动时为 `../Data/`）。空字段以 `"#"` 填充，所有删除均为逻辑删除。

> **重要：** 所有 `Data/` 子目录必须预先存在，缺失目录会导致静默加载失败。

| 实体 | 文件路径 |

|------|----------|
| 管理员 | `Data/UserData/AdminChainData/admin_users.txt` |
| 医生 | `Data/UserData/DoctorChainData/doctor_users.txt` |
| 护士 | `Data/UserData/NurseChainData/nurse_users.txt` |
| 药剂师 | `Data/UserData/PharmacistChainData/pharmacist_users.txt` |
| 患者 | `Data/UserData/PatientChainData/patient_users.txt` |
| 挂号 | `Data/RecordData/RegistrationChainData/registrations.txt` |
| 看诊 | `Data/RecordData/ConsultationChainData/consultations.txt` |
| 检查 | `Data/RecordData/ExaminationChainData/examinations.txt` |
| 住院 | `Data/RecordData/HospitalizationChainData/hospitalizations.txt` |
| 床位 | `Data/RecordData/HospitalizationChainData/bed_info.txt` |
| 用药记录 | `Data/RecordData/MedicineChainData/medication_records.txt` |
| 药品 | `Data/RecordData/MedicineChainData/medicines.txt` |
| 药品流水 | `Data/RecordData/MedicineChainData/medicine_flow.txt` |
| 排班 | `Data/schedules.json`（JSON，仅服务端） |
| 操作日志 | `Data/OperationLog/his_YYYY_MM_DD.log` |

---

## 运行方式

### 模式一：控制台程序

```bash
# 配置（或 VS Code: Ctrl+Shift+P → Tasks: Run Task → "CMake: 配置")
cd build && cmake ..

# 编译（或: "CMake: 编译"）
cd build && cmake --build . --config Debug

# 运行（必须从 build/ 目录启动）
cd build && ./Debug/his.exe
```

> 程序通过 `../Data/` 相对路径读取数据，**必须从 `build/` 目录运行**。首次运行强制注册管理员账号。

```text
主菜单
├── 1. 登录 → 选择角色 → 输入 ID + 密码 → 角色功能菜单
├── 2. 注册 → 选择角色 → 填写信息 → 自动分配 ID
└── 0. 退出 → 保存全部数据并退出
```

### 模式二：REST API + Vue 前端

**后端（C++ REST API，端口 8080）：**

```bash
# 编译（或: "CMake: his_server编译"）
cd build && cmake --build . --config Debug --target his_server

# 运行（Ctrl+C 优雅退出并自动保存）
cd build && ./Debug/his_server.exe
```

**前端（Vue 3 + Element Plus，端口 3000）：**

```bash
cd frontend

# 首次安装依赖
npm install

# 开发服务器（自动代理 /api → localhost:8080）
npm run dev

# 生产构建
npm run build
```

**完整流程：** 先启动后端（8080） → 再启动前端（3000） → 浏览器访问 `http://localhost:3000` → 退出时先关前端再关后端。

### 前后端角色映射

| 后端（0-based） | 前端（1-based） | 角色 |

|-----------------|-----------------|------|
| 0 | 1 | 管理员 |
| 1 | 2 | 医生 |
| 2 | 3 | 护士 |
| 3 | 4 | 药剂师 |
| 4 | 5 | 患者 |

---

## 调试方式

| 模式 | 方法 |

|------|------|
| 控制台程序 | 设置断点 → `F5` → 外部终端运行 → 调试结束后自动清理 `build/`（`postDebugTask`） |
| REST API 后端 | 修改 `.vscode/launch.json` 的 `program` 指向 `his_server.exe` → `F5` → 浏览器/Postman 触发 |
| 前端 | 浏览器 `F12`（Console / Network / Vue DevTools），或 VS Code 断点调试 |

---

## 项目结构

```text
C课设-HIS/
├── CMakeLists.txt                  # 构建配置（C++17, MSVC /utf-8, 两个目标: his / his_server）
├── main.cpp                        # 控制台程序入口
├── server_main.cpp                 # REST API 服务器入口
├── HIS-Mind-Map.jpg                # 功能思维导图
│
├── Head/                           # 头文件（23 个项目 .h + 2 个第三方库）
│   ├── User.h                      #   基类：枚举定义、费用计算、状态转换、SHA-256 盐值
│   ├── Admin.h                     #   管理员：全局数据管理、统计报表、账号管理
│   ├── Doctor.h                    #   医生：挂号/看诊/检查管理
│   ├── Nurse.h                     #   护士：住院/床位/检查管理
│   ├── Pharmacist.h                #   药剂师：用药审核、药品库存管理
│   ├── Patient.h                   #   患者：预约挂号、缴费、查看记录
│   ├── Registration.h              #   挂号记录结构
│   ├── Consultation.h              #   看诊记录结构（含处方子结构）
│   ├── Examination.h               #   检查记录结构（含生命体征）
│   ├── Hospitalization.h           #   住院记录结构
│   ├── MedicationRecord.h          #   用药记录结构（含药品行明细）
│   ├── Medicine.h                  #   药品信息结构
│   ├── Login.h                     #   登录验证 + 泛型账户管理模板
│   ├── LoadData.h                  #   数据加载（13 条链表）
│   ├── SaveData.h                  #   数据保存（13 条链表）
│   ├── UI.h / UI.cpp               #   控制台 UI：双线框菜单、彩色输出、分页、日志
│   ├── DataAnalysis.h              #   数据分析与预测（移动平均 + 线性回归）
│   ├── ApiServer.h                 #   DataManager 单例 + API 路由注册（~140 端点）
│   ├── ApiResponse.h               #   统一 JSON 响应格式
│   ├── JWTAuth.h                   #   JWT 认证（HMAC-SHA256, 24h 有效）
│   ├── JsonHelper.h                #   数据结构 → JSON 序列化
│   ├── GetTime.h                   #   MyTime 单例时间工具
│   ├── SHA-256.h                   #   SHA-256 哈希算法
│   ├── httplib.h                   #   [第三方] cpp-httplib（header-only）
│   └── json.hpp                    #   [第三方] nlohmann/json（header-only）
│
├── Source/                         # 实现文件（16 个 .cpp）
│   ├── Admin.cpp                   #   管理员全部功能（~7,785 行，最大文件）
│   ├── ApiServer.cpp               #   REST API 路由处理（~140 个端点）
│   ├── DataAnalysis.cpp            #   数据分析与预测实现
│   ├── Doctor.cpp                  #   医生功能实现
│   ├── Nurse.cpp                   #   护士功能实现
│   ├── Pharmacist.cpp              #   药剂师功能实现
│   ├── Patient.cpp                 #   患者功能实现
│   ├── LoadData.cpp                #   CSV 数据加载
│   ├── SaveData.cpp                #   CSV 数据保存
│   ├── Login.cpp                   #   登录/注册逻辑
│   ├── User.cpp                    #   基类方法实现
│   ├── UI.cpp                      #   控制台 UI 实现
│   ├── JWTAuth.cpp                 #   JWT 签发与验证
│   ├── JsonHelper.cpp              #   JSON 序列化
│   ├── GetTime.cpp                 #   时间工具
│   └── SHA-256.cpp                 #   SHA-256 实现
│
├── Data/                           # 持久化数据（见上方数据持久化表格）
│
└── frontend/                       # Vue 3 前端（37 个 .vue 文件）
    ├── index.html
    ├── package.json
    ├── vite.config.js
    └── src/
        ├── main.js                 #   应用入口（Element Plus 中文 locale）
        ├── App.vue
        ├── styles/global.css       #   蓝白医疗主题 CSS 变量系统
        ├── api/                    #   8 个 API 模块（axios + JWT 拦截器 + 401 重定向）
        │   ├── index.js            #     axios 实例 + 请求/响应拦截器
        │   ├── auth.js             #     登录/注册/改密
        │   ├── admin.js            #     管理员端全部 API
        │   ├── doctor.js           #     医生端 API
        │   ├── nurse.js            #     护士端 API
        │   ├── pharmacist.js       #     药剂师端 API
        │   ├── patient.js          #     患者端 API
        │   └── common.js           #     公共 API（科室/费用标准/检查项目）
        ├── store/user.js           #   Pinia 状态管理（token/角色/localStorage 持久化）
        ├── router/index.js         #   Vue Router（角色路由守卫 + 懒加载）
        └── views/
            ├── Login.vue           #   登录页
            ├── Register.vue        #   注册页
            ├── Layout.vue          #   主布局（侧边栏 + 顶栏）
            ├── admin/              #   16 个页面
            │   ├── Dashboard.vue   #     仪表盘
            │   ├── Doctors.vue     #     医生管理
            │   ├── Nurses.vue      #     护士管理
            │   ├── Pharmacists.vue #     药剂师管理
            │   ├── Patients.vue    #     患者管理
            │   ├── Registrations.vue    # 挂号管理
            │   ├── Consultations.vue    # 看诊管理
            │   ├── Examinations.vue     # 检查管理
            │   ├── Hospitalizations.vue # 住院管理
            │   ├── MedicationRecords.vue # 用药管理
            │   ├── Medicines.vue   #     药品管理
            │   ├── MedicineFlows.vue #   药品流水
            │   ├── Beds.vue        #     床位管理
            │   ├── Reports.vue     #     统计报表
            │   ├── Schedules.vue   #     排班管理
            │   └── Profile.vue     #     个人信息
            ├── doctor/             #   4 个页面
            │   ├── Registrations.vue
            │   ├── Consultations.vue
            │   ├── Examinations.vue
            │   └── Profile.vue
            ├── nurse/              #   4 个页面
            │   ├── Hospitalizations.vue
            │   ├── Examinations.vue
            │   ├── Beds.vue
            │   └── Profile.vue
            ├── pharmacist/         #   3 个页面
            │   ├── MedicationRecords.vue
            │   ├── Medicines.vue
            │   └── Profile.vue
            └── patient/            #   7 个页面
                ├── Registrations.vue
                ├── Consultations.vue
                ├── Examinations.vue
                ├── Hospitalizations.vue
                ├── MedicationRecords.vue
                ├── Schedules.vue
                └── Profile.vue
```

---

## 后端 API 架构

`DataManager` 单例管理全部数据，`std::mutex` 保证线程安全。每次写操作后调用 `saveAllUnsafe()` 持久化（调用方必须持有锁，`Unsafe` 指"不在无锁状态下调用"）。

**认证流程：** `POST /api/auth/login` → JWT Bearer Token → 后续请求 `Authorization: Bearer <token>`

**端点分组（~140 个）：**

| 分组 | 路径前缀 | 数量 | 说明 |

|------|----------|------|------|
| 认证 | `/api/auth/` | 4 | 登录 / 注册 / 修改密码 |
| 管理员 | `/api/admin/` | 45+ | 五类人员 CRUD、五类记录管理、药品/床位、排班、统计报表 |
| 医生 | `/api/doctor/` | 14 | 挂号 / 看诊 / 检查 CRUD、个人信息 |
| 护士 | `/api/nurse/` | 12 | 住院管理 / 床位分配 / 出院转科、个人信息 |
| 药剂师 | `/api/pharmacist/` | 12 | 用药审核 / 发药 / 药品 CRUD、个人信息 |
| 患者 | `/api/patient/` | 13 | 挂号 / 缴费 / 充值 / 查看记录、个人信息 |
| 公共 | `/api/` | 3 | 科室列表 / 费用标准 / 检查项目 |

**统一响应格式：**

```json
{ "code": 200, "message": "success", "data": { ... } }
```

错误码：`200` 成功 / `400` 参数错误 / `401` 未认证 / `403` 权限不足 / `404` 未找到 / `500` 服务器错误。所有响应包含 `Access-Control-Allow-Origin: *` CORS 头。

---

## 前端架构

| 技术 | 版本 | 用途 |

|------|------|------|
| Vue 3 | 3.x | 框架（Composition API） |
| Vite | 6.x | 构建工具 |
| Element Plus | — | UI 组件库（中文 locale） |
| Pinia | — | 状态管理（localStorage 持久化） |
| Vue Router | — | 路由（角色守卫 + 懒加载） |
| Axios | — | HTTP 客户端（JWT 拦截器 + 401 自动跳转） |

**设计主题：** "Pure & Clinical" 简约医疗风 — 蓝白主色（`#1e88e5`）、磨砂玻璃卡片、`fade-in` 页面动画、CSS 变量驱动。

**路由守卫：** 未登录 → `/login`；角色不匹配 → 自动重定向到该角色的默认页面。

---

## 已知陷阱

| 陷阱 | 说明 |

|------|------|
| `bedStatus` 命名 | 小写 `b`（非 `BedStatus`） |
| `ConsultationStatus` | 使用 `COMPLETED`（非 `FINISHED`） |
| `Examination` 报告字段 | `reportSummary`（非 `report`） |
| ID 前缀 | 用药记录 `mrd`，药品 `med`，药品流水 `mfl`，床位用 `autoGenerateBedID()` 生成 |
| 链表插入 | 始终头插入 O(1)，尾插入曾为确认 bug |
| `saveAllUnsafe()` | 调用方必须先持锁，函数本身不加锁 |
| `printMenuSeparator()` | 已定义但从未调用（死代码），用 `printMenuLine("")` 代替 |
| 条件嵌套 | `else if` 块勿嵌套在前一个 `else if` 的 `if` 分支内（曾导致菜单选项不可达） |

---

## 许可证

本项目仅用于课程设计学习，详见 [LICENSE](./LICENSE)。
