# C 课程设计 —— 医疗管理系统 (HIS)

C++17 医疗信息管理系统，支持两种运行模式：交互式控制台 和 REST API + Vue 3 前后端分离。

---

## 公共约定

| 约定 | 规则 |
| ------ | ------ |
| **头文件** | 大驼峰命名，如 `Doctor.h`，配套同名 `.cpp` |
| **函数** | 小驼峰命名，如 `addDoctor()` |
| **ID 规则** | 6 位数字：首位区分身份（0=管理员, 1=医生, 2=护士, 3=药剂师, 4=患者），后 5 位递增序号 |
| **数据结构** | 双向链表（`prev`/`next`），头插入 O(1) |
| **继承** | 所有角色类继承自 `User`，医疗信息保存在基类中，按身份访问不同字段 |
| **密码** | SHA-256 + 随机盐 + 1000 次迭代，存储格式 `salt$hash`，使用防时序攻击比较，5 次失败锁定 |
| **空值** | 字符串空字段统一用 `"#"` 作哨兵值 |
| **删除** | 逻辑删除（`isDeleted` 标志），不物理移除 |
| **时间** | 统一调用 `GetTime.h`，格式 `YYYY-MM-DD hh:mm:ss` |
| **科室** | 内科, 外科, 儿科, 妇产科, 急诊科 |

### 费用标准

| 类型 | 依据 | 价格 |
| ------ | ------ | ------ |
| 挂号费 | 医生职称 | 实习10 / 住院20 / 主治30 / 副高40 / 正高50 元 |
| 检查费 | 项目名称 | 2–30 元（详见 `User.h`） |
| 住院费 | 病房类型 × 天数 | 普通50 / 隔离100 / VIP200 / ICU500 元/天 |

### 数据持久化

纯文本 CSV 格式，存储于 `Data/` 目录（从 `build/` 启动时为 `../Data/`）。所有 `Data/` 子目录必须预先存在。

| 角色/记录 | 文件路径 |
| ----------- | --------- |
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
| 用药 | `Data/RecordData/MedicineChainData/medication_records.txt` |
| 药品 | `Data/RecordData/MedicineChainData/medicines.txt` |
| 操作日志 | `Data/OperationLog/his_YYYY_MM_DD.log` |

---

## 运行方式

### 模式一：控制台程序

```bash
# 1. 配置（或通过 VS Code 任务面板: CMake: 配置）
cd build && cmake ..

# 2. 编译（或: CMake: 编译）
cd build && cmake --build . --config Debug

# 3. 运行（必须从 build/ 目录启动）
cd build && ./Debug/his.exe
```

> **重要：** 程序通过 `../Data/` 相对路径读取数据，**必须从 `build/` 目录运行**。首次运行会强制要求注册管理员账号。

```text
主菜单
├── 1. 登录 → 选择角色 → 角色功能菜单
├── 2. 注册 → 选择角色 → 填写信息
└── 0. 退出 → 保存所有数据
```

### 模式二：REST API + Vue 前端

**后端（C++ REST API，监听 8080）：**

```bash
# 编译（或: CMake: his_server编译）
cd build && cmake --build . --config Debug --target his_server

# 运行
cd build && ./Debug/his_server.exe
# Ctrl+C 优雅退出并自动保存数据
```

**前端（Vue 3 + Element Plus，端口 3000）：**

```bash
cd frontend

# 首次安装依赖
npm install

# 开发服务器（自动代理 /api 到 localhost:8080）
npm run dev

# 生产构建
npm run build

# 预览生产构建
npm run preview
```

**完整流程：** 先启动后端 → 再启动前端 → 浏览器访问 `http://localhost:3000` → 退出时先关前端再关后端。

### 前后端角色映射

| 前端（1-based） | 后端（0-based） | 角色 |
| ----------------- | ----------------- | ------ |
| 1 | 0 | 管理员 |
| 2 | 1 | 医生 |
| 3 | 2 | 护士 |
| 4 | 3 | 药剂师 |
| 5 | 4 | 患者 |

---

## 调试方式

- **控制台程序**：设置断点 → `F5` 启动调试 → 外部终端运行 → 调试结束后自动清理 `build/`（`postDebugTask`）
- **REST API 后端**：修改 `.vscode/launch.json` 的 `program` 指向 `his_server.exe` → 设置断点 → `F5` → 通过浏览器/Postman 触发
- **前端**：浏览器 `F12`（Console / Network / Vue DevTools），或 VS Code 断点调试

---

## 项目结构

```text
C课设-HIS/
├── CMakeLists.txt              # 构建配置（C++17, MSVC /utf-8, 两个目标 his / his_server）
├── main.cpp                    # 控制台程序入口
├── server_main.cpp             # REST 服务器入口
├── Head/                       # 头文件（24 个 .h）
│   ├── User.h                  # 基类：枚举、费用计算、状态转换、SHA-256 盐值
│   ├── Admin.h / Doctor.h / Nurse.h / Pharmacist.h / Patient.h  # 角色类
│   ├── Registration.h / Consultation.h / Examination.h / Hospitalization.h / MedicationRecord.h / Medicine.h  # 记录结构
│   ├── Login.h                 # 登录验证（含泛型账户管理）
│   ├── LoadData.h / SaveData.h # 数据持久化
│   ├── UI.h                    # 控制台 UI / 颜色 / 分页 / 日志
│   ├── ApiServer.h             # DataManager 单例 + API 路由注册（~124 端点）
│   ├── ApiResponse.h           # 统一响应格式 {code, message, data}
│   ├── JWTAuth.h               # JWT 认证（HMAC-SHA256, 24h 有效）
│   ├── JsonHelper.h            # 数据结构 → JSON 序列化
│   ├── httplib.h / json.hpp    # 第三方依赖（header-only）
│   └── GetTime.h / SHA-256.h   # 工具库
├── Source/                     # 实现文件（15 个 .cpp）
├── Data/                       # 持久化数据（见上方表格）
└── frontend/                   # Vue 3 前端（33 个 .vue 文件）
    ├── index.html
    ├── package.json / vite.config.js
    └── src/
        ├── main.js             # 应用入口（Element Plus 中文 locale）
        ├── App.vue
        ├── styles/global.css   # 蓝白医疗主题 CSS 变量系统
        ├── api/                # 8 个 API 模块（axios 实例 + JWT 拦截器）
        ├── store/user.js       # Pinia 状态（token/角色/localStorage 持久化）
        ├── router/index.js     # Vue Router（角色路由守卫）
        └── views/
            ├── Login.vue / Register.vue / Layout.vue
            ├── admin/          # 13 个页面（Dashboard, 人员 CRUD, 记录管理, 药品, 床位, Profile）
            ├── doctor/         # 4 个页面（挂号/看诊/检查, Profile）
            ├── nurse/          # 4 个页面（住院/体征/床位, Profile）
            ├── pharmacist/     # 3 个页面（用药审核/药品库存, Profile）
            └── patient/        # 6 个页面（预约挂号/支付/查看记录/充值, Profile）
```

---

## 后端 API 架构

`DataManager` 单例替代全局变量，`std::mutex` 线程安全。每次写操作后 `saveAllUnsafe()` 持久化（**Caller must hold mutex**，"Unsafe" 指不可在无锁状态下调用）。

**认证**：`POST /api/auth/login`（统一登录）+ `POST /api/auth/register`（统一注册） → JWT Bearer Token。

**端点分组**：

| 分组 | 路径前缀 | 数量 | 说明 |
| ------ | --------- | ------ | ------ |
| 通用 | `/api/` | 4 | 登录/注册/修改密码/科室列表/费用标准/检查项目 |
| 管理员 | `/api/admin/` | 30+ | 人员 CRUD、记录管理、药品/床位管理、账号状态 |
| 医生 | `/api/doctor/` | 11 | 挂号/看诊/检查 CRUD、个人信息 |
| 护士 | `/api/nurse/` | 11 | 住院管理/分配床位/出院/床位管理、个人信息 |
| 药剂师 | `/api/pharmacist/` | 11 | 用药审核/发药/药品 CRUD、个人信息 |
| 患者 | `/api/patient/` | 12 | 挂号/支付/充值/查看记录/个人信息 |

**统一响应**：`{ "code": 200, "message": "", "data": {} }`。错误码：200/400/401/403/404/500。

**CORS**：所有响应添加 `Access-Control-Allow-Origin: *`。

---

## 前端架构

| 技术 | 用途 |
| ------ | ------ |
| Vue 3 (Composition API) | 框架 |
| Vite 6 | 构建工具 |
| Element Plus (zh-cn) | UI 组件库 |
| Pinia | 状态管理（localStorage 持久化） |
| Vue Router | 路由（角色守卫 + 懒加载） |
| Axios | HTTP（JWT 拦截器 + 401 自动跳转） |

**设计主题**："Pure & Clinical" 简约医疗风 —— 蓝白主色 (`#1e88e5`)、磨砂玻璃卡片、`fade-in` 页面动画、CSS 变量驱动。

**路由守卫**：未登录 → `/login`；角色不匹配 → 自动重定向到角色默认页面。

---

## 已知陷阱

| 陷阱 | 说明 |
| ------ | ------ |
| `bedStatus` 命名 | 小写 `b`，非 `BedStatus` |
| `ConsultationStatus` | 使用 `COMPLETED` 而非 `FINISHED` |
| `Examination` | 报告字段为 `reportSummary` 非 `report` |
| ID 前缀 | 用药记录 `mrd`，药品 `med`，床位用 `autoGenerateBedID()` 结构化生成 |
| 链表插入 | 始终头插入（O(1)），尾插入曾为确认 bug |
| `saveAllUnsafe()` | 调用方必须先持有锁，内部不加锁 |
| `Admin` 结构 | 含 `prev` 指针（双向链表），与其他实体一致 |
