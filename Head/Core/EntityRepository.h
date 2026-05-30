#ifndef ENTITY_REPOSITORY_H
#define ENTITY_REPOSITORY_H

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <algorithm>
#include "Entities/Registration.h"

template <typename T>
class EntityRepository
{
private:
    T *head = nullptr;
    int &idCounter;
    std::string idPrefix;
    int idDigits;
    std::unordered_map<std::string, T *> idMap;

    void rebuildIndex()
    {
        idMap.clear();
        T *cur = head;
        while (cur)
        {
            if (!cur->isDeleted)
                idMap[getIdField(cur)] = cur;
            cur = cur->next;
        }
    }

public:
    EntityRepository(int &counter, const std::string &prefix = "", int digits = 6)
        : idCounter(counter), idPrefix(prefix), idDigits(digits) {}

    ~EntityRepository() { clear(); }

    T *getHead() { return head; }
    // 设置链表头并重建索引
    // 警告：接管后，实体的 ID 字段不应被外部修改（否则 idMap 过期，需手动调用 reindex()）
    void setHead(T *h) { head = h; rebuildIndex(); }
    int &getCounter() { return idCounter; }

    // 根据 ID 查找（O(1)，跳过已删除）
    T *findById(const std::string &id)
    {
        auto it = idMap.find(id);
        return (it != idMap.end()) ? it->second : nullptr;
    }

    // 头插法添加节点并生成 ID
    T *add(T *node)
    {
        idCounter++;
        if constexpr (requires { node->recordID; })
        {
            if (node->recordID.empty() || node->recordID == "#")
                node->recordID = idPrefix + padId(idCounter, idDigits);
        }
        node->next = head;
        if (head) head->prev = node;
        head = node;
        idMap[getIdField(node)] = node;
        return node;
    }

    // 逻辑删除
    bool remove(const std::string &id)
    {
        T *node = findById(id);
        if (!node) return false;
        node->isDeleted = true;
        idMap.erase(id);
        return true;
    }

    // 获取所有未删除的节点
    std::vector<T *> getAll()
    {
        std::vector<T *> result;
        T *cur = head;
        while (cur)
        {
            if (!cur->isDeleted) result.push_back(cur);
            cur = cur->next;
        }
        return result;
    }

    // 同 getAll，语义化别名
    std::vector<T *> getActive() { return getAll(); }

    // 过滤查询（通用断言）
    std::vector<T *> filter(std::function<bool(const T *)> predicate)
    {
        std::vector<T *> result;
        T *cur = head;
        while (cur)
        {
            if (!cur->isDeleted && predicate(cur)) result.push_back(cur);
            cur = cur->next;
        }
        return result;
    }

    // 按科室字段筛选（要求 T 有 std::string department 成员）
    std::vector<T *> findByDepartment(const std::string &dept)
    {
        return filter([&](const T *e) { return e->department == dept; });
    }

    // 获取总节点数（含已删除）
    size_t size()
    {
        size_t n = 0;
        T *cur = head;
        while (cur) { n++; cur = cur->next; }
        return n;
    }

    // 遍历所有节点（含已删除）
    void forEach(std::function<void(T *)> fn)
    {
        T *cur = head;
        while (cur) { fn(cur); cur = cur->next; }
    }

    // 统计未删除节点数
    size_t activeCount()
    {
        size_t n = 0;
        T *cur = head;
        while (cur) { if (!cur->isDeleted) n++; cur = cur->next; }
        return n;
    }

    // 清理所有节点
    void clear()
    {
        while (head)
        {
            T *n = head->next;
            delete head;
            head = n;
        }
        idMap.clear();
    }

    // ID 字段访问器 — 默认返回 recordID，各实体类型可特化
    // 注意：返回 const 引用，禁止通过此接口修改 ID（否则 idMap 索引会过期）
    static const std::string &getIdField(T *entity) { return entity->recordID; }

    // 重建索引（外部修改链表后调用）
    void reindex() { rebuildIndex(); }

    // ID 填充辅助
    static std::string padId(int counter, int digits)
    {
        std::string s = std::to_string(counter);
        if ((int)s.size() < digits)
            s = std::string(digits - s.size(), '0') + s;
        return s;
    }
};

// ==================== 实体类型 ID 字段特化 ====================

// Patient 使用 patientID 作为 ID 字段（而非默认的 recordID）
// 注意：返回 const 引用，禁止通过此接口修改 ID（否则 idMap 索引会过期）
template<>
inline const std::string& EntityRepository<Patient>::getIdField(Patient *entity) {
    return entity->patientID;
}

// Registration 使用 registrationID 作为 ID 字段
// 注意：返回 const 引用，禁止通过此接口修改 ID（否则 idMap 索引会过期）
template<>
inline const std::string& EntityRepository<Registration>::getIdField(Registration *entity) {
    return entity->registrationID;
}

#endif // ENTITY_REPOSITORY_H
