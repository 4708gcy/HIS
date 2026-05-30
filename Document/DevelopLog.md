# HIS 开发日志

---

## 2026.5.30 — 第四轮优化：安全加固、Bug 修复、代码质量提升

基于全面代码审查报告（karpathy-coder），GLM 完成了 11 项改进任务：

### 安全加固
- **替换 `system("cls")`**：`UI.cpp` 的 `clearScreen()` 改用 Win32 `FillConsoleOutputCharacter` + `SetConsoleCursorPosition`（Windows）/ ANSI `\033[2J\033[1;1H`（Linux），不再调用 `system()`
- **移除 `CLIENT_MULTI_STATEMENTS`**：`Database.cpp` 的 `mysql_real_connect()` 调用中移除此标志，防止多语句 SQL 注入
- **API Key 三级优先级**：`User.cpp` 的 `signUp()` 中 API Key 读取顺序改为 `HIS_ADMIN_API_KEY` 环境变量 → `Data/AdminAPIKey.txt` → 默认 `88888888`；AI 服务 `app.py` 同理支持 `HIS_API_KEY` 环境变量

### Bug 修复
- **PNG 下载二进制安全**：`AIQueryClient::downloadChart()` 新增 `parseContentLength()` 解析响应头、使用 `std::ios::binary` 写入、8KB 缓冲区、Content-Length 校验
- **EntityRepository ID 不变性**：`getIdField()` 三个版本（主模板 + Patient/Registration 特化）均返回 `const std::string&`，防止外部修改 ID 导致 `idMap` 索引过期
- **药物相互作用精确匹配**：`DrugSafety.cpp` 新增 `tokenizeDrugName()`（按 `/`, `+`, `-`, `·`, `\`, `&` 分隔药品名）和 `isConstituentOf()`，用成分级精确匹配替代子字符串匹配，消除"阿莫西林克拉维酸钾"误匹配"阿莫西林"的假阳性
- **dbDateTime SQL NULL 改进**：空日期时间返回 `\x01NULL`（`\x01` 前缀标记），`Database::buildSql()` 识别后将 `NULL` 作为原始 SQL 片段注入，不加引号不转义

### 代码质量提升
- **拆分 Admin.cpp 巨型文件**：8586 行单文件 → 4 个文件
  - `Admin.cpp`（64 行）：注册/登录 + include 聚合
  - `AdminRecords.cpp`（~5200 行）：挂号/看诊/检查/住院/床位/用药记录/药品管理
  - `AdminUsers.cpp`（~4000 行）：医生/护士/药剂师/患者管理 + 个人信息
  - `AdminReports.cpp`（~1000 行）：6 个统计报表函数
- **提取实体打印函数**：`UI.h/cpp` 中新增 6 个 `printXxxCard()` 函数（Registration/Consultation/Examination/MedicationRecord/Hospitalization/Medicine），集中管理实体显示格式
- **通用查询过滤模板**：`UI.h` 新增 `displayChainByFilter<T>(head, filterFn, printer)`，接受 lambda 谓词，替代手动 while 循环
- **移动 NursingRecord**：从 `User.h`（用户基类头文件）移至 `Head/Entities/NursingRecord.h`（实体头文件目录），改善模块边界
- **CMake 构建类型**：`CMakeLists.txt` 新增 Debug（默认）/ Release 两种构建类型支持，Release 开启 MSVC `/GL` + `/LTCG`
- **删除死代码**：移除 `LoadData.cpp` 中未被调用的 `fillUserBase()` 函数

### 项目统计更新
- Head/ 从 23 增至 25 个头文件（Entities 新增 NursingRecord.h，Modules 新增 DrugInteractionUtils.h）
- Source/Roles/ 从 5 增至 8 个源文件（Admin 拆分为 4 文件）
- 总文件数：25 头文件 + 18 源文件 = 43 个 C++ 源文件

---

## 2026.4.8 — 项目初始化

- 创建核心医疗信息类：`Registration`、`Consultation`、`Examination`
- 创建 `UI.h` 并完成 6 个控制台菜单界面（主菜单、管理员菜单、科室/记录管理菜单）
- 实现管理员注册、登录、账号解锁/封禁（泛型方式）
- 实现挂号记录的查看（按科室/状态/患者ID/医生ID检索）、状态修改、逻辑删除

---

## 2026.4.9 — 输入校验与数据持久化

- 在 `UI.h` 中实现 6 个输入校验函数（范围选择、费用、字符串、ID、记录ID、密码）
- 封装挂号管理操作为独立函数，新增 `addRegistration()` 创建挂号记录
- 创建 `LoadData.h` / `SaveData.h`，实现管理员数据及 4 种医疗记录（挂号/看诊/检查/住院）的加载与保存
- 创建 `Login.h`，实现管理员登录验证
- 在 `User` 基类中添加 6 个状态枚举转字符串工具函数
- 搭建 `main.cpp` 主程序框架；修复 `Admin::next` 指针野指针导致的崩溃

---

## 2026.4.10 — VS Code 调试配置与看诊管理

- 完善 `.vscode/tasks.json`（CMake 配置、build 清理）和 `launch.json`（调试后自动清理、外部终端）
- 实现挂号记录加载函数
- `Consultation` 新增 `isHospitalizationRecommended` 和 `isPrecriptionReviewed` 属性
- 实现管理员看诊信息管理（9 个函数：查看/过滤/修改/删除/添加），配套 2 个 UI 菜单
- 新增 `trim()` 辅助函数和 `conStatusToString()` 状态转换
- **策略变更**：ID 计数器从 `User` 基类移至全局变量

---

## 2026.4.11-4.12 — 检查/住院/床位管理

- 补全看诊、检查、住院、床位信息的 `LoadData` / `SaveData` 函数
- 新增 `bedInfo` 结构体，实现床位状态转字符串
- 实现管理员检查记录管理（9 个函数）和住院记录管理（11 个函数），配套 UI 菜单
- 实现床位信息管理（11 个函数：增删查改），配套 3 个 UI 菜单和床位 ID 校验

---

## 2026.4.13-4.14 — 用药/药品/用户属性扩展

- 实现用药记录管理（13 个函数）和药品信息管理（21 个函数，覆盖所有属性的增删改查）
- `User` 新增性别、年龄、电话、邮箱属性，同步修改注册/保存/加载函数
- **Bug 修复**：CSV 分隔符冲突 — 备注字段中的英文逗号导致解析崩溃，优化 `inputStringCheck` 禁止输入英文逗号
- 完善药品结构体属性，新增 4 个药品管理 UI 菜单

---

## 2026.4.15-4.16 — 人员管理全面实现 & 费用规则

- 实现医生（22 个函数）、护士（22 个）、药剂师（20 个）、患者（27 个）的增删改查
- 实现各角色类的 setter/getter、数据持久化（保存/加载）
- 在 `Login.h` 中实现管理员账户管理（14 个函数，含泛型激活/封锁）
- **费用规则制定**（`User.h`）：
  - 挂号费：按医生职称（实习10 / 住院20 / 主治30 / 副高40 / 正高50 元）
  - 检查费：按项目名称（5-30 元，共 14 项）
  - 住院费：按病房类型×天数（普通50 / 隔离100 / VIP200 / ICU500 元/天）

---

## 2026.4.17-4.18 — 医生功能 & 全面重构优化

- 实现医生挂号（9 个函数）、看诊（19 个）、检查（16 个）管理，配套 UI 菜单
- 重构 `Admin.h` 中全部 8 个管理模块（检查/住院/用药/药品/医生/护士/药剂师/患者），优化 152 个函数
- 修正各角色注册函数返回值设计
- **规范**：所有结构体 String 类型空值统一使用 `"#"` 哨兵
- 实现医生/管理员个人信息管理（查看/修改），配套 UI 菜单
- `Login.h` 使用泛型编程统一实现各角色账户激活/封锁

---

## 2026.4.20-4.22 — 患者/药剂师/护士功能完善

- 实现患者全部功能：挂号（10 个）、看诊（8 个）、检查（9 个）、用药（10 个）、住院（13 个）管理，新增 `balance` 属性
- 实现患者、药剂师登录函数
- 集成小组成员的 `Nurse` / `Pharmacist` 代码
- 实现药剂师用药/药品管理、护士住院/检查/床位管理，配套全部 UI 菜单
- 修正枚举类型起始值（全部从 1 开始）
- 修复床位 ID 唯一性校验漏洞
- 调试配置优化：`launch.json` 设置 `cwd` 和 `externalTerminal`

---

## 2026.4.24 — 数据初始化 & UI 优化

- 批量生成 100 条测试药品数据，为 5 个科室各初始化 60 条床位数据
- 修复 `main.cpp` 中药品管理科室选择循环的死循环 Bug（缺失 `break`）
- 统一优化控制台交互界面格式

---

## 2026.4.25 — 全面代码审核（35+ Bug 修复）

**Critical：**

- `Login.cpp` 遍历链表缺失 `current = current->next` 导致死循环
- `pharmacistLogin` 硬编码 `if (id == "0")` 绕过登录
- `SHA-256.cpp` 时序攻击漏洞 — 改用 volatile XOR 逐字节比较
- `User.cpp` ID 生成未定义行为（同一表达式修改和读取变量）
- 所有角色登录成功后未重置 `loginAttempts` 计数器

**High：**

- `Admin.cpp` 约 50 处 modify 函数传错指针（头指针 vs 目标指针）
- `Admin.cpp` `modifyMedicationRecordReviewStatus` 用错字段（`status` vs `reviewStatus`）
- `Doctor.cpp` `while(true)` 无限循环、处方显示错误（"药品名称"→"药品ID"）、费用计算用错对象
- `Pharmacist.cpp` 变量遮蔽导致状态过滤失效
- `Patient.cpp` 密码修改 `this->salt` → `this->storedHash`

**Medium/Low：**

- 拼写修正 `AdminPersionalInfo` → `AdminPersonalInfo`
- `deleteAdmin` 从物理删除改为逻辑删除
- `selectIntCheck` 范围修正

---

## 2026.4.25（续）— 终端体验 & 操作日志

- **颜色系统**：新增 `ConsoleColor` 枚举 + `printSuccess()` / `printError()` / `printWarning()` + 标题美化
- **分页显示**：`printWithPagination()` 支持上一页/下一页/跳转
- **面包屑导航**：`pause()` 新增 `breadcrumb` 参数（如 "管理员 > 医疗记录 > 挂号管理"）
- **隐藏密码输入**：`inputHiddenPwdCheck()` 使用 Windows `ReadFile` API
- **操作日志**：`LogManager` 单例类，线程安全，结构化日志持久化到 `Data/OperationLog/his_YYYY_MM_DD.log`

---

## 2026.4.26 — 新功能全面接入 & 6 项关键 Bug 修复

**接入：**

- 250+ 处裸 `std::cout` 替换为彩色打印函数
- 300+ 处 `pause()` 添加面包屑路径
- 全部角色登录/注册接入操作日志
- 管理员用户列表接入分页显示

**Critical Bug：**

- `Admin.cpp` `addHospitalization`：`bedFound = true` 在 `break` 之后（死代码），床位分配始终失败
- `Admin.cpp` `viewConsultationByID`：链表遍历缺失推进语句，死循环
- 用药记录 11 处状态显示错误：`statusStr`（缴费状态）误用于审核状态列
- 5 个登录函数不检查 `isDeleted` 标志，已删除账户仍可登录
- `Patient.cpp` `applyForDischarge`：日期计算改用 `std::mktime` + `std::difftime` 精确计算

**High/Medium Bug：**

- 5 个登录函数缺少密码重试机制（改为 3 次重试）
- `LoadData.cpp` `loadBedInfos`：`std::stoi` 缺少 try-catch
- `Doctor.cpp` 处方数量上限 `INT_MAX` → 10000
- `main.cpp` 退出时新增 12 条链表完整内存清理

---

## 2026.4.27 — REST API 后端 + Vue 3 前端全面实现

### 后端 API 化

**新增文件：** `ApiResponse.h`、`JsonHelper.h/cpp`、`JWTAuth.h/cpp`、`ApiServer.h/cpp`、`server_main.cpp`

**架构：**

- 引入 cpp-httplib（header-only）和 nlohmann/json（header-only），零外部构建依赖
- `DataManager` 单例 + `std::mutex` 线程安全
- JWT 认证（HMAC-SHA256，24h 有效）
- 统一响应格式 `{code, message, data}`，CORS `Access-Control-Allow-Origin: *`
- 60+ API 端点，覆盖 5 种角色核心业务

**关键设计：** 零侵入现有代码，API 层作为独立模块，直接操作链表数据结构，写操作后立即持久化

### 前端 Web 界面（Vue 3 + Element Plus）

**技术栈：** Vue 3 + Vite 6 + Pinia + Vue Router + Element Plus (zh-cn) + Axios (JWT 拦截器)

**架构：**

- `api/` 按角色拆分 8 个模块（axios 实例 + JWT 拦截器 + 401 自动跳转）
- `store/user.js` Pinia 状态（token/角色/localStorage 持久化）
- `router/index.js` 角色路由守卫 + 组件懒加载
- `Layout.vue` el-container 布局（侧边栏菜单按角色动态显示）
- 39 个文件：3 配置 + 1 HTML + 9 JS 基础层 + 2 公共页面 + 24 角色页面

**角色页面分布：**

| 角色 | 页面数 | 核心功能 |
| ------ | -------- | --------- |
| 管理员 | 12 | Dashboard + 人员/记录/药品/床位 CRUD |
| 医生 | 4 | 挂号/看诊/检查 + Profile |
| 护士 | 4 | 住院/体征录入/床位 + Profile |
| 药剂师 | 3 | 用药审核/药品库存 + Profile |
| 患者 | 6 | 预约挂号/支付/查看记录/充值 + Profile |

### 关键 Bug：互斥锁死锁

**根因**：API handler 持有 `std::mutex` 锁后调用 `saveAll()`，而 `saveAll()` 内部对同一非递归互斥锁再次加锁

**修复**：新增 `saveAllUnsafe()`（不加锁版本），33 处写操作端点替换调用

---

## 2026.4.27（续）— 前端功能全面补全

### 后端新增 18 个端点

| 角色 | 端点数 | 核心功能 |
| ------ | -------- | --------- |
| 通用 | 1 | 检查项目列表 |
| 管理员 | 3 | 添加药品/床位、编辑个人信息 |
| 医生 | 3 | 从挂号创建看诊、从看诊批量创建检查、编辑个人信息 |
| 护士 | 8 | 创建住院、分配床位、出院办理、床位 CRUD、编辑个人信息 |
| 药剂师 | 5 | 创建用药记录、药品 CRUD、编辑个人信息 |
| 患者 | 1 | 住院押金缴纳 |

### 前端 13 个页面更新

- **医生 Registrations.vue 重写**：开始看诊对话框（主诉/病史/诊断/检查项目多选/建议住院）
- **护士 Hospitalizations.vue 重写**：创建住院/分配床位/出院办理完整流程
- **药剂师 MedicationRecords.vue 重写**：从看诊处方自动创建用药记录 + 药品行明细子表格
- **患者页面增强**：Consultations 完整详情（处方子表格）、Examinations 生命体征区块、Hospitalizations 缴纳押金
- **所有角色 Profile.vue**：统一新增编辑/查看切换功能

### 前端设计决策

- 前端分页（数据量小，`slice()` 避免后端复杂度）
- 角色菜单隔离（`v-if="store.role === X"`，后端 JWT 保证权限）
- Vite proxy 代理开发环境 CORS，生产环境 Nginx 反向代理
- 直接对接真实 API，无 Mock 数据

---

## 2026.4.29 — 代码质量审查 & 前端全面优化

### 1. /simplify 代码审查（4 项修复）

- **死代码清理**：删除 `checkRoleAuth()` / `checkAuth()` 未调用函数（25 行）
- **修改密码重构**：10 次 O(n) 遍历 → 单次遍历 + 缓存 `User*` 指针，SHA256 计算 2 次 → 1 次
- **床位 ID 去重**：17 行手动 if/else → 调用已有 `autoGenerateBedID()` 工具函数
- **药品 ID 溢出防护**：缓存 `to_string` 结果，显式判断长度后再零填充

### 2. CLAUDE.md 文档优化（85 → 92 分）

- 补充 VS Code 任务、`postDebugTask` 陷阱、零外部构建依赖
- 新增 REST API 架构、前端架构、角色编号差异说明
- 新增 Known Gotchas、`"#"` 哨兵值规范章节

### 3. 前端路由权限控制

- 全部 28 个子路由添加 `meta: { roles: [...] }` 角色限制
- 路由守卫新增角色校验，不匹配时自动重定向到角色默认页面
- Axios 拦截器细分 401/403/404/超时错误处理

### 4. 后端 API 补全

- `PUT /api/auth/change-password` 支持 5 种角色修改密码

### 5. 控制台代码精简

- `main.cpp` 管理员账户管理模块简化（-425 行冗余嵌套循环）

### 6. README.md 大幅扩充

- 新增两种运行模式的完整编译/运行/调试说明
- 前后端联调启动步骤、API 测试示例
- 角色编号映射表

### 7. 前端 UI 全面重构 — "Pure & Clinical" 风格

- `global.css` 2 行 → 167 行 CSS 变量主题系统（蓝白主色 `#1e88e5`、磨砂玻璃、阴影层次）
- Login/Register：蓝白渐变 SVG 背景 + 医疗十字 Logo + 磨砂玻璃卡片
- Layout：深色侧边栏 → 白色侧边栏 + 蓝色激活态 + 页面切换动画
- Dashboard：自定义悬浮卡片（图标 + 数值 + 独立主题色）
- 全部 26 个 CRUD 页面添加 `fade-in` 动画

**UI 重设计统计：** 30 个文件变更，+1032 / -400 行

---

## 2026.4.29（续）— 权限修复 & 管理员 CRUD

### 10. 前端权限路由修复

**问题**：非管理员登录 Dashboard 显示 "无权限"（调用了管理员专属 API）

**修复**：

- `/dashboard` 限制 `meta: { roles: [1] }`，仅管理员可访问
- Login/Register 按角色重定向（管理员→Dashboard，医生→挂号列表，护士→住院管理，药剂师→用药审核，患者→预约挂号）

### 11. 布局空白修复

- `Layout.vue` `el-main` padding 从 20px → `16px 20px`

### 12. 管理员创建功能（9 个 POST 端点）

| 端点 | 功能 |
|------|------|
| `POST /api/admin/{doctors,nurses,pharmacists,patients}` | 添加人员（含 SHA-256 密码加密） |
| `POST /api/admin/{registrations,consultations,examinations,hospitalizations,medication-records}` | 添加医疗记录 |

**前端**：12 个 Admin Vue 文件新增 "新建" 按钮 + 创建对话框 + `handleCreate()` 逻辑

### 13. CLAUDE.md 二次优化

- 新增 `npm run preview` 命令、路由数量更新（~124）、`saveAllUnsafe()` 命名约定说明

### 14. 代码统计

```text
后端：+200 行（9 个 POST 端点）
前端 API：+9 个 create 函数（admin.js）
前端页面：12 个 Admin 文件各 +40~80 行
修复：3 个文件 +20 行（router/Login/Register）
```

---

## 2026.4.30 — Phase 3 数据分析模块集成 & 代码审查修复

### 1. 数据分析模块集成（Phase 3 收尾）

**新增文件：** `Head/DataAnalysis.h`、`Source/DataAnalysis.cpp`

**功能：**

- 月度科室统计（入院/出院人数、挂号总数、住院费用、床位日数）
- 需求预测（3个月移动平均 + 线性回归 + 增长率）
- 床位分配分析（利用率、平均住院天数、预测需求、推荐床位数）
- 床位调配建议（调入/调出科室及数量）
- 药品统计（库存量、进销价值、利润率）
- 三种展示格式：表格报告、ASCII 可视化图表、摘要报告

**集成路径：**

- `Admin.h` 新增 `showDataAnalysisReport()` 方法声明
- `Admin.cpp` 实现该方法，依次调用三种展示格式
- `UI.cpp` `adminReportMenu()` 新增选项 6 "数据分析与预测"
- `main.cpp` 添加 case 6 分发调用

### 2. 关键编译错误修复

| 错误 | 根因 | 修复 |
|------|------|------|
| C3861 `printMenuBorder` 未找到 | 函数为 UI.cpp 中 static，Admin.cpp 不可见 | 改用 `printTitle()` 重写展示逻辑 |
| LNK2019 链接错误 | CMake GLOB 未识别新文件 | 重新运行 `cmake ..` 配置 |
| C3646 `VitalSigns` 未知类型 | Hospitalization.h 未包含 Examination.h | DataAnalysis.cpp 中调整 include 顺序 |
| C2027 `Registration` 未定义 | 仅有前向声明，DataAnalysis.cpp 需解引用 | 添加 `#include "Registration.h"` |
| C3861 `deptMatch` 未找到 | 在 Admin.cpp 中为 static | 提取为 UI.h/UI.cpp 共享工具函数 |
| C4477 snprintf 警告 | `std::string` 传给 `%s` 未调 `.c_str()` | 全部添加 `.c_str()` 调用 |

### 3. /simplify 代码审查（多项修复）

- **deptMatch 去重**：Admin.cpp 和 DataAnalysis.cpp 各有一份 static 副本 → 提取到 UI.h/UI.cpp
- **analyzeMedicineStats 效率优化**：6 次遍历药品链表（每个科室+全院各一次）→ 单次遍历 + `std::map` 聚合 → 构建结果向量
- **ASCII 图表 maxPred 修复**：每次循环重复计算最大预测值 → 提前计算一次
- **未用参数清理**：`runFullAnalysis()` 的 `flowHead` 参数无实际用途 → 从签名中移除
- **库存状态简化**：冗余的三层条件判断 → 简化为一次比较
- **CLAUDE.md 优化**：补充 MedicineFlow、medicine_flow.txt、"mfl" 前缀、deptMatch 工具、DataAnalysis 模块说明

### 4. 构建状态

```text
his.exe — 编译通过
his_server.exe — 编译通过
```

### 5. 待后续处理

- Phase 4：前端数据分析页面适配（Vue 3 可视化、API 端点）
- 药品别名显示块提取（Admin.cpp 中 6 处重复）
- 时间范围选择块提取（report 函数中 4 处重复）
- 床位利用率阈值常量（85、50 硬编码）

---

## 2026.4.30（续）— 课程设计测试数据生成

### 1. 测试数据批量生成

编写独立 C++ 生成器 `generate_test_data.cpp`，调用项目 SHA-256 模块统一生成密码哈希，满足课程设计要求：

| 数据类型 | 数量 | 说明 |
|----------|------|------|
| 管理员 | 5 | 含原有 gcy，新增 4 名 |
| 医生 | 21 | 每科室 4 名 + gcy，覆盖 5 科室 |
| 护士 | 21 | 每科室 4 名 + gcy，覆盖 5 科室 |
| 药剂师 | 21 | 每科室 4 名 + gcy，覆盖 5 科室 |
| 患者 | 101 | 含原有 gcy，新增 100 名（前 30 名住院） |
| 挂号记录 | 131 | 住院挂号 30 条 + 门诊挂号 101 条 |
| 看诊记录 | 130 | 每条挂号对应一条看诊 |
| 检查记录 | 90 | 每条看诊附带 0-2 项检查 |
| 住院记录 | 30 | 对应前 30 名患者 |
| 床位分配 | 30 | 更新 bed_info.txt 中 30 张床位为 OCCUPIED |
| 用药记录 | 90 | 每条看诊处方对应一条用药记录 |

**密码统一**：所有新建账户密码为 `jlu123456`，SHA-256 加盐 1000 次迭代

### 2. 生成过程 Bug 修复

| 问题 | 根因 | 修复 |
|------|------|------|
| 编译错误 `std::to_string` 类型不匹配 | `int` 和 `""` 混用 | 移除死代码，统一使用 `ostringstream` |
| 0/30 床位更新成功 | 床位 ID 格式不匹配（生成器用 `N1-P1-1`，实际为 `N-03-P-001-01`） | 读取实际 bed_info.txt 提取正确 ID |
| 26/30 床位更新 | VIP/ICU 病房仅 3 个（非 5 个），第 3 病房仅 2 床 | 调整 `maxWard=2`，使用 8 张床/科室/类型 |

### 3. 验证结果

- 全部数据文件写入成功，数量符合课程要求
- 密码哈希格式正确（16 字符盐 + `$` + 64 字符十六进制）
- 30 张床位状态更新为 OCCUPIED，关联患者 ID 和护士 ID
- `his.exe` 和 `his_server.exe` 均编译通过

### 4. CLAUDE.md 质量审计

使用 CLAUDE.md Improver 技能进行评估：**93/100（Grade A）**

| 维度 | 得分 | 说明 |
|------|------|------|
| 命令/工作流 | 20/20 | 完整的构建、运行、调试命令 |
| 架构清晰度 | 18/20 | 类层次、数据结构、API 架构均有说明 |
| 非显而易见的模式 | 14/15 | Gotchas 章节覆盖关键陷阱 |
| 简洁性 | 14/15 | 内容密度高，无冗余 |
| 时效性 | 14/15 | 反映当前代码状态 |
| 可操作性 | 13/15 | 命令可直接复制执行 |

**结论**：无需修改，文件已足够完善

---

## 2026.5.2 — 课程设计全面审查与补全（6步流程）

### Step 1: 后端代码审查

对照 PDF 课程设计要求，检查纯后端（main.cpp 控制台模式）代码，发现 20 项问题：

| 级别 | 数量 | 关键项 |
|------|------|--------|
| Critical | 2 | 排班管理模块缺失、处方打印功能缺失 |
| High | 8 | 转科流程不完整、费用明细缺失、CSV 逗号冲突、药品流水 API 缺失等 |
| Medium | 6 | 报表统计、床位编辑、记录删除等 |
| Low | 4 | 边界情况、UI 提示优化 |

### Step 2: 前后端贯通检查

验证 REST API 覆盖率约 85%，识别出 P0-P3 分级的前后端缺口。

### Step 3: P0-P2 全部修复 + 前端美化

#### P0 修复（3 项）

| 功能 | 后端 | 前端 |
|------|------|------|
| 排班管理 | `g_schedules` vector + JSON 持久化 `Data/schedules.json`，5 个端点（CRUD + 公开查询） | `admin/Schedules.vue`（管理员 CRUD）、`patient/Schedules.vue`（患者只读） |
| 处方打印 | — | `patient/Consultations.vue` 新增 `printPrescription()`，`window.open()` 打印格式化处方 |
| 转科办理 | `POST /api/nurse/hospitalizations/:id/transfer` | — |

#### P1 修复（2 项）

| 功能 | 后端 | 前端 |
|------|------|------|
| 统计报表 | 6 个报表端点（overview/department/doctor-workload/patient/bed-utilization/medicine-inventory） | `admin/Reports.vue`（概览卡片 + 科室统计 + 医生工作量 + 床位利用率 + 药品库存） |
| 药品流水 | `GET /api/admin/medicine-flows`、`GET /api/pharmacist/medicine-flows` | `admin/MedicineFlows.vue`（类型标签 + 药品筛选） |

#### P2 修复（6 项）

- `api/doctor.js` 新增 3 个 delete 函数（registration/consultation/examination）
- `api/patient.js` 新增 deleteRegistration
- `api/admin.js` 新增 updateBed/updateRegistration/updateConsultation/updateExamination + 4 个 schedule 函数 + 6 个 report 函数 + getMedicineFlows
- `doctor/Registrations.vue` 新增删除按钮
- `doctor/Consultations.vue` 新增删除按钮
- `doctor/Examinations.vue` 新增删除按钮
- `patient/Registrations.vue` 新增取消按钮
- `admin/Beds.vue` 新增编辑按钮 + 编辑对话框

#### 前端美化

- `global.css`：167 行 → 296 行。主色 `#1e88e5` → `#1976d2`，新增 teal accent `#00897b`，引入 Noto Sans SC 字体，新增 radius/font CSS 变量，增强 card/dialog/table/form 样式，新增 stagger 动画和 print 样式
- `Dashboard.vue`：统计卡片渐变顶边框、tabular-nums 字体、图标 hover 缩放、accent 边框色过渡
- `Login.vue`：磨砂玻璃增强（blur 16px）、cardAppear 关键帧、logoPulse 动画、按钮渐变 + translateY 悬浮

#### 路由 & 布局更新

- `router/index.js` 新增 4 条路由：admin/schedules、admin/reports、admin/medicine-flows、patient/schedules
- `Layout.vue` 新增"系统管理"子菜单（排班管理、统计报表）、药品流水菜单项、患者排班查询菜单项

#### 构建验证

```text
his.exe — 编译通过
his_server.exe — 编译通过
```

### Step 4: CLAUDE.md 更新

- 新增 `schedules.json` 到数据持久化表
- API 端点数 ~125 → ~140，补充排班/报表/转科/药品流水端点文档
- 前端文件数 34 → 38，页面分布更新（admin 13→16、patient 6→7）
- 补充处方打印功能说明
- `global.css` 行数 167 → 296，更新描述

### Step 5: 上下文压缩

执行 `/compact` 压缩会话上下文。

### Step 6: 开发日志

本条日志。

### 代码统计

```text
后端 Source/ApiServer.cpp：+~490 行（15 个新端点 + 排班 JSON 持久化）
前端 api/：+30 个函数（admin.js 17、doctor.js 3、patient.js 1、+ schedule/report/flow）
前端 views/：4 个新页面 + 6 个页面增强
前端 styles/global.css：+129 行
前端 router/Layout：+4 路由 + 3 菜单项
CLAUDE.md：+30 行更新
```

---

## 2026.5.6 — 后端课程设计审阅问题集中修复

### 1. 修复背景

根据《程序设计基础课程设计（2025级）》中的“全程链表实现”和后端一致性要求，对纯后端项目进行二次审阅后，集中修复 5 条较高优先级问题：

| 编号 | 优先级 | 问题 | 影响 |
|------|--------|------|------|
| Finding 1 | P1 | API 新建记录 ID 为纯数字 | 与控制台 `reg/con/exa/hos/mrd/med` 前缀格式不兼容 |
| Finding 2 | P1 | 控制台紧急保存使用旧 head 指针 | 注册或头插新增后，异常保存可能漏掉新头节点 |
| Finding 3 | P2 | API 出院天数按固定 30 天月份粗算 | 跨月、跨年或非 30 天月份费用计算不准确 |
| Finding 4 | P2 | API 管理员头插未维护 `prev` | 破坏管理员双向链表完整性 |
| Finding 5 | P2 | 持久业务子集合仍使用 `std::vector` | “全程链表实现”答辩合规性存在风险 |

### 2. 业务持久子集合链表化

新增 `Head/LinkedList.h`，实现一个 RAII 双向链表容器，用于替代持久业务结构体中的 `std::vector` 子集合。

#### LinkedList 支持能力

- `push_back()`
- `empty()`
- `size()`
- `begin()` / `end()` / range-for
- `erase(iterator)`
- `operator[]`
- `begin() + n`、`begin() + n - 1` 等旧代码兼容写法
- 拷贝构造、移动构造、拷贝赋值、移动赋值
- 析构自动释放节点，避免子集合内存泄漏

#### 替换范围

| 文件 | 字段 | 修改 |
|------|------|------|
| `Head/Registration.h` | `relatedRegistrationIDs` | `std::vector<std::string>` → `LinkedList<std::string>` |
| `Head/Consultation.h` | `examinationlist` | `std::vector<std::string>` → `LinkedList<std::string>` |
| `Head/Consultation.h` | `prescriptions` | `std::vector<Prescription>` → `LinkedList<Prescription>` |
| `Head/Consultation.h` | `attachments` | `std::vector<std::string>` → `LinkedList<std::string>` |
| `Head/Consultation.h` | `relatedConsultationIDs` | `std::vector<std::string>` → `LinkedList<std::string>` |
| `Head/Examination.h` | `attachments` | `std::vector<std::string>` → `LinkedList<std::string>` |
| `Head/Examination.h` | `relatedExaminationIDs` | `std::vector<std::string>` → `LinkedList<std::string>` |
| `Head/Hospitalization.h` | `relatedHospitalizationIDs` | `std::vector<std::string>` → `LinkedList<std::string>` |
| `Head/MedicationRecord.h` | `lines` | `std::vector<MedicationLine>` → `LinkedList<MedicationLine>` |
| `Head/Medicine.h` | `aliases` | `std::vector<std::string>` → `LinkedList<std::string>` |

#### 保留 `std::vector` 的范围

按照“持久业务数据结构链表化，临时计算/展示容器不强行链表化”的原则，以下场景保留标准容器：

- `DataAnalysis` 中的统计结果、排序缓存、图表数据
- `UI` 中的临时分页展示数据
- `JWTAuth`、`SHA-256` 等算法内部缓冲
- API 排班 `g_schedules`，该部分使用 JSON 持久化且属于服务器端辅助配置

### 3. API 记录 ID 格式修复

在 `Source/ApiServer.cpp` 中新增 `generateRecordID(prefix, counter)`，统一生成控制台兼容的业务记录 ID。

#### 修复前

API 使用 `generateID(5/6/7/8/9, counter)`，生成纯数字 ID，例如：

```text
500001、600001、700001
```

这种 ID 无法通过控制台 `inputRecordIDCheck()` 的前缀校验。

#### 修复后

API 与控制台统一使用：

| 业务类型 | ID 格式 |
|----------|---------|
| 挂号记录 | `reg000000` |
| 看诊记录 | `con000000` |
| 检查记录 | `exa000000` |
| 住院记录 | `hos000000` |
| 用药记录 | `mrd000000` |
| 药品信息 | `med000000` |

用户账号 ID 仍保留原有 6 位纯数字角色前缀格式：

```text
0xxxxx 管理员
1xxxxx 医生
2xxxxx 护士
3xxxxx 药剂师
4xxxxx 患者
```

#### 同步修复 API 路由

由于业务记录 ID 变为带前缀字符串，原本只匹配 `(\d+)` 的业务记录路由同步放宽为 `([^/]+)`，覆盖：

- 管理员：挂号、看诊、检查、住院、用药记录、药品
- 医生：看诊、检查
- 护士：住院、检查
- 药剂师：用药记录
- 患者：挂号支付、检查支付、用药支付、住院支付

纯数字用户账号路由、排班路由保持不变。

### 4. API 链表头插统一修复

在 `Source/ApiServer.cpp` 中新增统一头插 helper：

```cpp
template <typename T>
void pushFront(T *&head, T *node)
{
    node->prev = nullptr;
    node->next = head;
    if (head)
        head->prev = node;
    head = node;
}
```

替换 API 中新增用户、记录、床位、药品时的手写头插逻辑，保证：

- 新头节点 `prev == nullptr`
- 旧头节点 `prev` 正确指向新头节点
- 管理员注册头插不再遗漏 `prev`
- 各类业务记录新增保持双向链表不变式

### 5. 控制台紧急保存头指针修复

`main.cpp` 原先将 `g_adminHead`、`g_regHead` 等全局指针设置为加载后的 head 值。

问题在于：注册或新增记录采用头插法后，局部 `adminHead/regHead/...` 会变成新节点，但全局 `g_*Head` 仍指向旧头节点。若此时异常或 `Ctrl+C` 触发 `emergencySave()`，新插入的头节点可能不会写入文件。

#### 修复方式

将全局紧急保存指针改为“指向当前 head 变量的指针”：

```cpp
static Admin **g_adminHead = nullptr;
static Registration **g_regHead = nullptr;
```

初始化时绑定局部 head 变量地址：

```cpp
g_adminHead = &adminHead;
g_regHead = &regHead;
```

保存时解引用当前 head：

```cpp
if (g_adminHead && *g_adminHead)
    saveAdminData(*g_adminHead, adminIDCount);
```

这样无论之后发生多少次头插，紧急保存都能拿到最新链表头。

### 6. 住院天数计算公共化

在 `User.h/User.cpp` 中新增：

```cpp
static int calculateStayDays(const std::string &admitTime,
                             const std::string &dischargeTime);
```

实现要点：

- 使用 `std::tm`、`std::mktime`、`std::difftime` 计算真实日期差
- 支持跨月、跨年
- 日期无效、为空或 `"#"` 时默认返回 1 天
- 不足 1 天按 1 天计费
- 使用 `mktime` 归一化后反查年月日，过滤非法日期

复用位置：

- `Source/ApiServer.cpp`：护士 API 出院结算
- `Source/Patient.cpp`：患者控制台申请出院结算

修复后不再使用 API 原先的 `(nowMon - admitMon) * 30 + (nowDay - admitDay)` 粗略算法。

### 7. JSON 与药品别名兼容

由于药品别名 `aliases` 从 `std::vector` 改为 `LinkedList`，同步更新：

- `JsonHelper::toJson(const Registration*)`：将 `relatedRegistrationIDs` 手动转 JSON array
- `JsonHelper::toJson(const Medicine*)`：新增输出 `genericName` 和 `aliases`
- `ApiServer.cpp`：新增 `applyMedicineNames()`，支持 API 创建/修改药品时读取 `genericName` 和 `aliases`

数据文件格式未改变，`LoadData.cpp` / `SaveData.cpp` 原有子行格式继续工作：

```text
ALIAS:药品别名
PRESCRIPTION:...
ATTACHMENT:...
RELATED_CONSULTATION_ID:...
```

### 8. 回归测试补充

新增 `tests/backend_regression.cpp`，并在 `CMakeLists.txt` 中增加目标：

```text
his_backend_regression
```

测试覆盖：

- `LinkedList` 空判断、尾插、下标访问
- `erase(begin() + n)` 兼容旧代码用法
- 拷贝构造为深拷贝
- range-for 遍历
- 住院天数跨月计算
- 住院天数跨年计算
- 同日不足 1 天按 1 天
- 非法/空日期按 1 天兜底

### 9. 验证结果

执行构建验证：

```powershell
cmake --build build --config Debug --target his his_server his_backend_regression
```

结果：

```text
his.exe — 编译通过
his_server.exe — 编译通过
his_backend_regression.exe — 编译通过
```

执行回归测试：

```powershell
cd build
.\Debug\his_backend_regression.exe
```

结果：

```text
退出码 0，全部 assert 通过
```

额外静态核对：

- `Source/ApiServer.cpp` 中不存在 `generateID(5/6/7/8/9, ...)` 业务记录 ID 生成调用
- 业务记录 API 路由不再使用纯数字 `(\d+)` 匹配
- API 出院逻辑不再出现 `admitMon/nowMon` 或固定 30 天月份算法
- API 管理员新增不再手写 `newUser->next = dm.getAdminHead()`，统一走 `pushFront()`

### 10. 本次涉及文件

```text
CMakeLists.txt
Head/LinkedList.h
Head/Registration.h
Head/Consultation.h
Head/Examination.h
Head/Hospitalization.h
Head/MedicationRecord.h
Head/Medicine.h
Head/User.h
Source/ApiServer.cpp
Source/JsonHelper.cpp
Source/Patient.cpp
Source/User.cpp
main.cpp
tests/backend_regression.cpp
```

### 11. 后续可选优化

- 将 `pushFront()` 从 `ApiServer.cpp` 抽为公共链表工具函数，供控制台和 API 共用
- 为 API 创建记录流程增加更完整的自动化集成测试
- 如老师要求更严格，可继续把排班 `g_schedules` 从 `std::vector<json>` 改为链式结构或在答辩中说明其为服务器配置数据

---

## 2026.5.7 — 统计报表优化

**1. 移除"数据分析与预测"菜单入口**

- `UI.cpp` `adminReportMenu()` 移除选项 6，`selectIntCheck` 范围改为 `(0,5)`
- `main.cpp` 移除 `reportChoice == 6` 的 dispatch 调用
- `CLAUDE.md`、`README.md` 移除 DataAnalysis 模块引用

**2. 医生工作量统计重构**

- 根因：Doctor 结构体的 `consultationCount`/`examinationCount`/`hospitalizationApplyCount` 字段从未被业务逻辑自增，始终为 0
- 方案：改为动态遍历 Consultation、Examination、Hospitalization 链表按 doctorID 聚合统计
- 新增：时间范围选择（复用 `timeRangeMenu`）、科室筛选（全院/内科/外科/妇产科/急诊科/儿科）
- 排版：`std::setw` 列对齐 + 分隔线 + 底部合计行
- 文件：`Head/Admin.h` 函数签名扩展，`Source/Admin.cpp` 函数重写，`main.cpp` 调用处更新

---

## 2026.5.8 — 冗余架构精简、UI 细节修复及最终文档归档

### 1. 架构精简：移除 `MedicineFlow` 模块

为精简项目架构并去除过度设计的特性，进行了全栈移除“药品流水”及关联功能的手术：

- **后端**：在 `LoadData.cpp` 和 `SaveData.cpp` 中移除了 `MedicineFlow` 数据的加载与保存；从业务链路中解绑并彻底删除了 `Data/RecordData/MedicineChainData/medicine_flow.txt` 文件。
- **前端**：删除了视图文件 `views/admin/MedicineFlows.vue`，清理了 `router/index.js` 中的相关路由映射、`Layout.vue` 侧边栏菜单以及 `api/admin.js` 中的 API 接口调用。

### 2. 控制台 UI 交互体验修复

- 修复了 `Source/Nurse.cpp` 和 `Source/Patient.cpp` 等模块中存在的“提示信息闪退”的 Bug。
- 在用户撤销修改或进行部分失败操作出错的分支后，补充了 `pause(breadcrumb)` 拦截等待，确保状态提示信息对用户可见，优化了黑框命令行的交互体验。

### 3. 全链路测试与底层数据更新

- 发起了一轮详尽的全业务流端到端测试，80余项生命周期的连转操作联动落地，当天的系统操作日志 (`his_2026_05_08.log`) 完整录入了追溯记录。
- 更新并同步了包括身份注册（如 `nurse_users.txt`）、就医流程（`registrations.txt`、`consultations.txt`、`examinations.txt`）、住院（`hospitalizations.txt`、`bed_info.txt`）以及发药记录在内的所有持久化数据文件，验证了系统的磁盘 I/O 能力完全正轨。

### 4. 课程设计最终报告与可视化工程

- 深度重排并大幅扩写了 `总结报告.md` (新增 800 余行内容)。
- 新增 `custom-reference.docx` 全量参考文献及附件结构。
- 引入使用 Mermaid 绘制的前后端双向交互与角色流程，产出了 `程序主要功能简图.mmd` / `.svg`。
- 在 `Document/image/` 目录下集中整理了由系统全面运行期间捕获的高清测试截图，为最终期末答辩做好了全副武装的素材准备。

---

## 2026.5.29 — MySQL 数据库迁移 & 两阶段全面优化

### 第一阶段：业务功能增强（GLM 执行）

| Sprint | 内容 | 关键产出 |
|:---:|------|------|
| 1 | 数据完整性修复 | `isHospitalized` 状态同步、金额 `double`→`int`(分)、出院结算 `totalCost` 时序修复、`reg000035` 重复 ID 去重 |
| 2 | 药物安全系统 | `Head/DrugSafety.h` + `Source/DrugSafety.cpp`，过敏检查 + DDI 检查（30 对药物相互作用），医生开药 + 药剂师审核双端集成 |
| 3 | 科室去硬编码 | `UI::getDepartmentList()` 从 4 条角色链动态聚合唯一科室，替换 5 处硬编码数组，`inputDepartmentCheck` 接受任意非空科室 |
| 4 | Admin 修改功能补全 | 5 类记录扩展修改菜单：住院(押金/病房类型/护士ID)、检查(项目名称/报告摘要/备注)、看诊(主诉/现病史/诊断/备注)、挂号(备注)、用药(备注) |
| 5 | 护理记录模块 | `NursingRecord` 结构体 + load/save + main.cpp 集成 + Nurse CRUD + 生命体征快照，新增 `Data/RecordData/NursingRecordChainData/` |
| 6 | EntityRepository 模板 | `Head/EntityRepository.h` 泛型数据仓库（O(1) ID 查找/过滤/自动 ID 生成），NursingRecord 链示范集成 |
| 7 | Holt-Winters + Z-score | `DataAnalysis` 升级：双指数平滑预测（n≥6 覆盖简单预测）+ Z-score 异常检测（按科室），集成到 `runFullAnalysis` 管线 |
| 8 | Python AI 微服务 | `ai_service/` Flask REST API (7 端点)，C++ `AIQueryClient` HTTP 客户端（WinSock2），管理员菜单选项 9 "AI 智能分析" |

### 第二阶段：MySQL 数据库迁移（GLM 执行）

| Sprint | 内容 | 关键产出 |
|:---:|------|------|
| 1 | 数据库建表 | `Data/Schema/schema.sql` — 21 张表（14 主表 + 7 子表），外键约束（SET NULL + CASCADE），CHECK 约束，建表执行通过 |
| 2 | Database 封装层 | `Head/Database.h` + `Source/Database.cpp` — MySQL C API 封装（query/execute/事务/参数化查询/连接池），`loadConfig` 读取 `Data/DatabaseConfig.txt` |
| 3 | Load/Save 重写 | 13 个 load 函数 + 13 个 save 函数全部重写为 MySQL 版本：SELECT 加载 + INSERT ON DUPLICATE KEY UPDATE 保存 + 事务 + 回滚 |
| 4 | main.cpp 集成 | 全局 `Database g_db` 对象，连接→加载→运行→保存→断开 生命周期，ID 计数器从 `SELECT MAX` 初始化 |
| 5 | 业务 SQL 化 | 5 个统计报表使用 SQL `GROUP BY`/`SUM`/`COUNT` 聚合，保留链表回退路径 |
| 6 | 数据完整性 | 20 个外键（13 SET NULL + 4 CASCADE + 3 无动作），16 个 CHECK 约束，1 个 UNIQUE 索引 |
| 7 | 旧代码清理 | CSV 文件路径宏移除、LoadData.h/SaveData.h 注释更新、main.cpp 误导消息修正 |

### 终审修复（本会话执行）

**架构 & 文件统计：**
- 头文件 19→23（新增 Database/DrugSafety/EntityRepository/AIQueryClient）
- 源文件 13→16（新增 Database/DrugSafety/AIQueryClient）
- 记录类型 7→8（新增 NursingRecord）
- 数据存储：CSV 文本文件 → MySQL 关系型数据库（`his_db`，21 张表）

**Critical Bug 修复：**
- `Doctor::initConsultationPrescription` 缺少 DrugSafety 检查 → 添加过敏+DDI 检查（与 `addConsultationPrescription` 保持一致），函数签名新增 `Patient *patientHead` 参数，级联更新 2 个调用点

**项目清理：**
- 删除 `HIS第二阶段-MySQL数据库迁移-执行版.md`（已执行完毕的提示词）
- 删除 `glm.txt`（GLM 会话日志残留）
- 删除 `Data/Schema/__pycache__/`（Python 字节码缓存）
- 删除 `Data/UserData/`（5 个遗留 CSV 用户数据目录，共 5 个 txt 文件）
- 删除 `Data/RecordData/`（8 个遗留 CSV 记录数据目录，共 7 个 txt 文件 + 1 个空目录）

**CLAUDE.md 维护：**
- 药物相互作用对数 31→30（与实际文件一致）
- 移除已删除的 `Data/UserData/` 和 `Data/RecordData/` 目录引用
- 更新 CSV 遗留数据说明为"已移除，使用 MySQL"

**构建验证：** `his.exe` 全程零错误零警告编译通过
