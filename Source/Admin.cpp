#include "../Head/Admin.h"
#include <fstream>

// 最小析构与空实现，保证 vtable 被生成
Admin::~Admin()
{
    clearAllStaffLists();
}

void Admin::loadFromFile( const std::string &path)
{
    // 简单占位：如需加载，请在此实现实际逻辑
    // 可以读取 userDataPath 文件并填充 adminLevel / historyBalance 等
}

void Admin::saveToFile( const std::string &path)
{
    // 简单占位：如需保存，请在此实现实际逻辑
}

void Admin::loadAllStaffLists()
{
    // 占位：实际实现应从文件加载各类 Staff 链表
}

void Admin::clearAllStaffLists()
{
    // 释放链表节点（若有分配）
    // 目前假定无动态分配或由其他函数负责释放
}