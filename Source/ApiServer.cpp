#include "ApiServer.h"
#include "LoadData.h"
#include "SaveData.h"
#include "SHA-256.h"
#include <algorithm>

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
    bedHead = loadBedInfos(bedCount_);
}

void DataManager::saveAll()
{
    std::lock_guard<std::mutex> lock(mtx);
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
    saveBedInfos(bedHead, bedCount_);
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

            dm.saveAll();
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
            dm.saveAll();
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
        dm.saveAll();
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
            dm.saveAll();
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
        dm.saveAll();
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
            dm.saveAll();
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
        pha->isDeleted = true; dm.saveAll();
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
            dm.saveAll();
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
        pat->isDeleted = true; dm.saveAll();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

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
            dm.saveAll();
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
        reg->isDeleted = true; dm.saveAll();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

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
            dm.saveAll();
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
        con->isDeleted = true; dm.saveAll();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

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
        exa->isDeleted = true; dm.saveAll();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

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
        hos->isDeleted = true; dm.saveAll();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

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
        med->isDeleted = true; dm.saveAll();
        res.set_content(ApiResponse::success("删除成功").dump(), "application/json"); });

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
            dm.saveAll();
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
        med->isDeleted = true; dm.saveAll();
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

    svr.Delete(R"(/api/admin/beds/([^/]+))", [&](const httplib::Request &req, httplib::Response &res)
               {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 1) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        bedInfo *bed = findById(dm.getBedHead(), req.matches[1]);
        if (!bed) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        bed->isDeleted = true; dm.saveAll();
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
            dm.saveAll();
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
            dm.saveAll();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(con)).dump(), "application/json");
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
            dm.saveAll();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(exa)).dump(), "application/json");
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
            dm.saveAll();
            res.set_content(ApiResponse::success("", JsonHelper::toJson(hos)).dump(), "application/json");
        } catch (const std::exception &e) { res.set_content(ApiResponse::badRequest(e.what()).dump(), "application/json"); } });

    svr.Get("/api/nurse/examinations", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Examination *cur = dm.getExamHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

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
            dm.saveAll();
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

    svr.Get("/api/nurse/profile", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 3) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Nurse *nurse = findNurse(dm.getNurseHead(), auth.userID);
        if (!nurse) { res.set_content(ApiResponse::notFound().dump(), "application/json"); return; }
        res.set_content(ApiResponse::success("", JsonHelper::userToJson(nurse)).dump(), "application/json"); });

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
            dm.saveAll();
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
        dm.saveAll();
        res.set_content(ApiResponse::success("发药成功", JsonHelper::toJson(med)).dump(), "application/json"); });

    svr.Get("/api/pharmacist/medicines", [&](const httplib::Request &req, httplib::Response &res)
            {
        setCORS(req, res);
        auto auth = authenticateRequest(req);
        if (!auth.valid || auth.role != 4) { res.set_content(ApiResponse::forbidden().dump(), "application/json"); return; }
        json list = json::array();
        std::lock_guard<std::mutex> lock(dm.getMutex());
        Medicine *cur = dm.getMedHead();
        while (cur) { if (!cur->isDeleted) list.push_back(JsonHelper::toJson(cur)); cur = cur->next; }
        res.set_content(ApiResponse::success("", json({{"list", list}, {"total", list.size()}})).dump(), "application/json"); });

    svr.Put(R"(/api/pharmacist/medicines/(\d+)/stock)", [&](const httplib::Request &req, httplib::Response &res)
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
            dm.saveAll();
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
            dm.saveAll();
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
        dm.saveAll();
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
        dm.saveAll();
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
        dm.saveAll();
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
            dm.saveAll();
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
            dm.saveAll();
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
}
