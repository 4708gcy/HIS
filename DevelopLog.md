# HIS 开发日志

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
