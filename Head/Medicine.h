/**
 * @file Medicine.h
 * @brief 医疗管理系统中的药品信息结构体
 * @details 该头文件定义了一个 Medicine 结构体，用于存储药品的基本信息，包括药品ID、名称、规格、价格等
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef MEDICINE_H
#define MEDICINE_H

#include <string>
#include <sstream>
#include <vector>

/**
 * @file Medicine.h
 * @brief 药品信息结构体（简化版：仅一栏名称）
 * @details 适用于课设的药品库存管理：包含唯一ID、名称、规格、价格、库存、有效期、所属科室等基本字段，
 *          并提供少量内联方法用于状态更新与简单序列化/反序列化（文本行格式）。
 */

enum class MedicineStatus
{
    NORMAL,      // 正常可用
    LOW_STOCK,   // 库存低于安全阈值
    EXPIRED,     // 已过期
    DISCONTINUED // 已停用
};

struct Medicine
{
    // 唯一标识（可用条码或自定义ID）
    std::string medicineID;

    // 药品名称
    std::string name;

    // 规格描述（例如 "0.5g*10片"）
    std::string specification;

    // 进价（采购价）
    double purchasePrice = 0.0;

    // 售价（结算/零售价）
    double salePrice = 0.0;

    // 当前库存数量（整数，按单位计）
    int stock = 0;

    // 安全库存阈值（低于此值应提醒补货）
    int safetyStock = 0;

    // 有效期（建议格式 "YYYY-MM-DD"）
    std::string expiryDate;

    // 所属科室标签（可为空表示通用药）
    std::string department;

    // 是否为专科受限用药（需二次审批）
    bool isSpecial = false;

    // 备注（储存条件、用法等）
    std::string note;

    // 当前状态
    MedicineStatus status = MedicineStatus::NORMAL;

    // 双向链表指针（供内存链表使用）
    Medicine *prev = nullptr;
    Medicine *next = nullptr;

    // ---------- 内联辅助方法（便于在内存中操作） ----------

    // 更新库存（delta 可正可负），并自动刷新状态
    // 返回更新后库存值
    int updateStock(int delta)
    {
        stock += delta;
        if (stock < 0)
            stock = 0;
        refreshStatus("");
        return stock;
    }

    // 根据当前库存与给定日期刷新 status
    // 参数 currentDate 格式 "YYYY-MM-DD"，如果为空则仅基于库存判断
    void refreshStatus(const std::string &currentDate)
    {
        // 过期判断：若 expiryDate 非空且 currentDate 非空，则用字符串比较（YYYY-MM-DD 可直接字典序比较）
        if (!expiryDate.empty() && !currentDate.empty() && expiryDate < currentDate)
        {
            status = MedicineStatus::EXPIRED;
            return;
        }
        // 停用/过期优先判断后，检查库存阈值
        if (stock <= 0)
        {
            status = MedicineStatus::DISCONTINUED; // 无库存视为临时停用（可调整为其它逻辑）
        }
        else if (stock <= safetyStock)
        {
            status = MedicineStatus::LOW_STOCK;
        }
        else
        {
            status = MedicineStatus::NORMAL;
        }
    }

    // 简单文本序列化（用 '|' 分隔字段），便于保存到 txt
    // 字段顺序：medicineID|name|spec|purchase|sale|stock|safety|expiry|dept|isSpecial|note
    std::string serialize() const
    {
        std::ostringstream os;
        os << medicineID << '|' << name << '|' << specification << '|'
           << purchasePrice << '|' << salePrice << '|' << stock << '|'
           << safetyStock << '|' << expiryDate << '|' << department << '|'
           << (isSpecial ? '1' : '0') << '|' << note;
        return os.str();
    }

    // 从一行文本解析成 Medicine（若解析失败返回默认构造的 Medicine，调用方需自行校验）
    // 期望与 serialize 相同的字段顺序
    static Medicine deserialize(const std::string &line)
    {
        Medicine m;
        std::vector<std::string> parts;
        std::string tmp;
        std::istringstream is(line);
        while (std::getline(is, tmp, '|'))
            parts.push_back(tmp);

        auto safeAt = [&](size_t i) -> std::string
        { return i < parts.size() ? parts[i] : std::string(); };

        m.medicineID = safeAt(0);
        m.name = safeAt(1);
        m.specification = safeAt(2);
        if (!safeAt(3).empty())
            m.purchasePrice = std::stod(safeAt(3));
        if (!safeAt(4).empty())
            m.salePrice = std::stod(safeAt(4));
        if (!safeAt(5).empty())
            m.stock = std::stoi(safeAt(5));
        if (!safeAt(6).empty())
            m.safetyStock = std::stoi(safeAt(6));
        m.expiryDate = safeAt(7);
        m.department = safeAt(8);
        m.isSpecial = (safeAt(9) == "1");
        m.note = safeAt(10);

        // 初次反序列化后刷新状态（无需 currentDate 时只根据库存和阈值设定）
        m.refreshStatus("");
        return m;
    }
};

#endif // MEDICINE_H