#include "ApiServer.h"
#include "LoadData.h"
#include "SaveData.h"
#include "SHA-256.h"
#include <algorithm>
#include <fstream>
#include <map>

// 前向声明（定义在匿名 namespace 之后）
static void loadSchedules();
static void saveSchedules();

// ===== DataManager 实现 =====

DataManager &DataManager::getInstance()
{
    static DataManager instance;
    return instance;
}

DataManager::~DataManager()
{
    // 内存清理在 saveAll 之后由 main 负责
}

void DataManager::init()
{
    std::lock_guard<std::mutex> lock(mtx);
    adminHead = loadAdminData(adminIDCount_);
    if (adminHead == nullptr)
    {
        std::cerr << "[WARN] 无管理员数据，请通过控制台程序先创建管理员" << std::endl;
    }
    docHead = loadDoctorData(doctorIDCount_);
    nurseHead = loadNurseData(nurseIDCount_);
    phaHead = loadPharmacistData(pharmacistIDCount_);
    patientHead = loadPatientData(patientIDCount_);
    regHead = loadRegistrations(registrationCount_);
    conHead = loadConsultations(consultationCount_);
    examHead = loadExaminations(examinationCount_);
    hosHead = loadHospitalizations(hospitalizationCount_);
    medRecHead = loadMedicationRecords(medicationRecordCount_);
    medHead = loadMedicines(medicineCount_);
    medFlowHead = loadMedicineFlows(medicineFlowCount_);
    bedHead = loadBedInfos(bedCount_);
    loadSchedules();
}

void DataManager::saveAllUnsafe()
{
    saveAdminData(adminHead, adminIDCount_);
    saveDoctorData(docHead, doctorIDCount_);
    saveNurseData(nurseHead, nurseIDCount_);
    savePharmacistData(phaHead, pharmacistIDCount_);
    savePatientData(patientHead, patientIDCount_);
    saveRegistrations(regHead, registrationCount_);
    saveConsultations(conHead, consultationCount_);
    saveExaminations(examHead, examinationCount_);
    saveHospitalizations(hosHead, hospitalizationCount_);
    saveMedicationRecords(medRecHead, medicationRecordCount_);
    saveMedicines(medHead, medicineCount_);
    saveMedicineFlows(medFlowHead, medicineFlowCount_);
    saveBedInfos(bedHead, bedCount_);
    saveSchedules();
}

void DataManager::saveAll()
{
    std::lock_guard<std::mutex> lock(mtx);
    saveAllUnsafe();
}

// ===== 认证中间件 =====

AuthResult authenticateRequest(const httplib::Request &req)
{
    AuthResult result;
    std::string authHeader = req.get_header_value("Authorization");
    if (authHeader.empty())
        return result;

    std::string token = JWTAuth::extractBearerToken(authHeader);
    if (token.empty())
        return result;

    if (!JWTAuth::validateToken(token, JWTAuth::getSecretKey()))
        return result;

    result.valid = true;
    result.userID = JWTAuth::getUserID(token);
    result.role = JWTAuth::getRole(token);
    return result;
}

// ===== 辅助函数 =====
namespace
{
    // 从链表中按 ID 查找节点
    template <typename T>
    T *findById(T *head, const std::string &id)
    {
        T *cur = head;
        while (cur)
        {
            if (!cur->isDeleted)
            {
                std::string nid;
                if constexpr (std::is_same_v<T, Registration>)
                    nid = cur->registrationID;
                else if constexpr (std::is_same_v<T, Consultation>)
                    nid = cur->consultationID;
                else if constexpr (std::is_same_v<T, Examination>)
                    nid = cur->examinationID;
                else if constexpr (std::is_same_v<T, Hospitalization>)
                    nid = cur->hospitalizationID;
                else if constexpr (std::is_same_v<T, MedicationRecord>)
                    nid = cur->medRecordID;
                else if constexpr (std::is_same_v<T, Medicine>)
                    nid = cur->medicineID;
                else if constexpr (std::is_same_v<T, bedInfo>)
                    nid = cur->bedID;
                if (nid == id)
                    return cur;
            }
            cur = cur->next;
        }
        return nullptr;
    }

    // 通过 userID 查找用户
    Admin *findAdmin(Admin *head, const std::string &id)
    {
        Admin *cur = head;
        while (cur)
        {
            if (!cur->isDeleted && cur->getUserID() == id)
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    Doctor *findDoctor(Doctor *head, const std::string &id)
    {
        Doctor *cur = head;
        while (cur)
        {
            if (!cur->isDeleted && cur->getUserID() == id)
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    Nurse *findNurse(Nurse *head, const std::string &id)
    {
        Nurse *cur = head;
        while (cur)
        {
            if (!cur->isDeleted && cur->getUserID() == id)
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    Pharmacist *findPharmacist(Pharmacist *head, const std::string &id)
    {
        Pharmacist *cur = head;
        while (cur)
        {
            if (!cur->isDeleted && cur->getUserID() == id)
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    Patient *findPatient(Patient *head, const std::string &id)
    {
        Patient *cur = head;
        while (cur)
        {
            if (!cur->isDeleted && cur->getUserID() == id)
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    // 生成新 ID
    std::string generateID(int roleDigit, int &counter)
    {
        counter++;
        std::string num = std::to_string(counter);
        while (num.size() < 5)
            num = "0" + num;
        return std::to_string(roleDigit) + num;
    }

    // 验证密码
    bool verifyPassword(const std::string &inputPassword, const std::string &storedHash)
    {
        return SHA256Verify(inputPassword, storedHash, hashIterations);
    }

    // 设置 CORS 头
    void setCORS(const httplib::Request &, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }

}

// ===== 排班数据持久化 =====
static std::vector<json> g_schedules;
static int g_scheduleIdCounter = 0;

static void loadSchedules()
{
    std::ifstream ifs("../Data/schedules.json");
    if (!ifs.is_open()) return;
    try {
        json root = json::parse(ifs);
        g_scheduleIdCounter = root.value("counter", 0);
        if (root.contains("schedules") && root["schedules"].is_array())
            g_schedules = root["schedules"].get<std::vector<json>>();
    } catch (...) {}
}

static void saveSchedules()
{
    std::ofstream ofs("../Data/schedules.json");
    if (!ofs.is_open()) return;
    json root;
    root["counter"] = g_scheduleIdCounter;
    root["schedules"] = g_schedules;
    ofs << root.dump(2);
}

// ===== 路由注册 =====

void registerApiRoutes(httplib::Server &svr)
{
    auto &dm = DataManager::getInstance();

    // 全局 CORS 处理
    svr.Options(".*", [](const httplib::Request &, httplib::Response &res)
                { setCORS({}, res); res.status = 204; });

    // 根路径 — API 欢迎页
    svr.Get("/", [](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        json data;
        data["name"] = "HIS REST API";
        data["version"] = "1.0.0";
        data["endpoints"] = json::array({
            "GET  /api/departments       - 科室列表",
            "GET  /api/fee-standards      - 费用标准",
            "POST /api/auth/login         - 登录",
            "POST /api/auth/register      - 注册",
            "GET  /api/admin/doctors      - 医生列表(需管理员token)",
            "GET  /api/admin/registrations - 挂号列表(需管理员token)",
            "GET  /api/doctor/consultations - 看诊列表(需医生token)",
            "GET  /api/nurse/beds          - 床位列表(需护士token)",
            "GET  /api/pharmacist/medicines - 药品列表(需药剂师token)",
            "GET  /api/patient/profile     - 个人信息(需患者token)"
        });
        res.set_content(ApiResponse::success("HIS REST API 服务器运行中", data).dump(), "application/json"); });

    // ==================== 认证 API ====================

    // POST /api/auth/login
    svr.Post("/api/auth/login", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        try {
            json body = json::parse(req.body);
            std::string userID = body.value("userID", "");
            std::string password = body.value("password", "");
            int role = body.value("role", 0);

            if (userID.empty() || password.empty() || role < 1 || role > 5)
            {
                res.set_content(ApiResponse::badRequest("参数不完整或角色无效(1-5)").dump(), "application/json");
                return;
            }

            bool loginOk = false;
            std::string username;
            switch (static_cast<UserRole>(role))
            {
            case UserRole::ADMIN:
            {
                Admin *admin = findAdmin(dm.getAdminHead(), userID);
                if (admin && admin->getIsAccountActive() && verifyPassword(password, admin->getStoredHash()))
                {
                    loginOk = true;
                    username = admin->getUsername();
                }
                break;
            }
            case UserRole::DOCTOR:
            {
                Doctor *doc = findDoctor(dm.getDoctorHead(), userID);
                if (doc && doc->getIsAccountActive() && verifyPassword(password, doc->getStoredHash()))
                {
                    loginOk = true;
                    username = doc->getUsername();
                }
                break;
            }
            case UserRole::NURSE:
            {
                Nurse *nurse = findNurse(dm.getNurseHead(), userID);
                if (nurse && nurse->getIsAccountActive() && verifyPassword(password, nurse->getStoredHash()))
                {
                    loginOk = true;
                    username = nurse->getUsername();
                }
                break;
            }
            case UserRole::PHARMACIST:
            {
                Pharmacist *pha = findPharmacist(dm.getPharmacistHead(), userID);
                if (pha && pha->getIsAccountActive() && verifyPassword(password, pha->getStoredHash()))
                {
                    loginOk = true;
                    username = pha->getUsername();
                }
                break;
            }
            case UserRole::PATIENT:
            {
                Patient *pat = findPatient(dm.getPatientHead(), userID);
                if (pat && pat->getIsAccountActive() && verifyPassword(password, pat->getStoredHash()))
                {
                    loginOk = true;
                    username = pat->getUsername();
                }
                break;
            }
            }

            if (!loginOk)
            {
                res.set_content(ApiResponse::error(401, "用户ID、密码错误或账户已锁定").dump(), "application/json");
                return;
            }

            std::string token = JWTAuth::generateToken(userID, role, JWTAuth::getSecretKey());
            json data;
            data["token"] = token;
            data["userID"] = userID;
            data["username"] = username;
            data["role"] = role;
            data["roleStr"] = JsonHelper::roleToStr(role);
            res.set_content(ApiResponse::success("登录成功", data).dump(), "application/json");
        }
        catch (const std::exception &e) {
            res.set_content(ApiResponse::badRequest(std::string("请求解析失败: ") + e.what()).dump(), "application/json");
        } });

    // POST /api/auth/register
    svr.Post("/api/auth/register", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        try {
            json body = json::parse(req.body);
            int role = body.value("role", 0);
            std::string username = body.value("username", "");
            std::string password = body.value("password", "");
            std::string gender = body.value("gender", "#");
            int age = body.value("age", 0);
            std::string telephone = body.value("telephone", "#");
            std::string email = body.value("email", "#");

            if (role < 1 || role > 5 || username.empty() || password.empty())
            {
                res.set_content(ApiResponse::badRequest("角色、用户名和密码不能为空").dump(), "application/json");
                return;
            }

            std::string newID;
            std::string salt = generateSalt(16);
            std::string storedHash = SHA256Encrypt(password, salt, hashIterations);

            std::lock_guard<std::mutex> lock(dm.getMutex());

            switch (static_cast<UserRole>(role))
            {
            case UserRole::ADMIN:
            {
                Admin *newUser = new Admin();
                newID = generateID(0, dm.adminIDCount());
                newUser->setUserID(newID);
                newUser->setUsername(username);
                newUser->setSalt(salt);
                newUser->setStoredHash(storedHash);
                newUser->setIsAccountActive(true);
                newUser->setRole(UserRole::ADMIN);
                newUser->setCreateTime(MyTime::getInstance().getTime());
                newUser->setGender(gender);
                newUser->setAge(age);
                newUser->setTelephone(telephone);
                newUser->setEmail(email);
                newUser->next = dm.getAdminHead();
                dm.getAdminHead() = newUser;
                break;
            }
            case UserRole::DOCTOR:
            {
                Doctor *newUser = new Doctor();
                newID = generateID(1, dm.doctorIDCount());
                newUser->setUserID(newID);
                newUser->setDoctorID(newID);
                newUser->setUsername(username);
                newUser->setSalt(salt);
                newUser->setStoredHash(storedHash);
                newUser->setIsAccountActive(true);
                newUser->setRole(UserRole::DOCTOR);
                newUser->setCreateTime(MyTime::getInstance().getTime());
                newUser->setGender(gender);
                newUser->setAge(age);
                newUser->setTelephone(telephone);
                newUser->setEmail(email);
                newUser->setDepartment(body.value("department", "急诊科"));
                newUser->setTitle(static_cast<DoctorTitle>(body.value("title", 1)));
                newUser->setSpecialty(body.value("specialty", "#"));
                newUser->setScheduleInfo(body.value("scheduleInfo", "#"));
                newUser->next = dm.getDoctorHead();
                if (dm.getDoctorHead()) dm.getDoctorHead()->prev = newUser;
                dm.getDoctorHead() = newUser;
                break;
            }
            case UserRole::NURSE:
            {
                Nurse *newUser = new Nurse();
                newID = generateID(2, dm.nurseIDCount());
                newUser->setUserID(newID);
                newUser->setNurseID(newID);
                newUser->setUsername(username);
                newUser->setSalt(salt);
                newUser->setStoredHash(storedHash);
                newUser->setIsAccountActive(true);
                newUser->setRole(UserRole::NURSE);
                newUser->setCreateTime(MyTime::getInstance().getTime());
                newUser->setGender(gender);
                newUser->setAge(age);
                newUser->setTelephone(telephone);
                newUser->setEmail(email);
                newUser->setDepartment(body.value("department", "急诊科"));
                newUser->setLevel(static_cast<NurseLevel>(body.value("level", 1)));
                newUser->setScheduleInfo(body.value("scheduleInfo", "#"));
                newUser->next = dm.getNurseHead();
                if (dm.getNurseHead()) dm.getNurseHead()->prev = newUser;
                dm.getNurseHead() = newUser;
                break;
            }
            case UserRole::PHARMACIST:
            {
                Pharmacist *newUser = new Pharmacist();
                newID = generateID(3, dm.pharmacistIDCount());
                newUser->setUserID(newID);
                newUser->setPharmacistID(newID);
                newUser->setUsername(username);
                newUser->setSalt(salt);
                newUser->setStoredHash(storedHash);
                newUser->setIsAccountActive(true);
                newUser->setRole(UserRole::PHARMACIST);
                newUser->setCreateTime(MyTime::getInstance().getTime());
                newUser->setGender(gender);
                newUser->setAge(age);
                newUser->setTelephone(telephone);
                newUser->setEmail(email);
                newUser->setDepartment(body.value("department", "急诊科"));
                newUser->setLevel(static_cast<PharmacistLevel>(body.value("level", 1)));
                newUser->setScheduleInfo(body.value("scheduleInfo", "#"));
                newUser->next = dm.getPharmacistHead();
                if (dm.getPharmacistHead()) dm.getPharmacistHead()->prev = newUser;
                dm.getPharmacistHead() = newUser;
                break;
            }
            case UserRole::PATIENT:
            {
                Patient *newUser = new Patient();
                newID = generateID(4, dm.patientIDCount());
                newUser->setUserID(newID);
                newUser->setPatientID(newID);
                newUser->setUsername(username);
                newUser->setSalt(salt);
                newUser->setStoredHash(storedHash);
                newUser->setIsAccountActive(true);
                newUser->setRole(UserRole::PATIENT);
                newUser->setCreateTime(MyTime::getInstance().getTime());
                newUser->setGender(gender);
                newUser->setAge(age);
                newUser->setTelephone(telephone);
                newUser->setEmail(email);
                newUser->department = body.value("department", "急诊科");
                newUser->address = body.value("address", "#");
                newUser->idCardNumber = body.value("idCardNumber", "#");
                newUser->emergencyContactName = body.value("emergencyContactName", "#");
                newUser->emergencyContactPhone = body.value("emergencyContactPhone", "#");
                newUser->allergyHistory = body.value("allergyHistory", "#");
                newUser->pastMedicalHistory = body.value("pastMedicalHistory", "#");
                newUser->maritalStatus = static_cast<MaritalStatus>(body.value("maritalStatus", 1));
                newUser->next = dm.getPatientHead();
                if (dm.getPatientHead()) dm.getPatientHead()->prev = newUser;
                dm.getPatientHead() = newUser;
                break;
            }
            }

            dm.saveAllUnsafe();
            std::string token = JWTAuth::generateToken(newID, role, JWTAuth::getSecretKey());
            json data;
            data["token"] = token;
            data["userID"] = newID;
            data["username"] = username;
            data["role"] = role;
            data["roleStr"] = JsonHelper::roleToStr(role);
            res.set_content(ApiResponse::success("注册成功", data).dump(), "application/json");
        }
        catch (const std::exception &e) {
            res.set_content(ApiResponse::serverError(std::string("注册失败: ") + e.what()).dump(), "application/json");
        } });

    // PUT /api/auth/change-password - 修改密码（任意角色）
    svr.Put("/api/auth/change-password", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid) { res.set_content(ApiResponse::unauthorized().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string oldPwd = body.value("oldPassword", "");
            std::string newPwd = body.value("newPassword", "");
            if (oldPwd.empty() || newPwd.empty()) {
                res.set_content(ApiResponse::badRequest("旧密码和新密码不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            // 查找用户并验证旧密码
            User *user = nullptr;
            bool match = false;
            switch (static_cast<UserRole>(auth.role)) {
            case UserRole::ADMIN: { Admin *a = findAdmin(dm.getAdminHead(), auth.userID); if (a) { user = a; match = verifyPassword(oldPwd, a->getStoredHash()); } break; }
            case UserRole::DOCTOR: { Doctor *d = findDoctor(dm.getDoctorHead(), auth.userID); if (d) { user = d; match = verifyPassword(oldPwd, d->getStoredHash()); } break; }
            case UserRole::NURSE: { Nurse *n = findNurse(dm.getNurseHead(), auth.userID); if (n) { user = n; match = verifyPassword(oldPwd, n->getStoredHash()); } break; }
            case UserRole::PHARMACIST: { Pharmacist *p = findPharmacist(dm.getPharmacistHead(), auth.userID); if (p) { user = p; match = verifyPassword(oldPwd, p->getStoredHash()); } break; }
            case UserRole::PATIENT: { Patient *t = findPatient(dm.getPatientHead(), auth.userID); if (t) { user = t; match = verifyPassword(oldPwd, t->getStoredHash()); } break; }
            }
            if (!match) { res.set_content(ApiResponse::badRequest("旧密码不正确").dump(), "application/json"); return; }
            // 计算新密码哈希，复用结果避免重复计算
            std::string newHash = SHA256Encrypt(newPwd, user->getSalt(), hashIterations);
            if (user->getStoredHash() == newHash) {
                res.set_content(ApiResponse::badRequest("新密码不能与旧密码相同").dump(), "application/json"); return; }
            user->setStoredHash(newHash);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("密码修改成功").dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // ==================== 管理员 API ====================

    // GET /api/admin/doctors - 查看医生列表
    svr.Get("/api/admin/doctors", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { // ADMIN=1
            res.set_content(ApiResponse::forbidden().dump(), "application/json");
            return;
        }
        std::string department = req.get_param_value("department");
        std::string name = req.get_param_value("name");
        std::string titleStr = req.get_param_value("title");

        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Doctor *cur = dm.getDoctorHead();
        while (cur) {
            if (!cur->isDeleted) {
                bool match = true;
                if (!department.empty() && cur->getDepartment() != department) match = false;
                if (!name.empty() && cur->getUsername().find(name) == std::string::npos) match = false;
                if (!titleStr.empty() && std::to_string(static_cast<int>(cur->getTitle())) != titleStr) match = false;
                if (match) list.push_back(JsonHelper::userToJson(cur));
            }
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("获取医生列表成功", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // GET /api/admin/doctors/:id - 查看单个医生
    svr.Get(R"(/api/admin/doctors/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) {
            res.set_content(ApiResponse::forbidden().dump(), "application/json");
            return;
        }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Doctor *doc = findDoctor(dm.getDoctorHead(), id);
        if (!doc) {
            res.set_content(ApiResponse::notFound("医生不存在").dump(), "application/json");
            return;
        }
        res.set_content(ApiResponse::success("获取医生信息成功", JsonHelper::userToJson(doc)).dump(), "application/json"); });

    // PUT /api/admin/doctors/:id - 修改医生信息
    svr.Put(R"(/api/admin/doctors/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) {
            res.set_content(ApiResponse::forbidden().dump(), "application/json");
            return;
        }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Doctor *doc = findDoctor(dm.getDoctorHead(), id);
        if (!doc) {
            res.set_content(ApiResponse::notFound("医生不存在").dump(), "application/json");
            return;
        }
        try {
            json body = json::parse(req.body);
            if (body.contains("username")) doc->setUsername(body["username"]);
            if (body.contains("gender")) doc->setGender(body["gender"]);
            if (body.contains("age")) doc->setAge(body["age"]);
            if (body.contains("telephone")) doc->setTelephone(body["telephone"]);
            if (body.contains("email")) doc->setEmail(body["email"]);
            if (body.contains("department")) doc->setDepartment(body["department"]);
            if (body.contains("title")) doc->setTitle(static_cast<DoctorTitle>(body["title"].get<int>()));
            if (body.contains("specialty")) doc->setSpecialty(body["specialty"]);
            if (body.contains("scheduleInfo")) doc->setScheduleInfo(body["scheduleInfo"]);
            if (body.contains("isOnDuty")) doc->setIsOnDuty(body["isOnDuty"]);
            if (body.contains("consultationCount")) doc->setConsultationCount(body["consultationCount"]);
            if (body.contains("examinationCount")) doc->setExaminationCount(body["examinationCount"]);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("修改成功", JsonHelper::userToJson(doc)).dump(), "application/json");
        } catch (const std::exception &e) {
            res.set_content(ApiResponse::badRequest(std::string("参数错误: ") + e.what()).dump(), "application/json");
        } });

    // DELETE /api/admin/doctors/:id - 删除医生（逻辑删除）
    svr.Delete(R"(/api/admin/doctors/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) {
            res.set_content(ApiResponse::forbidden().dump(), "application/json");
            return;
        }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Doctor *doc = findDoctor(dm.getDoctorHead(), id);
        if (!doc) {
            res.set_content(ApiResponse::notFound("医生不存在").dump(), "application/json");
            return;
        }
        doc->isDeleted = true;
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // GET /api/admin/nurses - 护士列表
    svr.Get("/api/admin/nurses", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) {
            res.set_content(ApiResponse::forbidden().dump(), "application/json");
            return;
        }
        std::string department = req.get_param_value("department");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Nurse *cur = dm.getNurseHead();
        while (cur) {
            if (!cur->isDeleted) {
                bool match = true;
                if (!department.empty() && cur->getDepartment() != department) match = false;
                if (match) list.push_back(JsonHelper::userToJson(cur));
            }
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("获取护士列表成功", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // GET /api/admin/nurses/:id
    svr.Get(R"(/api/admin/nurses/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Nurse *nurse = findNurse(dm.getNurseHead(), req.matches[1]);
        if (!nurse) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::userToJson(nurse)).dump(), "application/json"); });

    // PUT /api/admin/nurses/:id
    svr.Put(R"(/api/admin/nurses/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Nurse *nurse = findNurse(dm.getNurseHead(), req.matches[1]);
        if (!nurse) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("username")) nurse->setUsername(body["username"]);
            if (body.contains("gender")) nurse->setGender(body["gender"]);
            if (body.contains("age")) nurse->setAge(body["age"]);
            if (body.contains("telephone")) nurse->setTelephone(body["telephone"]);
            if (body.contains("email")) nurse->setEmail(body["email"]);
            if (body.contains("department")) nurse->setDepartment(body["department"]);
            if (body.contains("level")) nurse->setLevel(static_cast<NurseLevel>(body["level"].get<int>()));
            if (body.contains("scheduleInfo")) nurse->setScheduleInfo(body["scheduleInfo"]);
            if (body.contains("isOnDuty")) nurse->setIsOnDuty(body["isOnDuty"]);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::userToJson(nurse)).dump(), "application/json");
        } catch (const std::exception &e) {
            res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json");
        } });

    // DELETE /api/admin/nurses/:id
    svr.Delete(R"(/api/admin/nurses/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Nurse *nurse = findNurse(dm.getNurseHead(), req.matches[1]);
        if (!nurse) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        nurse->isDeleted = true;
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // GET /api/admin/pharmacists - 药剂师列表
    svr.Get("/api/admin/pharmacists", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Pharmacist *cur = dm.getPharmacistHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::userToJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // GET /api/admin/pharmacists/:id
    svr.Get(R"(/api/admin/pharmacists/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Pharmacist *pha = findPharmacist(dm.getPharmacistHead(), req.matches[1]);
        if (!pha) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::userToJson(pha)).dump(), "application/json"); });

    // PUT /api/admin/pharmacists/:id
    svr.Put(R"(/api/admin/pharmacists/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Pharmacist *pha = findPharmacist(dm.getPharmacistHead(), req.matches[1]);
        if (!pha) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("username")) pha->setUsername(body["username"]);
            if (body.contains("gender")) pha->setGender(body["gender"]);
            if (body.contains("age")) pha->setAge(body["age"]);
            if (body.contains("telephone")) pha->setTelephone(body["telephone"]);
            if (body.contains("email")) pha->setEmail(body["email"]);
            if (body.contains("department")) pha->setDepartment(body["department"]);
            if (body.contains("level")) pha->setLevel(static_cast<PharmacistLevel>(body["level"].get<int>()));
            if (body.contains("isOnDuty")) pha->setIsOnDuty(body["isOnDuty"]);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::userToJson(pha)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // DELETE /api/admin/pharmacists/:id
    svr.Delete(R"(/api/admin/pharmacists/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Pharmacist *pha = findPharmacist(dm.getPharmacistHead(), req.matches[1]);
        if (!pha) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        pha->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // GET /api/admin/patients - 患者列表
    svr.Get("/api/admin/patients", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Patient *cur = dm.getPatientHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::userToJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // GET /api/admin/patients/:id
    svr.Get(R"(/api/admin/patients/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Patient *pat = findPatient(dm.getPatientHead(), req.matches[1]);
        if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::userToJson(pat)).dump(), "application/json"); });

    // PUT /api/admin/patients/:id
    svr.Put(R"(/api/admin/patients/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Patient *pat = findPatient(dm.getPatientHead(), req.matches[1]);
        if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("username")) pat->setUsername(body["username"]);
            if (body.contains("gender")) pat->setGender(body["gender"]);
            if (body.contains("age")) pat->setAge(body["age"]);
            if (body.contains("telephone")) pat->setTelephone(body["telephone"]);
            if (body.contains("email")) pat->setEmail(body["email"]);
            if (body.contains("address")) pat->address = body["address"];
            if (body.contains("emergencyContactName")) pat->emergencyContactName = body["emergencyContactName"];
            if (body.contains("emergencyContactPhone")) pat->emergencyContactPhone = body["emergencyContactPhone"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::userToJson(pat)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // DELETE /api/admin/patients/:id
    svr.Delete(R"(/api/admin/patients/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Patient *pat = findPatient(dm.getPatientHead(), req.matches[1]);
        if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        pat->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // POST /api/admin/doctors - 创建医生
    svr.Post("/api/admin/doctors", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string username = body.value("username", "");
            std::string password = body.value("password", "");
            if (username.empty() || password.empty()) { res.set_content(ApiResponse::badRequest("用户名和密码不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            std::string newID = generateID(1, dm.doctorIDCount());
            std::string salt = generateSalt(16);
            std::string storedHash = SHA256Encrypt(password, salt, hashIterations);
            Doctor *doc = new Doctor();
            doc->setUserID(newID); doc->setDoctorID(newID);
            doc->setUsername(username); doc->setSalt(salt); doc->setStoredHash(storedHash);
            doc->setIsAccountActive(true); doc->setRole(UserRole::DOCTOR);
            doc->setCreateTime(MyTime::getInstance().getTime());
            doc->setGender(body.value("gender", "#"));
            doc->setAge(body.value("age", 0));
            doc->setTelephone(body.value("telephone", "#"));
            doc->setEmail(body.value("email", "#"));
            doc->setDepartment(body.value("department", "急诊科"));
            doc->setTitle(static_cast<DoctorTitle>(body.value("title", 1)));
            doc->setSpecialty(body.value("specialty", "#"));
            doc->setScheduleInfo(body.value("scheduleInfo", "#"));
            doc->setIsOnDuty(body.value("isOnDuty", false));
            doc->next = dm.getDoctorHead();
            if (dm.getDoctorHead()) dm.getDoctorHead()->prev = doc;
            dm.getDoctorHead() = doc;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::userToJson(doc)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // POST /api/admin/nurses - 创建护士
    svr.Post("/api/admin/nurses", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string username = body.value("username", "");
            std::string password = body.value("password", "");
            if (username.empty() || password.empty()) { res.set_content(ApiResponse::badRequest("用户名和密码不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            std::string newID = generateID(2, dm.nurseIDCount());
            std::string salt = generateSalt(16);
            std::string storedHash = SHA256Encrypt(password, salt, hashIterations);
            Nurse *nurse = new Nurse();
            nurse->setUserID(newID); nurse->setNurseID(newID);
            nurse->setUsername(username); nurse->setSalt(salt); nurse->setStoredHash(storedHash);
            nurse->setIsAccountActive(true); nurse->setRole(UserRole::NURSE);
            nurse->setCreateTime(MyTime::getInstance().getTime());
            nurse->setGender(body.value("gender", "#"));
            nurse->setAge(body.value("age", 0));
            nurse->setTelephone(body.value("telephone", "#"));
            nurse->setEmail(body.value("email", "#"));
            nurse->setDepartment(body.value("department", "急诊科"));
            nurse->setLevel(static_cast<NurseLevel>(body.value("level", 1)));
            nurse->setScheduleInfo(body.value("scheduleInfo", "#"));
            nurse->setIsOnDuty(body.value("isOnDuty", false));
            nurse->next = dm.getNurseHead();
            if (dm.getNurseHead()) dm.getNurseHead()->prev = nurse;
            dm.getNurseHead() = nurse;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::userToJson(nurse)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // POST /api/admin/pharmacists - 创建药剂师
    svr.Post("/api/admin/pharmacists", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string username = body.value("username", "");
            std::string password = body.value("password", "");
            if (username.empty() || password.empty()) { res.set_content(ApiResponse::badRequest("用户名和密码不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            std::string newID = generateID(3, dm.pharmacistIDCount());
            std::string salt = generateSalt(16);
            std::string storedHash = SHA256Encrypt(password, salt, hashIterations);
            Pharmacist *ph = new Pharmacist();
            ph->setUserID(newID); ph->setPharmacistID(newID);
            ph->setUsername(username); ph->setSalt(salt); ph->setStoredHash(storedHash);
            ph->setIsAccountActive(true); ph->setRole(UserRole::PHARMACIST);
            ph->setCreateTime(MyTime::getInstance().getTime());
            ph->setGender(body.value("gender", "#"));
            ph->setAge(body.value("age", 0));
            ph->setTelephone(body.value("telephone", "#"));
            ph->setEmail(body.value("email", "#"));
            ph->setDepartment(body.value("department", "急诊科"));
            ph->setLevel(static_cast<PharmacistLevel>(body.value("level", 1)));
            ph->setScheduleInfo(body.value("scheduleInfo", "#"));
            ph->setIsOnDuty(body.value("isOnDuty", true));
            ph->next = dm.getPharmacistHead();
            if (dm.getPharmacistHead()) dm.getPharmacistHead()->prev = ph;
            dm.getPharmacistHead() = ph;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::userToJson(ph)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // POST /api/admin/patients - 创建患者
    svr.Post("/api/admin/patients", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string username = body.value("username", "");
            std::string password = body.value("password", "");
            if (username.empty() || password.empty()) { res.set_content(ApiResponse::badRequest("用户名和密码不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            std::string newID = generateID(4, dm.patientIDCount());
            std::string salt = generateSalt(16);
            std::string storedHash = SHA256Encrypt(password, salt, hashIterations);
            Patient *pat = new Patient();
            pat->setUserID(newID); pat->setPatientID(newID);
            pat->setUsername(username); pat->setSalt(salt); pat->setStoredHash(storedHash);
            pat->setIsAccountActive(true); pat->setRole(UserRole::PATIENT);
            pat->setCreateTime(MyTime::getInstance().getTime());
            pat->setGender(body.value("gender", "#"));
            pat->setAge(body.value("age", 0));
            pat->setTelephone(body.value("telephone", "#"));
            pat->setEmail(body.value("email", "#"));
            pat->department = body.value("department", "急诊科");
            pat->address = body.value("address", "#");
            pat->idCardNumber = body.value("idCardNumber", "#");
            pat->emergencyContactName = body.value("emergencyContactName", "#");
            pat->emergencyContactPhone = body.value("emergencyContactPhone", "#");
            pat->allergyHistory = body.value("allergyHistory", "#");
            pat->pastMedicalHistory = body.value("pastMedicalHistory", "#");
            pat->maritalStatus = MaritalStatus::SINGLE;
            pat->next = dm.getPatientHead();
            if (dm.getPatientHead()) dm.getPatientHead()->prev = pat;
            dm.getPatientHead() = pat;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::userToJson(pat)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // ===== 医疗记录管理 API =====

    // GET /api/admin/registrations - 挂号记录列表
    svr.Get("/api/admin/registrations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string department = req.get_param_value("department");
        std::string patientID = req.get_param_value("patientID");
        std::string doctorID = req.get_param_value("doctorID");
        std::string statusStr = req.get_param_value("status");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *cur = dm.getRegHead();
        while (cur) {
            if (!cur->isDeleted) {
                bool match = true;
                if (!department.empty() && cur->department != department) match = false;
                if (!patientID.empty() && cur->patientID != patientID) match = false;
                if (!doctorID.empty() && cur->doctorID != doctorID) match = false;
                if (!statusStr.empty() && std::to_string(static_cast<int>(cur->status)) != statusStr) match = false;
                if (match) list.push_back(JsonHelper::toJson(cur));
            }
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // GET /api/admin/registrations/:id
    svr.Get(R"(/api/admin/registrations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *reg = findById(dm.getRegHead(), req.matches[1]);
        if (!reg) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(reg)).dump(), "application/json"); });

    // PUT /api/admin/registrations/:id/status - 修改挂号状态
    svr.Put(R"(/api/admin/registrations/(\d+)/status)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *reg = findById(dm.getRegHead(), req.matches[1]);
        if (!reg) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            int newStatus = body.value("status", -1);
            if (newStatus < 0 || newStatus > 3) { res.set_content(ApiResponse::badRequest("无效状态值(0-3)").dump(), "application/json"); return; }
            reg->status = static_cast<RegistrationStatus>(newStatus);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("修改成功", JsonHelper::toJson(reg)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // DELETE /api/admin/registrations/:id
    svr.Delete(R"(/api/admin/registrations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *reg = findById(dm.getRegHead(), req.matches[1]);
        if (!reg) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        reg->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // POST /api/admin/registrations - 管理员创建挂号
    svr.Post("/api/admin/registrations", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string patientID = body.value("patientID", "");
            std::string doctorID = body.value("doctorID", "");
            std::string department = body.value("department", "急诊科");
            if (patientID.empty() || doctorID.empty()) { res.set_content(ApiResponse::badRequest("患者ID和医生ID不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            Doctor *doc = findDoctor(dm.getDoctorHead(), doctorID);
            Patient *pat = findPatient(dm.getPatientHead(), patientID);
            if (!doc) { res.set_content(ApiResponse::notFound("医生不存在").dump(), "application/json"); return; }
            if (!pat) { res.set_content(ApiResponse::notFound("患者不存在").dump(), "application/json"); return; }
            std::string newID = generateID(5, dm.registrationCount());
            Registration *newReg = new Registration();
            newReg->registrationID = newID;
            newReg->patientID = patientID;
            newReg->department = department;
            newReg->doctorID = doctorID;
            newReg->registerTime = MyTime::getInstance().getTime();
            newReg->fee = doc->calculateRegistrationFee(doc->getTitle());
            newReg->status = RegistrationStatus::BOOKED;
            newReg->next = dm.getRegHead();
            if (dm.getRegHead()) dm.getRegHead()->prev = newReg;
            dm.getRegHead() = newReg;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::toJson(newReg)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 看诊记录
    svr.Get("/api/admin/consultations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string department = req.get_param_value("department");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *cur = dm.getConHead();
        while (cur) { if (!cur->isDeleted && (department.empty() || cur->department == department)) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Get(R"(/api/admin/consultations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *con = findById(dm.getConHead(), req.matches[1]);
        if (!con) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(con)).dump(), "application/json"); });

    svr.Put(R"(/api/admin/consultations/(\d+)/status)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *con = findById(dm.getConHead(), req.matches[1]);
        if (!con) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            con->status = static_cast<ConsultationStatus>(body.value("status", 0));
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(con)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    svr.Delete(R"(/api/admin/consultations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *con = findById(dm.getConHead(), req.matches[1]);
        if (!con) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        con->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // POST /api/admin/consultations - 管理员创建看诊
    svr.Post("/api/admin/consultations", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string registrationID = body.value("registrationID", "");
            if (registrationID.empty()) { res.set_content(ApiResponse::badRequest("挂号ID不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            Registration *reg = findById(dm.getRegHead(), registrationID);
            if (!reg) { res.set_content(ApiResponse::notFound("挂号记录不存在").dump(), "application/json"); return; }
            std::string newID = generateID(6, dm.consultationCount());
            Consultation *con = new Consultation();
            con->consultationID = newID;
            con->registrationID = registrationID;
            con->patientID = reg->patientID;
            con->doctorID = reg->doctorID;
            con->department = reg->department;
            con->consultationTime = MyTime::getInstance().getTime();
            con->chiefComplaint = body.value("chiefComplaint", "#");
            con->historyOfPresentIllness = body.value("historyOfPresentIllness", "#");
            con->pastMedicalHistory = body.value("pastMedicalHistory", "#");
            con->familyHistory = body.value("familyHistory", "#");
            con->preliminaryDiagnosis = body.value("preliminaryDiagnosis", "#");
            con->status = ConsultationStatus::IN_PROGRESS;
            con->next = dm.getConHead();
            if (dm.getConHead()) dm.getConHead()->prev = con;
            dm.getConHead() = con;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::toJson(con)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 检查记录
    svr.Get("/api/admin/examinations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *cur = dm.getExamHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Get(R"(/api/admin/examinations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *exa = findById(dm.getExamHead(), req.matches[1]);
        if (!exa) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(exa)).dump(), "application/json"); });

    svr.Delete(R"(/api/admin/examinations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *exa = findById(dm.getExamHead(), req.matches[1]);
        if (!exa) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        exa->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // POST /api/admin/examinations - 管理员创建检查
    svr.Post("/api/admin/examinations", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string consultationID = body.value("consultationID", "");
            std::string itemName = body.value("itemName", "");
            if (consultationID.empty() || itemName.empty()) { res.set_content(ApiResponse::badRequest("看诊ID和检查项目不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            Consultation *con = findById(dm.getConHead(), consultationID);
            if (!con) { res.set_content(ApiResponse::notFound("看诊记录不存在").dump(), "application/json"); return; }
            Doctor *doc = findDoctor(dm.getDoctorHead(), con->doctorID);
            std::string newID = generateID(7, dm.examinationCount());
            Examination *exa = new Examination();
            exa->examinationID = newID;
            exa->consultationID = consultationID;
            exa->patientID = con->patientID;
            exa->doctorID = con->doctorID;
            exa->department = con->department;
            exa->itemName = itemName;
            exa->fee = doc ? doc->calculateExaminationFee(itemName) : 5.0;
            exa->orderTime = MyTime::getInstance().getTime();
            exa->status = ExaminationStatus::ORDERED;
            exa->next = dm.getExamHead();
            if (dm.getExamHead()) dm.getExamHead()->prev = exa;
            dm.getExamHead() = exa;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::toJson(exa)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 住院记录
    svr.Get("/api/admin/hospitalizations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *cur = dm.getHosHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Get(R"(/api/admin/hospitalizations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *hos = findById(dm.getHosHead(), req.matches[1]);
        if (!hos) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(hos)).dump(), "application/json"); });

    svr.Delete(R"(/api/admin/hospitalizations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *hos = findById(dm.getHosHead(), req.matches[1]);
        if (!hos) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        hos->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // POST /api/admin/hospitalizations - 管理员创建住院
    svr.Post("/api/admin/hospitalizations", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string consultationID = body.value("consultationID", "");
            if (consultationID.empty()) { res.set_content(ApiResponse::badRequest("看诊ID不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            Consultation *con = findById(dm.getConHead(), consultationID);
            if (!con) { res.set_content(ApiResponse::notFound("看诊记录不存在").dump(), "application/json"); return; }
            std::string wardType = body.value("wardType", "普通病房");
            std::string newID = generateID(8, dm.hospitalizationCount());
            Hospitalization *hos = new Hospitalization();
            hos->hospitalizationID = newID;
            hos->consultationID = consultationID;
            hos->patientID = con->patientID;
            hos->doctorID = con->doctorID;
            hos->department = con->department;
            hos->wardType = wardType;
            hos->applyTime = MyTime::getInstance().getTime();
            hos->deposit = body.value("deposit", 0.0);
            hos->status = HospitalizationStatus::APPLIED;
            hos->next = dm.getHosHead();
            if (dm.getHosHead()) dm.getHosHead()->prev = hos;
            dm.getHosHead() = hos;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::toJson(hos)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 用药记录
    svr.Get("/api/admin/medication-records", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *cur = dm.getMedRecHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Delete(R"(/api/admin/medication-records/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *med = findById(dm.getMedRecHead(), req.matches[1]);
        if (!med) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        med->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // POST /api/admin/medication-records - 管理员创建用药记录
    svr.Post("/api/admin/medication-records", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string consultationID = body.value("consultationID", "");
            if (consultationID.empty()) { res.set_content(ApiResponse::badRequest("看诊ID不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            Consultation *con = findById(dm.getConHead(), consultationID);
            if (!con) { res.set_content(ApiResponse::notFound("看诊记录不存在").dump(), "application/json"); return; }
            std::string newID = generateID(9, dm.medicationRecordCount());
            MedicationRecord *med = new MedicationRecord();
            med->medRecordID = newID;
            med->consultationID = consultationID;
            med->doctorID = con->doctorID;
            med->patientID = con->patientID;
            med->department = con->department;
            med->createTime = MyTime::getInstance().getTime();
            med->reviewStatus = MedicationReviewStatus::PENDING_REVIEW;
            med->status = MedicationStatus::UNPAID;
            med->next = dm.getMedRecHead();
            if (dm.getMedRecHead()) dm.getMedRecHead()->prev = med;
            dm.getMedRecHead() = med;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::toJson(med)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 药品管理
    svr.Get("/api/admin/medicines", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string department = req.get_param_value("department");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Medicine *cur = dm.getMedHead();
        while (cur) {
            if (!cur->isDeleted && (department.empty() || cur->department == department))
                list.push_back(JsonHelper::toJson(cur));
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // POST /api/admin/medicines - 添加药品
    svr.Post("/api/admin/medicines", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string name = body.value("name", "");
            if (name.empty()) { res.set_content(ApiResponse::badRequest("药品名称不能为空").dump(), "application/json"); return; }

            std::lock_guard<std::mutex> lock(dm.getMutex());
            std::string newID = generateID(8, dm.medicineCount());
            Medicine *med = new Medicine();
            med->medicineID = newID;
            med->name = name;
            med->specification = body.value("specification", "#");
            med->manufacturer = body.value("manufacturer", "#");
            med->purchasePrice = body.value("purchasePrice", 0.0);
            med->salePrice = body.value("salePrice", 0.0);
            med->stock = body.value("stock", 0);
            med->safetyStock = body.value("safetyStock", 0);
            med->productionDate = body.value("productionDate", "#");
            med->expiryDate = body.value("expiryDate", "#");
            med->department = body.value("department", "急诊科");
            med->note = body.value("note", "#");
            med->status = MedicineStatus::NORMAL;
            med->isSpecial = false;
            med->next = dm.getMedHead();
            if (dm.getMedHead()) dm.getMedHead()->prev = med;
            dm.getMedHead() = med;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::toJson(med)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    svr.Get(R"(/api/admin/medicines/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Medicine *med = findById(dm.getMedHead(), req.matches[1]);
        if (!med) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(med)).dump(), "application/json"); });

    svr.Put(R"(/api/admin/medicines/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Medicine *med = findById(dm.getMedHead(), req.matches[1]);
        if (!med) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("name")) med->name = body["name"];
            if (body.contains("specification")) med->specification = body["specification"];
            if (body.contains("manufacturer")) med->manufacturer = body["manufacturer"];
            if (body.contains("purchasePrice")) med->purchasePrice = body["purchasePrice"];
            if (body.contains("salePrice")) med->salePrice = body["salePrice"];
            if (body.contains("stock")) med->stock = body["stock"];
            if (body.contains("safetyStock")) med->safetyStock = body["safetyStock"];
            if (body.contains("status")) med->status = static_cast<MedicineStatus>(body["status"].get<int>());
            if (body.contains("note")) med->note = body["note"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(med)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    svr.Delete(R"(/api/admin/medicines/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Medicine *med = findById(dm.getMedHead(), req.matches[1]);
        if (!med) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        med->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // 床位管理
    svr.Get("/api/admin/beds", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string department = req.get_param_value("department");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        bedInfo *cur = dm.getBedHead();
        while (cur) {
            if (!cur->isDeleted && (department.empty() || cur->department == department))
                list.push_back(JsonHelper::toJson(cur));
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // POST /api/admin/beds - 添加床位
    svr.Post("/api/admin/beds", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string department = body.value("department", "");
            std::string wardType = body.value("wardType", "");
            int areaNumber = body.value("areaNumber", 0);
            int wardNumber = body.value("wardNumber", 0);
            int bedNumber = body.value("bedNumber", 0);
            if (department.empty() || wardType.empty() || areaNumber <= 0 || wardNumber <= 0 || bedNumber <= 0) {
                res.set_content(ApiResponse::badRequest("请填写完整床位信息").dump(), "application/json"); return;
            }

            // 生成床位ID
            std::string bedID = autoGenerateBedID(department, wardType, areaNumber, wardNumber, bedNumber);

            std::lock_guard<std::mutex> lock(dm.getMutex());
            // 检查床位ID是否重复
            bedInfo *cur = dm.getBedHead();
            while (cur) { if (!cur->isDeleted && cur->bedID == bedID) { res.set_content(ApiResponse::badRequest("床位ID已存在").dump(), "application/json"); return; } cur = cur->next; }

            bedInfo *bed = new bedInfo();
            bed->bedID = bedID;
            bed->department = department;
            bed->wardType = wardType;
            bed->areaNumber = areaNumber;
            bed->wardNumber = wardNumber;
            bed->bedNumber = bedNumber;
            bed->status = bedStatus::AVAILABLE;
            bed->next = dm.getBedHead();
            if (dm.getBedHead()) dm.getBedHead()->prev = bed;
            dm.getBedHead() = bed;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::toJson(bed)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    svr.Delete(R"(/api/admin/beds/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        bedInfo *bed = findById(dm.getBedHead(), req.matches[1]);
        if (!bed) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        bed->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // 账号激活/封锁
    svr.Put(R"(/api/admin/account/(\d+)/status)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string targetID = req.matches[1];
            int targetRole = body.value("role", 0);
            bool activate = body.value("active", true);
            std::lock_guard<std::mutex> lock(dm.getMutex());
            User *user = nullptr;
            switch (static_cast<UserRole>(targetRole)) {
                case UserRole::ADMIN: user = findAdmin(dm.getAdminHead(), targetID); break;
                case UserRole::DOCTOR: user = findDoctor(dm.getDoctorHead(), targetID); break;
                case UserRole::NURSE: user = findNurse(dm.getNurseHead(), targetID); break;
                case UserRole::PHARMACIST: user = findPharmacist(dm.getPharmacistHead(), targetID); break;
                case UserRole::PATIENT: user = findPatient(dm.getPatientHead(), targetID); break;
            }
            if (!user) { res.set_content(ApiResponse::notFound("用户不存在").dump(), "application/json"); return; }
            user->setIsAccountActive(activate);
            if (activate) user->setLoginAttempts(0);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success(activate ? "激活成功" : "封锁成功").dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 管理员列表
    svr.Get("/api/admin/admins", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Admin *cur = dm.getAdminHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::userToJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // PUT /api/admin/profile - 修改管理员个人信息
    svr.Put("/api/admin/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Admin *adm = findAdmin(dm.getAdminHead(), auth.userID);
        if (!adm) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("username")) adm->setUsername(body["username"]);
            if (body.contains("gender")) adm->setGender(body["gender"]);
            if (body.contains("age")) adm->setAge(body["age"]);
            if (body.contains("telephone")) adm->setTelephone(body["telephone"]);
            if (body.contains("email")) adm->setEmail(body["email"]);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::userToJson(adm)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // ==================== 医生 API ====================

    svr.Get("/api/doctor/registrations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *cur = dm.getRegHead();
        while (cur) { if (!cur->isDeleted && cur->doctorID == auth.userID) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Get("/api/doctor/consultations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *cur = dm.getConHead();
        while (cur) { if (!cur->isDeleted && cur->doctorID == auth.userID) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Get(R"(/api/doctor/consultations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *con = findById(dm.getConHead(), req.matches[1]);
        if (!con || con->doctorID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(con)).dump(), "application/json"); });

    svr.Put(R"(/api/doctor/consultations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *con = findById(dm.getConHead(), req.matches[1]);
        if (!con || con->doctorID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("chiefComplaint")) con->chiefComplaint = body["chiefComplaint"];
            if (body.contains("historyOfPresentIllness")) con->historyOfPresentIllness = body["historyOfPresentIllness"];
            if (body.contains("pastMedicalHistory")) con->pastMedicalHistory = body["pastMedicalHistory"];
            if (body.contains("familyHistory")) con->familyHistory = body["familyHistory"];
            if (body.contains("preliminaryDiagnosis")) con->preliminaryDiagnosis = body["preliminaryDiagnosis"];
            if (body.contains("status")) con->status = static_cast<ConsultationStatus>(body["status"].get<int>());
            if (body.contains("isHospitalizationRecommended")) con->isHospitalizationRecommended = body["isHospitalizationRecommended"];
            if (body.contains("note")) con->note = body["note"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(con)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // POST /api/doctor/consultations - 从挂号创建看诊记录
    svr.Post("/api/doctor/consultations", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string registrationID = body.value("registrationID", "");
            if (registrationID.empty()) { res.set_content(ApiResponse::badRequest("请提供挂号ID").dump(), "application/json"); return; }

            std::lock_guard<std::mutex> lock(dm.getMutex());
            Registration *reg = findById(dm.getRegHead(), registrationID);
            if (!reg || reg->doctorID != auth.userID) { res.set_content(ApiResponse::notFound("挂号记录不存在或不属于您").dump(), "application/json"); return; }
            if (reg->status != RegistrationStatus::PAID) { res.set_content(ApiResponse::badRequest("该挂号未支付或已完成").dump(), "application/json"); return; }

            std::string newID = generateID(6, dm.consultationCount());
            Consultation *con = new Consultation();
            con->consultationID = newID;
            con->registrationID = registrationID;
            con->patientID = reg->patientID;
            con->doctorID = auth.userID;
            con->department = reg->department;
            con->consultationTime = MyTime::getInstance().getTime();
            con->chiefComplaint = body.value("chiefComplaint", "#");
            con->historyOfPresentIllness = body.value("historyOfPresentIllness", "#");
            con->pastMedicalHistory = body.value("pastMedicalHistory", "#");
            con->familyHistory = body.value("familyHistory", "#");
            con->preliminaryDiagnosis = body.value("preliminaryDiagnosis", "#");
            if (body.contains("examinationList") && body["examinationList"].is_array()) {
                for (auto &item : body["examinationList"]) con->examinationlist.push_back(item);
            }
            con->isHospitalizationRecommended = body.value("isHospitalizationRecommended", false);
            con->note = body.value("note", "#");
            con->status = ConsultationStatus::PENDING;
            con->next = dm.getConHead();
            if (dm.getConHead()) dm.getConHead()->prev = con;
            dm.getConHead() = con;

            // 更新挂号状态为已完成
            reg->status = RegistrationStatus::FINISHED;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("看诊创建成功", JsonHelper::toJson(con)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    svr.Get("/api/doctor/examinations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *cur = dm.getExamHead();
        while (cur) { if (!cur->isDeleted && cur->doctorID == auth.userID) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Get(R"(/api/doctor/examinations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *exa = findById(dm.getExamHead(), req.matches[1]);
        if (!exa || exa->doctorID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(exa)).dump(), "application/json"); });

    svr.Put(R"(/api/doctor/examinations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *exa = findById(dm.getExamHead(), req.matches[1]);
        if (!exa || exa->doctorID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("reportSummary")) exa->reportSummary = body["reportSummary"];
            if (body.contains("status")) exa->status = static_cast<ExaminationStatus>(body["status"].get<int>());
            if (body.contains("note")) exa->note = body["note"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(exa)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // POST /api/doctor/examinations - 从看诊批量创建检查记录
    svr.Post("/api/doctor/examinations", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string consultationID = body.value("consultationID", "");
            if (consultationID.empty()) { res.set_content(ApiResponse::badRequest("请提供看诊ID").dump(), "application/json"); return; }
            if (!body.contains("items") || !body["items"].is_array() || body["items"].empty()) {
                res.set_content(ApiResponse::badRequest("请选择至少一个检查项目").dump(), "application/json"); return;
            }

            std::lock_guard<std::mutex> lock(dm.getMutex());
            Consultation *con = findById(dm.getConHead(), consultationID);
            if (!con || con->doctorID != auth.userID) { res.set_content(ApiResponse::notFound("看诊记录不存在或不属于您").dump(), "application/json"); return; }

            Doctor *doc = findDoctor(dm.getDoctorHead(), auth.userID);
            json created = json::array();
            for (auto &item : body["items"]) {
                std::string itemName = item;
                std::string exaID = generateID(7, dm.examinationCount());
                Examination *exa = new Examination();
                exa->examinationID = exaID;
                exa->consultationID = consultationID;
                exa->patientID = con->patientID;
                exa->doctorID = auth.userID;
                exa->department = con->department;
                exa->itemName = itemName;
                exa->fee = doc ? doc->calculateExaminationFee(itemName) : 5.0;
                exa->orderTime = MyTime::getInstance().getTime();
                exa->status = ExaminationStatus::ORDERED;
                exa->next = dm.getExamHead();
                if (dm.getExamHead()) dm.getExamHead()->prev = exa;
                dm.getExamHead() = exa;
                created.push_back(JsonHelper::toJson(exa));

                // 同步到看诊的检查列表
                bool found = false;
                for (auto &e : con->examinationlist) { if (e == itemName) { found = true; break; } }
                if (!found) con->examinationlist.push_back(itemName);
            }
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("检查创建成功", json({{"list", created}, {"total", created.size()}})).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 医生个人信息
    svr.Get("/api/doctor/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Doctor *doc = findDoctor(dm.getDoctorHead(), auth.userID);
        if (!doc) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::userToJson(doc)).dump(), "application/json"); });

    // PUT /api/doctor/profile - 修改医生个人信息
    svr.Put("/api/doctor/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Doctor *doc = findDoctor(dm.getDoctorHead(), auth.userID);
        if (!doc) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("username")) doc->setUsername(body["username"]);
            if (body.contains("gender")) doc->setGender(body["gender"]);
            if (body.contains("age")) doc->setAge(body["age"]);
            if (body.contains("telephone")) doc->setTelephone(body["telephone"]);
            if (body.contains("email")) doc->setEmail(body["email"]);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::userToJson(doc)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // ==================== 护士 API ====================

    svr.Get("/api/nurse/hospitalizations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *cur = dm.getHosHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Get(R"(/api/nurse/hospitalizations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *hos = findById(dm.getHosHead(), req.matches[1]);
        if (!hos) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(hos)).dump(), "application/json"); });

    svr.Put(R"(/api/nurse/hospitalizations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *hos = findById(dm.getHosHead(), req.matches[1]);
        if (!hos) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("nurseID")) hos->nurseID = body["nurseID"];
            if (body.contains("status")) hos->status = static_cast<HospitalizationStatus>(body["status"].get<int>());
            if (body.contains("bedNumber")) hos->bedNumber = body["bedNumber"];
            if (body.contains("admitTime")) hos->admitTime = body["admitTime"];
            if (body.contains("dischargeTime")) hos->dischargeTime = body["dischargeTime"];
            if (body.contains("deposit")) hos->deposit = body["deposit"];
            if (body.contains("wardType")) hos->wardType = body["wardType"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(hos)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // POST /api/nurse/hospitalizations - 护士创建住院记录
    svr.Post("/api/nurse/hospitalizations", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string consultationID = body.value("consultationID", "");
            std::string wardType = body.value("wardType", "");
            if (consultationID.empty()) { res.set_content(ApiResponse::badRequest("请提供看诊ID").dump(), "application/json"); return; }

            std::lock_guard<std::mutex> lock(dm.getMutex());
            Consultation *con = findById(dm.getConHead(), consultationID);
            if (!con || !con->isHospitalizationRecommended) { res.set_content(ApiResponse::badRequest("看诊记录不存在或未建议住院").dump(), "application/json"); return; }

            std::string newID = generateID(8, dm.hospitalizationCount());
            Hospitalization *hos = new Hospitalization();
            hos->hospitalizationID = newID;
            hos->consultationID = consultationID;
            hos->patientID = con->patientID;
            hos->doctorID = con->doctorID;
            hos->nurseID = auth.userID;
            hos->department = con->department;
            hos->wardType = wardType.empty() ? "普通病房" : wardType;
            hos->applyTime = MyTime::getInstance().getTime();
            hos->deposit = body.value("deposit", 0.0);
            hos->status = HospitalizationStatus::APPLIED;
            hos->next = dm.getHosHead();
            if (dm.getHosHead()) dm.getHosHead()->prev = hos;
            dm.getHosHead() = hos;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("住院记录创建成功", JsonHelper::toJson(hos)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // POST /api/nurse/hospitalizations/:id/assign-bed - 分配床位
    svr.Post(R"(/api/nurse/hospitalizations/(\d+)/assign-bed)", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string bedID = body.value("bedID", "");
            if (bedID.empty()) { res.set_content(ApiResponse::badRequest("请选择床位").dump(), "application/json"); return; }

            std::lock_guard<std::mutex> lock(dm.getMutex());
            Hospitalization *hos = findById(dm.getHosHead(), req.matches[1]);
            if (!hos) { res.set_content(ApiResponse::notFound("住院记录不存在").dump(), "application/json"); return; }
            if (hos->status != HospitalizationStatus::PAID && hos->status != HospitalizationStatus::APPLIED) {
                res.set_content(ApiResponse::badRequest("该住院记录状态不允许分配床位").dump(), "application/json"); return;
            }

            bedInfo *bed = findById(dm.getBedHead(), bedID);
            if (!bed || bed->status != bedStatus::AVAILABLE) { res.set_content(ApiResponse::badRequest("床位不可用").dump(), "application/json"); return; }

            bed->status = bedStatus::OCCUPIED;
            bed->patientID = hos->patientID;
            bed->nurseID = auth.userID;
            bed->useTimes++;
            hos->bedNumber = bedID;
            hos->status = HospitalizationStatus::ADMITTED;
            hos->admitTime = MyTime::getInstance().getTime();
            hos->nurseID = auth.userID;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("床位分配成功", JsonHelper::toJson(hos)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // POST /api/nurse/hospitalizations/:id/discharge - 出院办理
    svr.Post(R"(/api/nurse/hospitalizations/(\d+)/discharge)", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            std::lock_guard<std::mutex> lock(dm.getMutex());
            Hospitalization *hos = findById(dm.getHosHead(), req.matches[1]);
            if (!hos) { res.set_content(ApiResponse::notFound("住院记录不存在").dump(), "application/json"); return; }
            if (hos->status != HospitalizationStatus::ADMITTED) { res.set_content(ApiResponse::badRequest("该住院记录状态不允许出院").dump(), "application/json"); return; }

            // 计算住院费用
            double dailyRate = 50.0;
            if (hos->wardType == "隔离病房") dailyRate = 100.0;
            else if (hos->wardType == "VIP病房") dailyRate = 200.0;
            else if (hos->wardType == "ICU病房") dailyRate = 500.0;

            // 简单天数计算
            int days = 1;
            if (!hos->admitTime.empty() && hos->admitTime != "#") {
                // 从入院时间到当前的粗略天数
                std::string now = MyTime::getInstance().getTime();
                int admitDay = std::stoi(hos->admitTime.substr(8, 2));
                int nowDay = std::stoi(now.substr(8, 2));
                int admitMon = std::stoi(hos->admitTime.substr(5, 2));
                int nowMon = std::stoi(now.substr(5, 2));
                days = (nowMon - admitMon) * 30 + (nowDay - admitDay);
                if (days < 1) days = 1;
            }
            hos->totalCost = dailyRate * days;

            // 释放床位
            if (!hos->bedNumber.empty() && hos->bedNumber != "#") {
                bedInfo *bed = findById(dm.getBedHead(), hos->bedNumber);
                if (bed) { bed->status = bedStatus::ClEANING; bed->patientID = "#"; bed->nurseID = "#"; bed->daysOccupied += days; }
            }

            hos->dischargeTime = MyTime::getInstance().getTime();
            hos->status = HospitalizationStatus::DISCHARGED;

            // 扣费或退还差额
            Patient *pat = findPatient(dm.getPatientHead(), hos->patientID);
            json data;
            data["totalCost"] = hos->totalCost;
            data["deposit"] = hos->deposit;
            if (pat) {
                double diff = hos->totalCost - hos->deposit;
                if (diff > 0) { pat->balance -= diff; data["deducted"] = diff; }
                else if (diff < 0) { pat->balance += (-diff); data["refunded"] = -diff; }
                data["newBalance"] = pat->balance;
            }
            dm.saveAllUnsafe();
            data["hospitalization"] = JsonHelper::toJson(hos);
            res.set_content(ApiResponse::success("出院办理成功", data).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // DELETE /api/nurse/hospitalizations/:id
    svr.Delete(R"(/api/nurse/hospitalizations/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *hos = findById(dm.getHosHead(), req.matches[1]);
        if (!hos) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        hos->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    svr.Put(R"(/api/nurse/examinations/(\d+)/vitals)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *exa = findById(dm.getExamHead(), req.matches[1]);
        if (!exa) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            json vs = body["vitalSigns"];
            if (vs.contains("temperatureC")) exa->vitalSigns.temperatureC = vs["temperatureC"];
            if (vs.contains("systolicBP")) exa->vitalSigns.systolicBP = vs["systolicBP"];
            if (vs.contains("diastolicBP")) exa->vitalSigns.diastolicBP = vs["diastolicBP"];
            if (vs.contains("heartRate")) exa->vitalSigns.heartRate = vs["heartRate"];
            if (vs.contains("respiratoryRate")) exa->vitalSigns.respiratoryRate = vs["respiratoryRate"];
            if (vs.contains("spo2")) exa->vitalSigns.spo2 = vs["spo2"];
            if (vs.contains("height")) exa->vitalSigns.height = vs["height"];
            if (vs.contains("weight")) exa->vitalSigns.weight = vs["weight"];
            if (vs.contains("bmi")) exa->vitalSigns.bmi = vs["bmi"];
            if (vs.contains("bloodSugar")) exa->vitalSigns.bloodSugar = vs["bloodSugar"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(exa)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    svr.Get("/api/nurse/beds", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string department = req.get_param_value("department");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        bedInfo *cur = dm.getBedHead();
        while (cur) { if (!cur->isDeleted && (department.empty() || cur->department == department)) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // POST /api/nurse/beds - 护士添加床位
    svr.Post("/api/nurse/beds", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string department = body.value("department", "");
            std::string wardType = body.value("wardType", "");
            int areaNumber = body.value("areaNumber", 0);
            int wardNumber = body.value("wardNumber", 0);
            int bedNumber = body.value("bedNumber", 0);
            if (department.empty() || wardType.empty() || areaNumber <= 0 || wardNumber <= 0 || bedNumber <= 0) {
                res.set_content(ApiResponse::badRequest("请填写完整床位信息").dump(), "application/json"); return;
            }
            std::string dpt, type;
            if (department == "内科") dpt = "N"; else if (department == "外科") dpt = "W";
            else if (department == "妇产科") dpt = "F"; else if (department == "急诊科") dpt = "J";
            else if (department == "儿科") dpt = "E"; else { res.set_content(ApiResponse::badRequest("无效科室").dump(), "application/json"); return; }
            if (wardType == "普通病房") type = "P"; else if (wardType == "隔离病房") type = "G";
            else if (wardType == "VIP病房") type = "V"; else if (wardType == "ICU病房") type = "I";
            else { res.set_content(ApiResponse::badRequest("无效病房类型").dump(), "application/json"); return; }
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%s-%02d-%s-%03d-%02d", dpt.c_str(), areaNumber, type.c_str(), wardNumber, bedNumber);
            std::string bedID(buffer);
            std::lock_guard<std::mutex> lock(dm.getMutex());
            bedInfo *cur = dm.getBedHead();
            while (cur) { if (!cur->isDeleted && cur->bedID == bedID) { res.set_content(ApiResponse::badRequest("床位ID已存在").dump(), "application/json"); return; } cur = cur->next; }
            bedInfo *bed = new bedInfo();
            bed->bedID = bedID; bed->department = department; bed->wardType = wardType;
            bed->areaNumber = areaNumber; bed->wardNumber = wardNumber; bed->bedNumber = bedNumber;
            bed->status = bedStatus::AVAILABLE; bed->nurseID = auth.userID;
            bed->next = dm.getBedHead();
            if (dm.getBedHead()) dm.getBedHead()->prev = bed;
            dm.getBedHead() = bed;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::toJson(bed)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // PUT /api/nurse/beds/:id - 修改床位信息
    svr.Put(R"(/api/nurse/beds/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        bedInfo *bed = findById(dm.getBedHead(), req.matches[1]);
        if (!bed) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("status")) bed->status = static_cast<bedStatus>(body["status"].get<int>());
            if (body.contains("note")) bed->note = body["note"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(bed)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // DELETE /api/nurse/beds/:id
    svr.Delete(R"(/api/nurse/beds/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        bedInfo *bed = findById(dm.getBedHead(), req.matches[1]);
        if (!bed) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        if (bed->status == bedStatus::OCCUPIED) { res.set_content(ApiResponse::badRequest("床位被占用，无法删除").dump(), "application/json"); return; }
        bed->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    svr.Get("/api/nurse/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Nurse *nurse = findNurse(dm.getNurseHead(), auth.userID);
        if (!nurse) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::userToJson(nurse)).dump(), "application/json"); });

    // PUT /api/nurse/profile - 修改护士个人信息
    svr.Put("/api/nurse/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Nurse *nurse = findNurse(dm.getNurseHead(), auth.userID);
        if (!nurse) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("username")) nurse->setUsername(body["username"]);
            if (body.contains("gender")) nurse->setGender(body["gender"]);
            if (body.contains("age")) nurse->setAge(body["age"]);
            if (body.contains("telephone")) nurse->setTelephone(body["telephone"]);
            if (body.contains("email")) nurse->setEmail(body["email"]);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::userToJson(nurse)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // ==================== 药剂师 API ====================

    svr.Get("/api/pharmacist/medication-records", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *cur = dm.getMedRecHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Get(R"(/api/pharmacist/medication-records/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *med = findById(dm.getMedRecHead(), req.matches[1]);
        if (!med) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(med)).dump(), "application/json"); });

    // 审核用药记录
    svr.Put(R"(/api/pharmacist/medication-records/(\d+)/review)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *med = findById(dm.getMedRecHead(), req.matches[1]);
        if (!med) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            int reviewStatus = body.value("reviewStatus", -1);
            if (reviewStatus < 1 || reviewStatus > 4) { res.set_content(ApiResponse::badRequest("无效审核状态(1-4)").dump(), "application/json"); return; }
            med->reviewStatus = static_cast<MedicationReviewStatus>(reviewStatus);
            med->pharmacistID = auth.userID;
            if (reviewStatus == 2) { // REJECTED
                // 标记对应看诊记录的处方审核状态
            }
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(med)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 发药
    svr.Put(R"(/api/pharmacist/medication-records/(\d+)/dispense)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *med = findById(dm.getMedRecHead(), req.matches[1]);
        if (!med) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        if (med->reviewStatus != MedicationReviewStatus::APPROVED) {
            res.set_content(ApiResponse::badRequest("处方未审核通过，无法发药").dump(), "application/json");
            return;
        }
        if (med->status != MedicationStatus::PAID) {
            res.set_content(ApiResponse::badRequest("患者未缴费，无法发药").dump(), "application/json");
            return;
        }
        // 扣减库存
        for (auto &line : med->lines) {
            Medicine *m = findById(dm.getMedHead(), line.medicineID);
            if (m) m->stock -= line.quantity;
        }
        med->status = MedicationStatus::DISPENSED;
        med->dispenseTime = MyTime::getInstance().getTime();
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("发药成功", JsonHelper::toJson(med)).dump(), "application/json"); });

    svr.Put(R"(/api/pharmacist/medicines/([^/]+)/stock)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Medicine *med = findById(dm.getMedHead(), req.matches[1]);
        if (!med) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            int delta = body.value("delta", 0);
            med->stock += delta;
            if (med->stock < 0) med->stock = 0;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(med)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    svr.Get("/api/pharmacist/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Pharmacist *pha = findPharmacist(dm.getPharmacistHead(), auth.userID);
        if (!pha) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::userToJson(pha)).dump(), "application/json"); });

    // POST /api/pharmacist/medication-records - 从看诊创建用药记录
    svr.Post("/api/pharmacist/medication-records", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string consultationID = body.value("consultationID", "");
            if (consultationID.empty()) { res.set_content(ApiResponse::badRequest("请提供看诊ID").dump(), "application/json"); return; }

            std::lock_guard<std::mutex> lock(dm.getMutex());
            Consultation *con = findById(dm.getConHead(), consultationID);
            if (!con) { res.set_content(ApiResponse::notFound("看诊记录不存在").dump(), "application/json"); return; }
            if (con->prescriptions.empty()) { res.set_content(ApiResponse::badRequest("该看诊无处方").dump(), "application/json"); return; }

            std::string newID = generateID(9, dm.medicationRecordCount());
            MedicationRecord *med = new MedicationRecord();
            med->medRecordID = newID;
            med->consultationID = consultationID;
            med->doctorID = con->doctorID;
            med->pharmacistID = auth.userID;
            med->patientID = con->patientID;
            med->department = con->department;
            med->createTime = MyTime::getInstance().getTime();
            med->reviewStatus = MedicationReviewStatus::PENDING_REVIEW;
            med->status = MedicationStatus::UNPAID;
            double totalCost = 0.0;
            for (auto &pres : con->prescriptions) {
                MedicationLine line;
                line.medicineID = pres.medicineID;
                line.medicineName = pres.name;
                line.quantity = pres.quantity;
                // 查找药品获取单价
                Medicine *medItem = findById(dm.getMedHead(), pres.medicineID);
                line.unitPrice = medItem ? medItem->salePrice : 0.0;
                line.note = pres.dosage + " " + pres.frequency + " " + pres.duration;
                totalCost += line.unitPrice * line.quantity;
                med->lines.push_back(line);
            }
            med->totalCost = totalCost;
            med->next = dm.getMedRecHead();
            if (dm.getMedRecHead()) dm.getMedRecHead()->prev = med;
            dm.getMedRecHead() = med;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("用药记录创建成功", JsonHelper::toJson(med)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // DELETE /api/pharmacist/medication-records/:id
    svr.Delete(R"(/api/pharmacist/medication-records/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *med = findById(dm.getMedRecHead(), req.matches[1]);
        if (!med) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        med->isDeleted = true; dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // PUT /api/pharmacist/profile - 修改药剂师个人信息
    svr.Put("/api/pharmacist/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Pharmacist *pha = findPharmacist(dm.getPharmacistHead(), auth.userID);
        if (!pha) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("username")) pha->setUsername(body["username"]);
            if (body.contains("gender")) pha->setGender(body["gender"]);
            if (body.contains("age")) pha->setAge(body["age"]);
            if (body.contains("telephone")) pha->setTelephone(body["telephone"]);
            if (body.contains("email")) pha->setEmail(body["email"]);
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::userToJson(pha)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // ==================== 患者 API ====================

    svr.Get("/api/patient/registrations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *cur = dm.getRegHead();
        while (cur) { if (!cur->isDeleted && cur->patientID == auth.userID) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // 患者预约挂号
    svr.Post("/api/patient/registrations", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string doctorID = body.value("doctorID", "");
            std::string department = body.value("department", "急诊科");
            if (doctorID.empty()) { res.set_content(ApiResponse::badRequest("请选择医生").dump(), "application/json"); return; }

            std::lock_guard<std::mutex> lock(dm.getMutex());
            Doctor *doc = findDoctor(dm.getDoctorHead(), doctorID);
            if (!doc) { res.set_content(ApiResponse::notFound("医生不存在").dump(), "application/json"); return; }

            std::string newID = generateID(5, dm.registrationCount());
            Registration *newReg = new Registration();
            newReg->registrationID = newID;
            newReg->patientID = auth.userID;
            newReg->department = department;
            newReg->doctorID = doctorID;
            newReg->registerTime = MyTime::getInstance().getTime();
            newReg->fee = doc->calculateRegistrationFee(doc->getTitle());
            newReg->status = RegistrationStatus::BOOKED;
            newReg->next = dm.getRegHead();
            if (dm.getRegHead()) dm.getRegHead()->prev = newReg;
            dm.getRegHead() = newReg;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("预约成功", JsonHelper::toJson(newReg)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 患者支付挂号费
    svr.Put(R"(/api/patient/registrations/(\d+)/pay)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *reg = findById(dm.getRegHead(), req.matches[1]);
        if (!reg || reg->patientID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        Patient *pat = findPatient(dm.getPatientHead(), auth.userID);
        if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        if (pat->balance < reg->fee) { res.set_content(ApiResponse::badRequest("余额不足，请先充值").dump(), "application/json"); return; }
        pat->balance -= reg->fee;
        reg->status = RegistrationStatus::PAID;
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("支付成功", JsonHelper::toJson(reg)).dump(), "application/json"); });

    svr.Get("/api/patient/consultations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *cur = dm.getConHead();
        while (cur) { if (!cur->isDeleted && cur->patientID == auth.userID) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Get("/api/patient/examinations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *cur = dm.getExamHead();
        while (cur) { if (!cur->isDeleted && cur->patientID == auth.userID) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // 患者支付检查费
    svr.Put(R"(/api/patient/examinations/(\d+)/pay)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *exa = findById(dm.getExamHead(), req.matches[1]);
        if (!exa || exa->patientID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        Patient *pat = findPatient(dm.getPatientHead(), auth.userID);
        if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        if (pat->balance < exa->fee) { res.set_content(ApiResponse::badRequest("余额不足").dump(), "application/json"); return; }
        pat->balance -= exa->fee;
        exa->status = ExaminationStatus::PAID;
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("支付成功", JsonHelper::toJson(exa)).dump(), "application/json"); });

    svr.Get("/api/patient/medication-records", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *cur = dm.getMedRecHead();
        while (cur) { if (!cur->isDeleted && cur->patientID == auth.userID) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // 患者支付药费
    svr.Put(R"(/api/patient/medication-records/(\d+)/pay)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *med = findById(dm.getMedRecHead(), req.matches[1]);
        if (!med || med->patientID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        Patient *pat = findPatient(dm.getPatientHead(), auth.userID);
        if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        if (pat->balance < med->totalCost) { res.set_content(ApiResponse::badRequest("余额不足").dump(), "application/json"); return; }
        pat->balance -= med->totalCost;
        med->status = MedicationStatus::PAID;
        med->paymentTime = MyTime::getInstance().getTime();
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("支付成功", JsonHelper::toJson(med)).dump(), "application/json"); });

    svr.Get("/api/patient/hospitalizations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *cur = dm.getHosHead();
        while (cur) { if (!cur->isDeleted && cur->patientID == auth.userID) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // PUT /api/patient/hospitalizations/:id/pay - 住院缴费
    svr.Put(R"(/api/patient/hospitalizations/(\d+)/pay)", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *hos = findById(dm.getHosHead(), req.matches[1]);
        if (!hos || hos->patientID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        if (hos->status != HospitalizationStatus::APPLIED) { res.set_content(ApiResponse::badRequest("该住院记录状态不允许缴费").dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            double deposit = body.value("deposit", 0.0);
            if (deposit <= 0) { res.set_content(ApiResponse::badRequest("押金金额必须大于0").dump(), "application/json"); return; }
            Patient *pat = findPatient(dm.getPatientHead(), auth.userID);
            if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
            if (pat->balance < deposit) { res.set_content(ApiResponse::badRequest("余额不足，请先充值").dump(), "application/json"); return; }
            pat->balance -= deposit;
            hos->deposit = deposit;
            hos->status = HospitalizationStatus::PAID;
            dm.saveAllUnsafe();
            json data;
            data["deposit"] = deposit;
            data["balance"] = pat->balance;
            res.set_content(ApiResponse::success("缴费成功", data).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // 患者充值
    svr.Post("/api/patient/recharge", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            double amount = body.value("amount", 0.0);
            if (amount <= 0) { res.set_content(ApiResponse::badRequest("充值金额必须大于0").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            Patient *pat = findPatient(dm.getPatientHead(), auth.userID);
            if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
            pat->balance += amount;
            dm.saveAllUnsafe();
            json data;
            data["balance"] = pat->balance;
            data["amount"] = amount;
            res.set_content(ApiResponse::success("充值成功", data).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    svr.Get("/api/patient/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Patient *pat = findPatient(dm.getPatientHead(), auth.userID);
        if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::userToJson(pat)).dump(), "application/json"); });

    // PUT /api/patient/profile - 修改患者个人信息
    svr.Put("/api/patient/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Patient *pat = findPatient(dm.getPatientHead(), auth.userID);
        if (!pat) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("username")) pat->setUsername(body["username"]);
            if (body.contains("gender")) pat->setGender(body["gender"]);
            if (body.contains("age")) pat->setAge(body["age"]);
            if (body.contains("telephone")) pat->setTelephone(body["telephone"]);
            if (body.contains("email")) pat->setEmail(body["email"]);
            if (body.contains("address")) pat->address = body["address"];
            if (body.contains("emergencyContactName")) pat->emergencyContactName = body["emergencyContactName"];
            if (body.contains("emergencyContactPhone")) pat->emergencyContactPhone = body["emergencyContactPhone"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("", JsonHelper::userToJson(pat)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // ==================== 通用 API ====================

    // 获取科室列表
    svr.Get("/api/departments", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        json list = json::array();
        list.push_back("内科");
        list.push_back("外科");
        list.push_back("儿科");
        list.push_back("妇产科");
        list.push_back("急诊科");
        res.set_content(ApiResponse::success("", json({{"list", list}})).dump(), "application/json"); });

    // 获取费用标准
    svr.Get("/api/fee-standards", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        json data;
        data["registration"] = {
            {"实习医生", 10}, {"住院医师", 20}, {"主治医师", 30}, {"副主任医师", 40}, {"主任医师", 50}};
        data["hospitalization"] = {
            {"普通病房", 50}, {"隔离病房", 100}, {"VIP病房", 200}, {"ICU病房", 500}};
        data["examination"] = {
            {"体温", 5}, {"血压", 8}, {"心率", 5}, {"呼吸频率", 5}, {"血氧", 10},
            {"身高", 5}, {"体重", 5}, {"BMI", 5}, {"疼痛评分", 2}, {"腰围", 5},
            {"血糖", 20}, {"体脂率", 30}, {"尿酸", 25}, {"胆固醇", 25}};
        res.set_content(ApiResponse::success("", data).dump(), "application/json"); });

    // 获取检查项目列表（含费用）
    svr.Get("/api/examination-items", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        json items = json::array();
        // 必须与 User::calculateExaminationFee() 保持一致
        items.push_back({{"name", "体温测量"}, {"fee", 5}});
        items.push_back({{"name", "血压测量"}, {"fee", 8}});
        items.push_back({{"name", "心率测量"}, {"fee", 5}});
        items.push_back({{"name", "呼吸频率测量"}, {"fee", 5}});
        items.push_back({{"name", "脉搏血氧测量"}, {"fee", 10}});
        items.push_back({{"name", "身高测量"}, {"fee", 5}});
        items.push_back({{"name", "体重测量"}, {"fee", 5}});
        items.push_back({{"name", "BMI计算"}, {"fee", 5}});
        items.push_back({{"name", "疼痛评分"}, {"fee", 2}});
        items.push_back({{"name", "腰围测量"}, {"fee", 5}});
        items.push_back({{"name", "血糖测量"}, {"fee", 20}});
        items.push_back({{"name", "体脂测量"}, {"fee", 30}});
        items.push_back({{"name", "尿酸测定"}, {"fee", 25}});
        items.push_back({{"name", "血脂测定"}, {"fee", 25}});
        res.set_content(ApiResponse::success("", json({{"list", items}})).dump(), "application/json"); });

    // ==================== 新增端点：任意角色可访问的医生列表 ====================

    // GET /api/doctors - 任意认证用户可访问的医生列表（修复患者挂号 401 问题）
    svr.Get("/api/doctors", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid) { res.set_content(ApiResponse::unauthorized().dump(), "application/json"); return; }
        std::string department = req.get_param_value("department");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Doctor *cur = dm.getDoctorHead();
        while (cur) {
            if (!cur->isDeleted && cur->getIsOnDuty()) {
                if (department.empty() || cur->getDepartment() == department)
                    list.push_back(JsonHelper::userToJson(cur));
            }
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("获取医生列表成功", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // ==================== 新增端点：患者 ====================

    // DELETE /api/patient/registrations/:id - 患者取消挂号
    svr.Delete(R"(/api/patient/registrations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
                {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *reg = findById(dm.getRegHead(), id);
        if (!reg || reg->patientID != auth.userID) { res.set_content(ApiResponse::notFound("挂号记录不存在").dump(), "application/json"); return; }
        if (reg->status == RegistrationStatus::FINISHED || reg->status == RegistrationStatus::CANCELED) {
            res.set_content(ApiResponse::badRequest("该记录已支付/已完成或已取消，无法取消").dump(), "application/json"); return; }
        // 如果已支付，退款
        if (reg->status == RegistrationStatus::PAID) {
            Patient *pat = findPatient(dm.getPatientHead(), auth.userID);
            if (pat) pat->balance += reg->fee;
        }
        reg->status = RegistrationStatus::CANCELED;
        reg->isDeleted = true;
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("取消成功").dump(), "application/json"); });

    // GET /api/patient/consultations/:id - 查看单条看诊记录
    svr.Get(R"(/api/patient/consultations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *con = findById(dm.getConHead(), id);
        if (!con || con->patientID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(con)).dump(), "application/json"); });

    // GET /api/patient/examinations/:id - 查看单条检查记录
    svr.Get(R"(/api/patient/examinations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *exam = findById(dm.getExamHead(), id);
        if (!exam || exam->patientID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(exam)).dump(), "application/json"); });

    // GET /api/patient/medication-records/:id - 查看单条用药记录
    svr.Get(R"(/api/patient/medication-records/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicationRecord *mrd = findById(dm.getMedRecHead(), id);
        if (!mrd || mrd->patientID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(mrd)).dump(), "application/json"); });

    // GET /api/patient/hospitalizations/:id - 查看单条住院记录
    svr.Get(R"(/api/patient/hospitalizations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 5) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Hospitalization *hos = findById(dm.getHosHead(), id);
        if (!hos || hos->patientID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::toJson(hos)).dump(), "application/json"); });

    // ==================== 新增端点：医生 ====================

    // DELETE /api/doctor/registrations/:id - 医生取消挂号
    svr.Delete(R"(/api/doctor/registrations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
                {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *reg = findById(dm.getRegHead(), id);
        if (!reg || reg->doctorID != auth.userID) { res.set_content(ApiResponse::notFound("挂号记录不存在").dump(), "application/json"); return; }
        if (reg->status == RegistrationStatus::FINISHED || reg->status == RegistrationStatus::CANCELED) {
            res.set_content(ApiResponse::badRequest("该记录状态不允许取消").dump(), "application/json"); return; }
        // 退款
        if (reg->status == RegistrationStatus::PAID) {
            Patient *pat = findPatient(dm.getPatientHead(), reg->patientID);
            if (pat) pat->balance += reg->fee;
        }
        reg->status = RegistrationStatus::CANCELED;
        reg->isDeleted = true;
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("取消成功").dump(), "application/json"); });

    // DELETE /api/doctor/examinations/:id - 医生删除检查
    svr.Delete(R"(/api/doctor/examinations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
                {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *exam = findById(dm.getExamHead(), id);
        if (!exam || exam->doctorID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        if (exam->status == ExaminationStatus::PAID) {
            res.set_content(ApiResponse::badRequest("已支付的检查记录无法删除").dump(), "application/json"); return; }
        exam->isDeleted = true;
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // DELETE /api/doctor/consultations/:id - 医生取消看诊
    svr.Delete(R"(/api/doctor/consultations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
                {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 2) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *con = findById(dm.getConHead(), id);
        if (!con || con->doctorID != auth.userID) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        if (con->status == ConsultationStatus::COMPLETED) {
            res.set_content(ApiResponse::badRequest("已完成的看诊记录无法取消").dump(), "application/json"); return; }
        con->isDeleted = true;
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("取消成功").dump(), "application/json"); });

    // ==================== 新增端点：管理员 ====================

    // PUT /api/admin/beds/:id - 管理员修改床位信息
    svr.Put(R"(/api/admin/beds/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        bedInfo *bed = findById(dm.getBedHead(), id);
        if (!bed) { res.set_content(ApiResponse::notFound("床位不存在").dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("status")) bed->status = static_cast<bedStatus>(body["status"].get<int>());
            if (body.contains("note")) bed->note = body["note"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("修改成功", JsonHelper::toJson(bed)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // PUT /api/admin/registrations/:id - 管理员修改挂号
    svr.Put(R"(/api/admin/registrations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Registration *reg = findById(dm.getRegHead(), id);
        if (!reg) { res.set_content(ApiResponse::notFound("挂号记录不存在").dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("doctorID")) reg->doctorID = body["doctorID"];
            if (body.contains("patientID")) reg->patientID = body["patientID"];
            if (body.contains("status")) reg->status = static_cast<RegistrationStatus>(body["status"].get<int>());
            if (body.contains("department")) reg->department = body["department"];
            if (body.contains("note")) reg->note = body["note"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("修改成功", JsonHelper::toJson(reg)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // PUT /api/admin/consultations/:id - 管理员修改看诊（完整字段）
    svr.Put(R"(/api/admin/consultations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Consultation *con = findById(dm.getConHead(), id);
        if (!con) { res.set_content(ApiResponse::notFound("看诊记录不存在").dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("status")) con->status = static_cast<ConsultationStatus>(body["status"].get<int>());
            if (body.contains("chiefComplaint")) con->chiefComplaint = body["chiefComplaint"];
            if (body.contains("historyOfPresentIllness")) con->historyOfPresentIllness = body["historyOfPresentIllness"];
            if (body.contains("pastMedicalHistory")) con->pastMedicalHistory = body["pastMedicalHistory"];
            if (body.contains("familyHistory")) con->familyHistory = body["familyHistory"];
            if (body.contains("preliminaryDiagnosis")) con->preliminaryDiagnosis = body["preliminaryDiagnosis"];
            if (body.contains("note")) con->note = body["note"];
            if (body.contains("isHospitalizationRecommended")) con->isHospitalizationRecommended = body["isHospitalizationRecommended"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("修改成功", JsonHelper::toJson(con)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // PUT /api/admin/examinations/:id - 管理员修改检查
    svr.Put(R"(/api/admin/examinations/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *exam = findById(dm.getExamHead(), id);
        if (!exam) { res.set_content(ApiResponse::notFound("检查记录不存在").dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("status")) exam->status = static_cast<ExaminationStatus>(body["status"].get<int>());
            if (body.contains("report")) exam->reportSummary = body["report"];
            if (body.contains("note")) exam->note = body["note"];
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("修改成功", JsonHelper::toJson(exam)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // ==================== 新增端点：护士 ====================

    // GET /api/nurse/examinations - 护士查看检查列表（支持筛选）
    svr.Get("/api/nurse/examinations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string department = req.get_param_value("department");
        std::string statusStr = req.get_param_value("status");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *cur = dm.getExamHead();
        while (cur) {
            if (!cur->isDeleted) {
                bool match = true;
                if (!department.empty() && cur->department != department) match = false;
                if (!statusStr.empty() && std::to_string(static_cast<int>(cur->status)) != statusStr) match = false;
                if (match) list.push_back(JsonHelper::toJson(cur));
            }
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // ==================== 新增端点：药剂师 ====================

    // GET /api/pharmacist/medicines - 药剂师查看药品列表
    svr.Get("/api/pharmacist/medicines", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string department = req.get_param_value("department");
        std::string statusStr = req.get_param_value("status");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Medicine *cur = dm.getMedHead();
        while (cur) {
            if (!cur->isDeleted) {
                bool match = true;
                if (!department.empty() && cur->department != department) match = false;
                if (!statusStr.empty() && std::to_string(static_cast<int>(cur->status)) != statusStr) match = false;
                if (match) list.push_back(JsonHelper::toJson(cur));
            }
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // POST /api/pharmacist/medicines - 药剂师添加药品
    svr.Post("/api/pharmacist/medicines", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (!body.contains("name") || !body.contains("manufacturer")) {
                res.set_content(ApiResponse::badRequest("药品名称和厂家不能为空").dump(), "application/json"); return; }
            std::lock_guard<std::mutex> lock(dm.getMutex());
            Medicine *newMed = new Medicine;
            int count = dm.medicineCount();
            std::string medNum = std::to_string(count);
            if (medNum.length() < 6) medNum.insert(0, 6 - medNum.length(), '0');
            newMed->medicineID = "med" + medNum;
            dm.medicineCount()++;
            newMed->name = body["name"];
            newMed->manufacturer = body["manufacturer"];
            newMed->department = body.value("department", "急诊科");
            newMed->stock = body.value("stock", 0);
            newMed->safetyStock = body.value("safetyStock", 10);
            newMed->purchasePrice = body.value("purchasePrice", 0.0);
            newMed->salePrice = body.value("salePrice", 0.0);
            newMed->specification = body.value("specification", "#");
            newMed->productionDate = body.value("productionDate", "#");
            newMed->expiryDate = body.value("expiryDate", "#");
            newMed->status = newMed->stock < newMed->safetyStock ? MedicineStatus::LOW_STOCK : MedicineStatus::NORMAL;
            newMed->note = body.value("note", "#");
            newMed->isSpecial = (newMed->department != "#");
            newMed->next = dm.getMedHead();
            if (dm.getMedHead()) dm.getMedHead()->prev = newMed;
            dm.getMedHead() = newMed;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("添加成功", JsonHelper::toJson(newMed)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // PUT /api/pharmacist/medicines/:id - 药剂师修改药品
    svr.Put(R"(/api/pharmacist/medicines/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Medicine *med = findById(dm.getMedHead(), id);
        if (!med) { res.set_content(ApiResponse::notFound("药品不存在").dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (body.contains("name")) med->name = body["name"];
            if (body.contains("manufacturer")) med->manufacturer = body["manufacturer"];
            if (body.contains("specification")) med->specification = body["specification"];
            if (body.contains("purchasePrice")) med->purchasePrice = body["purchasePrice"];
            if (body.contains("salePrice")) med->salePrice = body["salePrice"];
            if (body.contains("stock")) med->stock = body["stock"];
            if (body.contains("safetyStock")) med->safetyStock = body["safetyStock"];
            if (body.contains("department")) med->department = body["department"];
            if (body.contains("note")) med->note = body["note"];
            if (body.contains("status")) med->status = static_cast<MedicineStatus>(body["status"].get<int>());
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("修改成功", JsonHelper::toJson(med)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // DELETE /api/pharmacist/medicines/:id - 药剂师删除药品
    svr.Delete(R"(/api/pharmacist/medicines/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
                {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string id = req.matches[1];
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Medicine *med = findById(dm.getMedHead(), id);
        if (!med) { res.set_content(ApiResponse::notFound("药品不存在").dump(), "application/json"); return; }
        med->isDeleted = true;
        dm.saveAllUnsafe();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

    // ==================== 排班 API ====================

    // GET /api/admin/schedules - 管理员查看排班列表（支持筛选）
    svr.Get("/api/admin/schedules", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string department = req.get_param_value("department");
        std::string doctorID = req.get_param_value("doctorID");
        std::string date = req.get_param_value("date");
        json list = json::array();
        for (const auto &s : g_schedules) {
            bool match = true;
            if (!department.empty() && s.value("department", "") != department) match = false;
            if (!doctorID.empty() && s.value("doctorID", "") != doctorID) match = false;
            if (!date.empty() && s.value("date", "") != date) match = false;
            if (match) list.push_back(s);
        }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // POST /api/admin/schedules - 管理员创建排班
    svr.Post("/api/admin/schedules", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            if (!body.contains("doctorID") || !body.contains("date") || !body.contains("timeSlot")) {
                res.set_content(ApiResponse::badRequest("缺少必填字段: doctorID, date, timeSlot").dump(), "application/json"); return;
            }
            std::string doctorID = body["doctorID"];
            std::lock_guard<std::mutex> lock(dm.getMutex());
            Doctor *doc = findDoctor(dm.getDoctorHead(), doctorID);
            if (!doc) { res.set_content(ApiResponse::notFound("医生不存在").dump(), "application/json"); return; }
            g_scheduleIdCounter++;
            json schedule;
            schedule["id"] = g_scheduleIdCounter;
            schedule["doctorID"] = doctorID;
            schedule["doctorName"] = doc->getUsername();
            schedule["department"] = body.value("department", doc->getDepartment());
            schedule["date"] = body["date"];
            schedule["timeSlot"] = body["timeSlot"];
            schedule["note"] = body.value("note", "");
            schedule["createTime"] = MyTime::getInstance().getTime();
            g_schedules.push_back(schedule);
            saveSchedules();
            res.set_content(ApiResponse::success("排班创建成功", schedule).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // PUT /api/admin/schedules/:id - 管理员修改排班
    svr.Put(R"(/api/admin/schedules/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        int id = std::stoi(req.matches[1]);
        try {
            json body = json::parse(req.body);
            std::lock_guard<std::mutex> lock(dm.getMutex());
            for (auto &s : g_schedules) {
                if (s.value("id", 0) == id) {
                    if (body.contains("doctorID")) {
                        Doctor *doc = findDoctor(dm.getDoctorHead(), body["doctorID"].get<std::string>());
                        if (!doc) { res.set_content(ApiResponse::notFound("医生不存在").dump(), "application/json"); return; }
                        s["doctorID"] = body["doctorID"];
                        s["doctorName"] = doc->getUsername();
                    }
                    if (body.contains("department")) s["department"] = body["department"];
                    if (body.contains("date")) s["date"] = body["date"];
                    if (body.contains("timeSlot")) s["timeSlot"] = body["timeSlot"];
                    if (body.contains("note")) s["note"] = body["note"];
                    saveSchedules();
                    res.set_content(ApiResponse::success("排班修改成功", s).dump(), "application/json");
                    return;
                }
            }
            res.set_content(ApiResponse::notFound("排班记录不存在").dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // DELETE /api/admin/schedules/:id - 管理员删除排班
    svr.Delete(R"(/api/admin/schedules/(\d+))", [&](const httplib::Request &req, httplib::Response &res)
                {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        int id = std::stoi(req.matches[1]);
        std::lock_guard<std::mutex> lock(dm.getMutex());
        for (auto it = g_schedules.begin(); it != g_schedules.end(); ++it) {
            if (it->value("id", 0) == id) {
                g_schedules.erase(it);
                saveSchedules();
                res.set_content(ApiResponse::success("排班删除成功").dump(), "application/json");
                return;
            }
        }
        res.set_content(ApiResponse::notFound("排班记录不存在").dump(), "application/json"); });

    // GET /api/schedules - 公开排班查询（无需登录）
    svr.Get("/api/schedules", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        std::string department = req.get_param_value("department");
        std::string date = req.get_param_value("date");
        json list = json::array();
        for (const auto &s : g_schedules) {
            bool match = true;
            if (!department.empty() && s.value("department", "") != department) match = false;
            if (!date.empty() && s.value("date", "") != date) match = false;
            if (match) list.push_back(s);
        }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // ==================== 转科 API ====================

    // POST /api/nurse/hospitalizations/:id/transfer - 转科
    svr.Post(R"(/api/nurse/hospitalizations/(\d+)/transfer)", [&](const httplib::Request &req, httplib::Response &res)
              {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        try {
            json body = json::parse(req.body);
            std::string newDepartment = body.value("newDepartment", "");
            if (newDepartment.empty()) { res.set_content(ApiResponse::badRequest("请提供新科室").dump(), "application/json"); return; }
            std::string newBedID = body.value("newBedID", "");

            std::lock_guard<std::mutex> lock(dm.getMutex());
            Hospitalization *hos = findById(dm.getHosHead(), req.matches[1]);
            if (!hos) { res.set_content(ApiResponse::notFound("住院记录不存在").dump(), "application/json"); return; }
            if (hos->status != HospitalizationStatus::ADMITTED) {
                res.set_content(ApiResponse::badRequest("该住院记录状态不允许转科").dump(), "application/json"); return;
            }

            // 释放旧床位
            if (!hos->bedNumber.empty() && hos->bedNumber != "#") {
                bedInfo *oldBed = findById(dm.getBedHead(), hos->bedNumber);
                if (oldBed) { oldBed->status = bedStatus::AVAILABLE; oldBed->patientID = "#"; oldBed->nurseID = "#"; }
            }

            // 分配新床位
            if (!newBedID.empty()) {
                bedInfo *newBed = findById(dm.getBedHead(), newBedID);
                if (!newBed || newBed->status != bedStatus::AVAILABLE) {
                    res.set_content(ApiResponse::badRequest("新床位不可用").dump(), "application/json"); return;
                }
                newBed->status = bedStatus::OCCUPIED;
                newBed->patientID = hos->patientID;
                newBed->nurseID = auth.userID;
                newBed->useTimes++;
                hos->bedNumber = newBedID;
                hos->wardType = newBed->wardType;
            } else {
                hos->bedNumber = "#";
            }

            hos->department = newDepartment;
            hos->nurseID = auth.userID;
            dm.saveAllUnsafe();
            res.set_content(ApiResponse::success("转科成功", JsonHelper::toJson(hos)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    // ==================== 管理员报表 API ====================

    // GET /api/admin/reports/department - 科室报表
    svr.Get("/api/admin/reports/department", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        // 按科室统计挂号和看诊数量
        std::map<std::string, json> deptStats;
        Registration *reg = dm.getRegHead();
        while (reg) {
            if (!reg->isDeleted) {
                std::string dept = reg->department.empty() || reg->department == "#" ? "未知" : reg->department;
                if (deptStats.find(dept) == deptStats.end())
                    deptStats[dept] = json{{"department", dept}, {"registrations", 0}, {"consultations", 0}};
                deptStats[dept]["registrations"] = deptStats[dept]["registrations"].get<int>() + 1;
            }
            reg = reg->next;
        }
        Consultation *con = dm.getConHead();
        while (con) {
            if (!con->isDeleted) {
                std::string dept = con->department.empty() || con->department == "#" ? "未知" : con->department;
                if (deptStats.find(dept) == deptStats.end())
                    deptStats[dept] = json{{"department", dept}, {"registrations", 0}, {"consultations", 0}};
                deptStats[dept]["consultations"] = deptStats[dept]["consultations"].get<int>() + 1;
            }
            con = con->next;
        }
        json list = json::array();
        for (auto &kv : deptStats) list.push_back(kv.second);
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // GET /api/admin/reports/doctor-workload - 医生工作量报表
    svr.Get("/api/admin/reports/doctor-workload", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        std::map<std::string, json> docStats;
        // 统计每个医生的看诊数量
        Consultation *con = dm.getConHead();
        while (con) {
            if (!con->isDeleted) {
                std::string docID = con->doctorID;
                if (docStats.find(docID) == docStats.end()) {
                    std::string docName = "未知";
                    Doctor *doc = findDoctor(dm.getDoctorHead(), docID);
                    if (doc) docName = doc->getUsername();
                    docStats[docID] = json{{"doctorID", docID}, {"doctorName", docName}, {"consultations", 0}, {"examinations", 0}};
                }
                docStats[docID]["consultations"] = docStats[docID]["consultations"].get<int>() + 1;
            }
            con = con->next;
        }
        // 统计每个医生的检查数量
        Examination *exa = dm.getExamHead();
        while (exa) {
            if (!exa->isDeleted) {
                std::string docID = exa->doctorID;
                if (docStats.find(docID) == docStats.end()) {
                    std::string docName = "未知";
                    Doctor *doc = findDoctor(dm.getDoctorHead(), docID);
                    if (doc) docName = doc->getUsername();
                    docStats[docID] = json{{"doctorID", docID}, {"doctorName", docName}, {"consultations", 0}, {"examinations", 0}};
                }
                docStats[docID]["examinations"] = docStats[docID]["examinations"].get<int>() + 1;
            }
            exa = exa->next;
        }
        json list = json::array();
        for (auto &kv : docStats) list.push_back(kv.second);
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // GET /api/admin/reports/patient - 患者报表
    svr.Get("/api/admin/reports/patient", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        int totalPatients = 0;
        Patient *pat = dm.getPatientHead();
        while (pat) {
            if (!pat->isDeleted) totalPatients++;
            pat = pat->next;
        }
        // 统计每个患者的就诊次数
        std::map<std::string, int> visitCounts;
        Registration *reg = dm.getRegHead();
        while (reg) {
            if (!reg->isDeleted && !reg->patientID.empty() && reg->patientID != "#")
                visitCounts[reg->patientID]++;
            reg = reg->next;
        }
        json list = json::array();
        for (auto &kv : visitCounts) {
            std::string pName = "未知";
            Patient *p = findPatient(dm.getPatientHead(), kv.first);
            if (p) pName = p->getUsername();
            list.push_back(json{{"patientID", kv.first}, {"patientName", pName}, {"visitCount", kv.second}});
        }
        json data;
        data["totalPatients"] = totalPatients;
        data["patientsWithVisits"] = list.size();
        data["list"] = list;
        res.set_content(ApiResponse::success("", data).dump(), "application/json"); });

    // GET /api/admin/reports/bed-utilization - 床位利用率报表
    svr.Get("/api/admin/reports/bed-utilization", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        std::map<std::string, json> deptBeds;
        bedInfo *bed = dm.getBedHead();
        while (bed) {
            if (!bed->isDeleted) {
                std::string dept = bed->department.empty() || bed->department == "#" ? "未知" : bed->department;
                if (deptBeds.find(dept) == deptBeds.end())
                    deptBeds[dept] = json{{"department", dept}, {"total", 0}, {"occupied", 0}, {"available", 0}, {"cleaning", 0}, {"unavailable", 0}};
                deptBeds[dept]["total"] = deptBeds[dept]["total"].get<int>() + 1;
                if (bed->status == bedStatus::OCCUPIED) deptBeds[dept]["occupied"] = deptBeds[dept]["occupied"].get<int>() + 1;
                else if (bed->status == bedStatus::AVAILABLE) deptBeds[dept]["available"] = deptBeds[dept]["available"].get<int>() + 1;
                else if (bed->status == bedStatus::ClEANING) deptBeds[dept]["cleaning"] = deptBeds[dept]["cleaning"].get<int>() + 1;
                else if (bed->status == bedStatus::UNAVAILABLE) deptBeds[dept]["unavailable"] = deptBeds[dept]["unavailable"].get<int>() + 1;
            }
            bed = bed->next;
        }
        json list = json::array();
        for (auto &kv : deptBeds) list.push_back(kv.second);
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // GET /api/admin/reports/medicine-inventory - 药品库存报表
    svr.Get("/api/admin/reports/medicine-inventory", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        json list = json::array();
        int totalMedicines = 0, lowStockCount = 0, expiredCount = 0, normalCount = 0;
        Medicine *med = dm.getMedHead();
        while (med) {
            if (!med->isDeleted) {
                totalMedicines++;
                if (med->status == MedicineStatus::LOW_STOCK) lowStockCount++;
                else if (med->status == MedicineStatus::EXPIRED) expiredCount++;
                else if (med->status == MedicineStatus::NORMAL) normalCount++;
                json item;
                item["medicineID"] = med->medicineID;
                item["name"] = med->name;
                item["stock"] = med->stock;
                item["safetyStock"] = med->safetyStock;
                item["status"] = static_cast<int>(med->status);
                item["statusStr"] = JsonHelper::medicineStatusToStr(static_cast<int>(med->status));
                item["department"] = med->department;
                list.push_back(item);
            }
            med = med->next;
        }
        json data;
        data["totalMedicines"] = totalMedicines;
        data["normalCount"] = normalCount;
        data["lowStockCount"] = lowStockCount;
        data["expiredCount"] = expiredCount;
        data["list"] = list;
        res.set_content(ApiResponse::success("", data).dump(), "application/json"); });

    // GET /api/admin/reports/overview - 总览仪表盘
    svr.Get("/api/admin/reports/overview", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        json data;
        // 统计各实体数量
        int cnt = 0;
        Doctor *doc = dm.getDoctorHead(); while (doc) { if (!doc->isDeleted) cnt++; doc = doc->next; }
        data["doctors"] = cnt;
        cnt = 0;
        Nurse *nur = dm.getNurseHead(); while (nur) { if (!nur->isDeleted) cnt++; nur = nur->next; }
        data["nurses"] = cnt;
        cnt = 0;
        Pharmacist *pha = dm.getPharmacistHead(); while (pha) { if (!pha->isDeleted) cnt++; pha = pha->next; }
        data["pharmacists"] = cnt;
        cnt = 0;
        Patient *pat = dm.getPatientHead(); while (pat) { if (!pat->isDeleted) cnt++; pat = pat->next; }
        data["patients"] = cnt;
        cnt = 0;
        Registration *reg = dm.getRegHead(); while (reg) { if (!reg->isDeleted) cnt++; reg = reg->next; }
        data["registrations"] = cnt;
        cnt = 0;
        Consultation *con = dm.getConHead(); while (con) { if (!con->isDeleted) cnt++; con = con->next; }
        data["consultations"] = cnt;
        cnt = 0;
        Examination *exa = dm.getExamHead(); while (exa) { if (!exa->isDeleted) cnt++; exa = exa->next; }
        data["examinations"] = cnt;
        cnt = 0;
        Hospitalization *hos = dm.getHosHead(); while (hos) { if (!hos->isDeleted) cnt++; hos = hos->next; }
        data["hospitalizations"] = cnt;
        cnt = 0;
        MedicationRecord *mr = dm.getMedRecHead(); while (mr) { if (!mr->isDeleted) cnt++; mr = mr->next; }
        data["medicationRecords"] = cnt;
        cnt = 0;
        Medicine *med = dm.getMedHead(); while (med) { if (!med->isDeleted) cnt++; med = med->next; }
        data["medicines"] = cnt;
        cnt = 0;
        bedInfo *bed = dm.getBedHead(); while (bed) { if (!bed->isDeleted) cnt++; bed = bed->next; }
        data["beds"] = cnt;
        res.set_content(ApiResponse::success("", data).dump(), "application/json"); });

    // ==================== 药品流水 API ====================

    // 辅助：MedicineFlow 序列化（JsonHelper 中没有该函数）
    auto medFlowToJson = [](const MedicineFlow *flow) -> json {
        json j;
        j["flowID"] = (flow->flowID == "#") ? "" : flow->flowID;
        j["medicineID"] = (flow->medicineID == "#") ? "" : flow->medicineID;
        j["type"] = static_cast<int>(flow->type);
        j["typeStr"] = (flow->type == MedicineFlowType::IN_STOCK) ? "入库" : "出库";
        j["quantity"] = flow->quantity;
        j["operatorID"] = (flow->operatorID == "#") ? "" : flow->operatorID;
        j["reason"] = (flow->reason == "#") ? "" : flow->reason;
        j["timestamp"] = (flow->timestamp == "#") ? "" : flow->timestamp;
        j["note"] = (flow->note == "#") ? "" : flow->note;
        j["isDeleted"] = flow->isDeleted;
        return j;
    };

    // GET /api/admin/medicine-flows - 管理员查看药品流水（支持筛选）
    svr.Get("/api/admin/medicine-flows", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string medicineID = req.get_param_value("medicineID");
        std::string typeStr = req.get_param_value("type");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicineFlow *cur = dm.getMedFlowHead();
        while (cur) {
            if (!cur->isDeleted) {
                bool match = true;
                if (!medicineID.empty() && cur->medicineID != medicineID) match = false;
                if (!typeStr.empty() && std::to_string(static_cast<int>(cur->type)) != typeStr) match = false;
                if (match) list.push_back(medFlowToJson(cur));
            }
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    // GET /api/pharmacist/medicine-flows - 药剂师查看药品流水
    svr.Get("/api/pharmacist/medicine-flows", [&](const httplib::Request &req, httplib::Response &res)
             {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::string medicineID = req.get_param_value("medicineID");
        std::string typeStr = req.get_param_value("type");
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        MedicineFlow *cur = dm.getMedFlowHead();
        while (cur) {
            if (!cur->isDeleted) {
                bool match = true;
                if (!medicineID.empty() && cur->medicineID != medicineID) match = false;
                if (!typeStr.empty() && std::to_string(static_cast<int>(cur->type)) != typeStr) match = false;
                if (match) list.push_back(medFlowToJson(cur));
            }
            cur = cur->next;
        }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });
}
