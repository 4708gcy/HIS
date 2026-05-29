# HIS 医院信息管理系统 — 专业评审报告

> 评审基准：C++17 控制台课程设计项目，目标定位为"简历亮点级个人项目"  
> 评审日期：2026-05-29  
> 评审范围：全部源码、数据库脚本、配置文件、AI微服务

---

## 一、总体评价

本项目是一个功能较完整的 C++17 控制台医院信息系统（HIS），覆盖挂号→看诊→检查→住院→用药五大核心业务流程，支持 5 种角色权限隔离，已完成从 CSV 到 MySQL 的数据持久化迁移（21 张表、外键约束、事务支持），并集成了 Python Flask AI 微服务（Holt-Winters 预测、Z-score 异常检测、matplotlib 图表生成）。

**核心优势：** 业务流程完整闭环、数据库设计规范化、AI 数据分析能力（超越 95% 的同类课设）。**主要短板：** 内存链表作为核心数据模型的架构设计过时、Admin.cpp 过于臃肿（8000+ 行）、无自动化测试、部分安全性问题。

作为课设已属上乘，作为简历项目在架构现代化方面有明确提升空间。

---

## 二、架构与设计

### 2.1 当前架构

```
┌─ his.exe ──────────────────────────────────────────┐
│  main.cpp: 主循环 + 13条内存链表 + 信号处理           │
│  Admin/Doctor/Nurse/Pharmacist/Patient: 业务逻辑     │
│  Database.h: MySQL C API 单例封装                    │
│  LoadData/SaveData: MySQL ↔ 内存链表双向同步         │
└────────────────────────────────────────────────────┘
         │ SQL                        │ HTTP
         ▼                            ▼
┌─ MySQL 8.0 (his_db, 21表) ─┐  ┌─ AI Service (:5001) ─┐
```

### 2.2 核心问题

| 问题 | 位置 | 严重度 |
|------|------|:---:|
| **内存链表作为权威数据源** | `main.cpp:124-153` — 启动时全量加载到13条链表，退出时全量回写；崩溃丢数据 | 高 |
| **Admin.cpp 过于臃肿** | `Source/Roles/Admin.cpp` — 8000+行，涵盖用户管理、记录管理、报表、药品、床位 | 高 |
| **无单元测试** | 全项目零测试 | 中 |
| **std::cin 失败状态无处理** | `main.cpp:277` 等 `std::cin >> choice` 位置，输入非数字导致无限循环 | 中 |
| **CMake 硬编码绝对路径** | `CMakeLists.txt:15-16` 写死 `E:/MySQL/8-0/` | 低 |
| **循环依赖风险** | `User.h` 包含了 Registration.h、Consultation.h 等 7 个头文件 | 低 |

### 2.3 哪些不是问题

以下评审项经常被误判为缺陷，但在控制台单用户课设上下文中是合理设计：

- **"无法支持并发"**：单用户控制台应用不涉及并发场景
- **"无会话/Token机制"**：控制台登录不需要 HTTP 会话概念
- **"无HTTPS"**：本地 localhost:3307 连接，HTTPS 无意义
- **"应改用 Web 前端"**：C++ 课设的核心竞争力恰好在 C++ 后端深度，不需要自降身价去追前端框架

### 2.4 数据库设计

**优点：** 14主表+7子表拆分合理；外键策略正确（SET NULL+CASCADE）；金额统一用 INT(分)；CHECK 约束覆盖状态和金额。

**可改进：**
- 缺失复合查询索引（如 `(department, status)` on registrations）
- 部分日期字段用 VARCHAR 而非 DATE 类型
- 字符串业务 ID（如 `reg000001`）比自增 INT 浪费空间，但保持了可读性

---

## 三、角色功能与业务流程

### 3.1 角色权限

5 种角色各有独立菜单和权限过滤（科室级别隔离通过 `deptMatch()` 实现），满足课设需求。管理员 API Key "88888888" 硬编码在源码中，建议改为从配置文件读取。

### 3.2 核心流程完整性

挂号→看诊→检查→住院→用药的五大流程全部打通，数据关联完整（均通过业务 ID 字符串关联）。状态枚举定义清晰，药物安全三明治（医生开方→药剂师审核→发药扣库存）设计正确。

`Consultation` 的 `isPrescriptionReviewed` 是 `bool`，而 `MedicationRecord` 有独立的 `reviewStatus` 枚举——两者在药剂师审核时会同步，但极端情况下可能不一致。

### 3.3 库存扣减安全性

`Pharmacist::dispenseMedicine()` 在同一个函数内完成校验+扣库+更新状态，控制台单线程环境下无竞态风险。如果未来改为多线程，需引入 `SELECT ... FOR UPDATE`。

---

## 四、代码质量与安全性

### 4.1 当前质量评估

| 维度 | 评分 | 说明 |
|------|:---:|------|
| 命名规范 | A | PascalCase/camelCase/全大写宏一致，中文注释完整 |
| 内存管理 | C | 全裸指针 new/delete，但 main.cpp 清理逻辑完整，无已知泄漏 |
| 现代C++ | B | 使用了 auto、range-for、lambda、if constexpr；缺少智能指针、std::optional |
| 安全性 | B | SHA-256+盐值+恒定时间比较；账户锁定；SQL参数化查询。但1000次迭代偏低，escapeString为手动实现 |

### 4.2 具体安全发现

| 问题 | 位置 | 优先级 |
|------|------|:---:|
| SHA-256 迭代次数仅 1000 次 | `User.h:31` | 中 |
| Flask `debug=True` 暴露调试器 | `ai_service/app.py:158` | 中 |
| AI 服务无 API 认证 | `app.py` 所有端点公开 | 中 |
| 数据库密码明文存储 | `Data/DatabaseConfig.txt` | 低（.gitignore 已排除） |
| `system("cls")` 安全风险 | `UI.cpp:96` | 低（仅 Windows，无命令注入向量） |
| 管理员 API Key 硬编码 | `User.cpp:51` | 低 |

### 4.3 输入校验

`UI.h` 提供 20+ 个输入校验函数，覆盖 ID、密码、电话、金额等。但 `selectIntCheck` 等函数在 `std::cin` 失败时不清除错误状态，会导致后续输入全部被跳过。

---

## 五、AI 服务评估

Python Flask AI 微服务 v2.0 是项目亮点：
- Holt-Winters 双指数平滑预测（与 C++ 端 DataAnalysis 互补）
- Z-score 异常检测
- 床位分配分析与调配建议（通过 pandas 数据透视）
- matplotlib 生成 4 种 PNG 图表（预测对比、床位利用率、异常时序、库存饼图）
- 数据源已从 CSV 文件迁移到 MySQL（pymysql 直连）

改进方向：关闭 Flask debug 模式、添加 API Key 认证、将 host 绑定到 127.0.0.1。

---

## 六、改进优先级

### P0 — 必须修复（可执行，高影响）

| 改进 | 文件 | 工时 |
|------|------|:---:|
| 修复 `std::cin` 失败状态死循环 | `UI.cpp` 中 `selectIntCheck` 等函数 | 2h |
| API Key 配置化 | `User.cpp` → 读取 `Data/AdminAPIKey.txt` | 0.5h |
| 关闭 Flask debug + 添加 API Key 认证 | `ai_service/app.py` | 1h |
| SHA-256 迭代提升至 10000 | `User.h` | 0.1h |

### P1 — 建议改进（提升架构质量）

| 改进 | 说明 | 工时 |
|------|------|:---:|
| Admin.cpp 拆分 | 将 7 类记录管理的修改菜单逻辑抽到独立函数中，减少重复 | 8h |
| 扩展 EntityRepository | 将 Patient、Registration 链也接入模板 | 6h |
| User.h 解耦 | 移除循环包含，改为前向声明 | 2h |
| 复合索引 | 为高频查询添加 `(department, status)` 等索引 | 1h |

### P2 — 锦上添花

| 改进 | 说明 |
|------|------|
| 单元测试 | Google Test 覆盖 Repository 层 |
| 日期字段标准化 | VARCHAR → DATE/DATETIME |
| CMake 路径相对化 | 从环境变量读取 MySQL 路径 |

---

> **评审结论：** 项目已达到课程设计要求，AI 分析能力和数据库设计是突出亮点。P0 类问题应在 1 天内修复完毕；P1 重构视时间投入决定。整体架构在单用户控制台场景下是合理的，无需追逐企业级技术栈。
