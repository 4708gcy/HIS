<h1 style="text-align:center"> HIS 开发日志 </h1>

---

### 2026.4.8

* 创建了3个医疗信息类
  * Registration    -挂号信息类
  * Consultation    -看诊信息类
  * Examination     -检查信息类
* 创建了用户交互界面的头文件 `UI.h`, 并完成了 5 个界面的初步设计
  * int beginUI();                          - 启动界面，显示欢迎信息和主菜单
  * int adminMenu();                        - 管理员菜单，提供账户管理和医疗记录管理选项
  * std::string adminDepartmentMenu();      - 管理员科室管理菜单
  * int adminMedicalRecordMenu();           - 管理员医疗记录管理菜单
  * int adminRegistrationManagementMenu();  - 管理员挂号记录管理菜单
* 完成了管理员的 `注册账号`函数
* 完成了管理员的 `登录账号`函数
* 完成了管理员的 `解锁/封禁账号`函数(使用泛函编程的方式实现)
* 完成了管理员的 `挂号信息管理`中的 3 个功能
  * 查看某一个科室的所有挂号信息
  * 修改某一个科室的挂号信息的状态(已预约/已支付/已取消/已完成)
  * 删除某一个科室中的某一条挂号信息
