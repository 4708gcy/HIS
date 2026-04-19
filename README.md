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
```
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
