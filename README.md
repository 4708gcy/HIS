# C 语言课程设计 --- 医疗管理系统(HIS, Hospital Information System)

## 公共约定

1. 头文件命名采用**大驼峰命名法**(就是每个单词开头都是大写字母，例如`Doctor.h`)，源文件的名字要和对应的头文件保持一致
2. 函数方法的命名采用**小驼峰命名法**(就是只有第一个单词的首字母小写，后面的字母都是大写，例如`addDoctor()`)
3. 不同人物之间依据ID来区分，ID总长度为6位，第 1 位数字用来区分不同身份，后面 5 位数字用来区分同一个身份下的不同个体，规定：
    **0** 开头表示管理员 `000001` 表示管理员1号
    **1** 开头表示医生   `100001` 医生1号
    **2** 开头表示护士   `200001` 护士1号
    **3** 开头表示药剂师 `300001` 药剂师1号
    **4** 开头表示患者   `400001` 患者1号
 为了简化，就不使用ID来区分不同科室，而是在类的属性中添加一个表示 `科室` 的属性
 这样命名也决定了在该系统中，每一个身份最多可以容纳 99999 个不同的个体
4. 所有人物类的数据在从文件中读取时，采用链表的存储方式，也就是每一个类中都应该添加一个属性 `next` 来保存下一个结点
    例如

    ```cpp
    class Doctor{
        public:
        Doctor *next;
    }
    ```

5. 所有人物类都应该继承自 User 类，否则每一个类都单独从文件中读取 "医疗信息" ，他们一旦对其中某一条信息进行修改，其他类很难获取到同步的信息，比如患者添加了一条挂号信息，但是由于类之间的独立，医生无法获取到该条信息，因此，只需要使用一个User类来保管所有的医疗信息，子类根据身份的不同来访问父类中的不同信息

6. 所有人员的登录密码都需要调用SHA256加密算法进行加密，然后保存到数据库中，下面是头文件`SHA-256.h`中的三个方法：

    ```cpp
    // 获取随机盐(可以理解成密钥)，这个是验证密码的关键，所以每一个类中都需要添加一个属性 `Salt` 来存储密钥，写入文件的时候也需要将该属性写入
    std::string generateSalt(unsigned int len = 16);

    // 密码加密，第一个参数password是明文密码，第二个参数salt是密钥，第三个参数iterations(该属性由管理员管理)是迭代次数
    // 这里的返回结果字符串格式是 "密钥$密文"，这就是存储在文件中的密码
    std::string SHA256Encrypt(const std::string &password, const std::string &salt, int iterations);


    // 密码验证，第一个参数inputPassword是输入的密码，第二个参数storedHash是保存在文件中的密码，第三个参数iterations(该属性由管理员管理)是迭代次数
    bool SHA256Verify(const std::string &inputPassword, const std::string &storedHash, int iterations);
    ```

7. 涉及到时间的属性，需要调用`GetTime.h`以保证时间存储格式的一致，这里面既有返回 "2026-4-7 10:0:0" 格式的函数`getTime()`
 也有可以获取单个时间位置的函数`getYear()`, `getMonth()`, `getDay()`, `getHour()`, `getMinute()`, `getSecond()`

8. 数据持久化存储的框架应按照如下格式：

```markdown

C课设-HIS
└── Data
    ├── RecordData
    │   ├── ConsultationChainData
    │   ├── ExaminationChainData
    │   ├── HospitalizationChainData
    │   ├── MedicineChainData
    │   └── RegistrationChainData
    └── UserData
        ├── AdminChainData
        ├── DoctorChainData
        ├── NurseChainData
        ├── PatientChainData
        └── PharmacistChainData

```

注意：上图中的名词全部都是文件夹的名称，而不是文件的名称，比如存储医生的相关信息的txt文件的路径是 `./Data/UserData/DoctorChainData/doctor_users.txt`

## 运行方式

1. 将整个项目文件夹克隆到本地
2. 使用 Visual Studio Code 打开该项目文件夹
3. 在 Visual Studio Code 中安装 C/C++ 插件
4. 在 Visual Studio Code 使用快捷键 `Ctrl + Shift + P` 来打开命令面板，输入 `Tasks: Run Task` 按回车
5. 在任务列表中选择 "`CMake: 编译` : 进入 build 目录后执行 cmake --build . --config Debug 命令，编译项目" 来编译项目
6. 然后打开终端，通过命令 `cd build` 进入到 build 目录
7. 在 build 目录下的终端中输入命令 `./Debug/his.exe` 来运行项目
8. 在运行完成之后，按照上面的操作重新打开运行任务列表，在运行任务中选择 "`CMake: build清理` : CMake模板清理任务" 来清理之前的编译文件

## 调试方式

在要调试的地方设置断点，然后使用快捷键 `F5` 来启动调试器

## 两种运行模式

本项目支持两种运行方式：**纯终端模式**（控制台程序）和 **前后端分离模式**（REST API + Vue 前端）。下面分别详细说明运行和调试方法。

---

### 模式一：纯终端模式（Console）

使用 `main.cpp` 编译链接，通过终端交互式操作。

#### 编译

```bash
# 方法1：使用 CMake 命令行
cd build && cmake .. && cmake --build . --config Debug

# 方法2：VS Code 命令面板
Ctrl+Shift+P → Tasks: Run Task → CMake: 编译
```

编译成功后，可执行文件位于 `build/Debug/his.exe`。

#### 运行

```bash
# 必须先进入 build 目录（数据文件使用 ../Data/ 相对路径）
cd build
./Debug/his.exe
```

> **重要：** 必须从 `build/` 目录启动程序，否则数据文件路径 `../Data/` 无法正确解析。

#### 调试

1. 在代码中需要调试的位置设置断点（点击行号左侧）
2. 按 `F5` 启动调试器（使用 `.vscode/launch.json` 中的配置）
3. 程序会在外部终端中启动，断点触发时可查看变量、调用堆栈等
4. 调试工具栏快捷键：
   - `F10`：单步跳过
   - `F11`：单步进入
   - `Shift+F5`：停止调试

> **注意：** `launch.json` 配置了 `postDebugTask`，调试结束后会自动清理 `build/` 目录。

#### 操作流程

程序启动后的交互流程：

```text
系统主菜单
├── 1. 登录
│   ├── 管理员登录 → 管理员功能菜单
│   ├── 医生登录 → 医生功能菜单
│   ├── 护士登录 → 护士功能菜单
│   ├── 药剂师登录 → 药剂师功能菜单
│   └── 患者登录 → 患者功能菜单
├── 2. 注册
│   ├── 管理员注册
│   ├── 医生注册
│   ├── 护士注册
│   ├── 药剂师注册
│   └── 患者注册
└── 0. 退出系统
```

首次运行时没有管理员数据，会强制要求注册管理员账号，否则系统无法启动。

---

### 模式二：前后端分离模式（REST API + Vue 前端）

使用 `server_main.cpp` 编译链接启动后端 API 服务器，同时运行 Vue 前端进行浏览器访问。

#### 后端（C++ REST API）

**编译：**

```bash
# 方法1：使用 CMake 命令行
cd build && cmake .. && cmake --build . --config Debug --target his_server

# 方法2：VS Code 命令面板
Ctrl+Shift+P → Tasks: Run Task → CMake: his_server编译
```

编译成功后，可执行文件位于 `build/Debug/his_server.exe`。

**运行：**

```bash
# 同样必须从 build/ 目录启动
cd build
./Debug/his_server.exe
```

服务器启动后会监听 `http://localhost:8080`，按 `Ctrl+C` 优雅退出并自动保存所有数据。

**后端调试：**

1. 在 `ApiServer.cpp`、`server_main.cpp` 等后端代码中设置断点
2. 按 `F5` 启动调试器（默认调试配置指向 `his.exe`，如需调试服务器需修改 `.vscode/launch.json` 中的 `program` 字段为 `${workspaceFolder}/build/Debug/his_server.exe`）
3. 可通过浏览器或 Postman 访问 `http://localhost:8080/api/...` 触发断点

**API 测试示例：**

```bash
# 测试科室列表接口
curl http://localhost:8080/api/departments

# 测试登录接口
curl -X POST http://localhost:8080/api/auth/login \
  -H "Content-Type: application/json" \
  -d '{"role": 0, "userID": "000001", "password": "your_password"}'
```

#### 前端（Vue 3 + Element Plus）

**安装依赖（仅首次需要）：**

```bash
cd frontend
npm install
```

**运行开发服务器：**

```bash
cd frontend
npm run dev
```

前端开发服务器默认运行在 `http://localhost:3000`，自动代理 `/api` 请求到后端 `localhost:8080`。

**前端调试：**

1. 浏览器打开 `http://localhost:3000`
2. 使用浏览器开发者工具（`F12`）进行调试：
   - **Console** 面板：查看 JavaScript 错误和 `console.log` 输出
   - **Network** 面板：查看 API 请求/响应详情
   - **Vue DevTools** 浏览器插件：查看组件树、Pinia 状态、路由信息
3. 在 VS Code 中打开 `.vue` 文件设置断点，通过 `launch.json` 中的浏览器调试配置附加到浏览器

**前端构建（生产环境）：**

```bash
cd frontend
npm run build
# 输出到 frontend/dist/ 目录
```

#### 完整开发流程

1. **启动后端**（终端1）：

   ```bash
   cd build
   ./Debug/his_server.exe
   ```

   等待看到 "服务器启动于 `http://localhost:8080`" 提示。

2. **启动前端**（终端2）：

   ```bash
   cd frontend
   npm run dev
   ```

   等待看到 "Local: `http://localhost:3000/`" 提示。

3. **访问应用**：浏览器打开 `http://localhost:3000`

4. **退出**：
   - 先关闭前端开发服务器（终端2 按 `Ctrl+C`）
   - 再关闭后端服务器（终端1 按 `Ctrl+C`，数据会自动保存）

#### 前后端角色映射

| 前端角色编号 | 后端角色编号 | 角色名称 |
| ------------- | ------------- | --------- |
| 1 | 0 | 管理员 |
| 2 | 1 | 医生 |
| 3 | 2 | 护士 |
| 4 | 3 | 药剂师 |
| 5 | 4 | 患者 |

前端使用 1-based 角色编号，后端 API 会自动进行转换。
