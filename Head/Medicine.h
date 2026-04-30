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
#include <vector>

enum class MedicineStatus
{
    NORMAL = 1,      // 正常可用
    LOW_STOCK = 2,   // 库存低于安全阈值
    EXPIRED = 3,     // 已过期
    DISCONTINUED = 4 // 已停用
};

enum class MedicineFlowType
{
    IN_STOCK = 1,   // 入库
    OUT_STOCK = 2   // 出库
};

struct Medicine
{
    // 唯一标识（可用条码或自定义ID）
    std::string medicineID = "#";

    // 药品名称
    std::string name = "#";

    // 规格描述（例如 "0.5g*10片"）
    std::string specification = "#";

    // 生产厂家
    std::string manufacturer = "#";

    // 进价（采购价）
    double purchasePrice = 0.0;

    // 售价（结算/零售价）
    double salePrice = 0.0;

    // 当前库存数量（整数，按单位计）
    int stock = 0;

    // 安全库存阈值（低于此值应提醒补货）
    int safetyStock = 0;

    // 生产日期（建议格式 "YYYY-MM-DD"）
    std::string productionDate = "#";

    // 有效期（建议格式 "YYYY-MM-DD"）
    std::string expiryDate = "#";

    // 所属科室标签（可为空表示通用药）
    std::string department = "#";

    // 是否为专科受限用药（需二次审批）
    bool isSpecial = false;

    // 通用名（国家药典标准名）
    std::string genericName = "#";

    // 别名/商品名列表（CSV中以 ALIAS: 子行存储）
    std::vector<std::string> aliases;

    bool isDeleted = false; // 逻辑删除标志

    // 备注（储存条件、用法等）
    std::string note = "#";

    // 当前状态
    MedicineStatus status = MedicineStatus::NORMAL;

    // 双向链表指针（供内存链表使用）
    Medicine *prev = nullptr;
    Medicine *next = nullptr;
};

struct MedicineFlow
{
    std::string flowID = "#";               // 流水号，格式 mflXXXXXX
    std::string medicineID = "#";           // 关联药品ID
    MedicineFlowType type = MedicineFlowType::IN_STOCK; // 1=入库 2=出库
    int quantity = 0;                       // 变动数量（正数）
    std::string operatorID = "#";           // 操作人ID
    std::string reason = "#";               // 原因：采购入库/退货出库/过期报损等
    std::string timestamp = "#";            // 操作时间 YYYY-MM-DD hh:mm:ss
    std::string note = "#";                 // 备注
    bool isDeleted = false;

    MedicineFlow *prev = nullptr;
    MedicineFlow *next = nullptr;
};

#endif // MEDICINE_H