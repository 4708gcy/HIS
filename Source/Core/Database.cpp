#include "Core/Database.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------
static Database g_dbInstance;
Database &GetDB() { return g_dbInstance; }

// ---------------------------------------------------------------------------
// Config loader
// ---------------------------------------------------------------------------
void Database::loadConfig(const std::string &configPath)
{
    std::ifstream file(configPath);
    if (!file.is_open())
    {
        std::cerr << "[DB] Cannot open config: " << configPath << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line))
    {
        // trim
        auto pos = line.find_first_not_of(" \t\r\n");
        if (pos == std::string::npos) continue;
        line = line.substr(pos);
        if (line.empty() || line[0] == '#') continue;

        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        // trim key/value
        while (!key.empty() && (key.back() == ' ' || key.back() == '\t')) key.pop_back();
        while (!val.empty() && (val.back() == ' ' || val.back() == '\t' || val.back() == '\r' || val.back() == '\n')) val.pop_back();

        if (key == "host") host = val;
        else if (key == "port") port = std::stoi(val);
        else if (key == "user") user = val;
        else if (key == "password") password = val;
        else if (key == "database") dbName = val;
    }
}

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------
Database::Database()
{
    loadConfig("../Data/DatabaseConfig.txt");
    conn = mysql_init(nullptr);
    if (conn)
    {
        mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    }
}

Database::~Database()
{
    disconnect();
}

// ---------------------------------------------------------------------------
// Connection management
// ---------------------------------------------------------------------------
bool Database::connect()
{
    if (connected) return true;
    if (!conn)
    {
        conn = mysql_init(nullptr);
        if (!conn) return false;
        mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    }
    MYSQL *result = mysql_real_connect(
        conn, host.c_str(), user.c_str(), password.c_str(),
        dbName.c_str(), port, nullptr, 0);
    if (!result)
    {
        std::cerr << "[DB] Connect failed: " << mysql_error(conn) << std::endl;
        return false;
    }
    connected = true;
    // Ensure UTF-8
    mysql_query(conn, "SET NAMES utf8mb4");
    return true;
}

void Database::disconnect()
{
    if (conn)
    {
        mysql_close(conn);
        conn = nullptr;
    }
    connected = false;
}

// ---------------------------------------------------------------------------
// Query / Execute
// ---------------------------------------------------------------------------
MYSQL_RES *Database::query(const std::string &sql)
{
    if (!connected) return nullptr;
    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cerr << "[DB] Query error: " << mysql_error(conn) << std::endl;
        std::cerr << "[DB] SQL: " << sql.substr(0, 200) << std::endl;
        return nullptr;
    }
    return mysql_store_result(conn);
}

int Database::execute(const std::string &sql)
{
    if (!connected) return -1;
    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cerr << "[DB] Execute error: " << mysql_error(conn) << std::endl;
        return -1;
    }
    return static_cast<int>(mysql_affected_rows(conn));
}

// ---------------------------------------------------------------------------
// Parameterized helpers — simple ? replacement with escaping
// ---------------------------------------------------------------------------
static std::string buildSql(Database &db, const std::string &sql,
                             const std::vector<std::string> &params,
                             const std::vector<bool> &nulls = {})
{
    std::string result;
    result.reserve(sql.size() + params.size() * 32);
    size_t paramIdx = 0;
    for (size_t i = 0; i < sql.size(); ++i)
    {
        if (sql[i] == '?' && paramIdx < params.size())
        {
            const std::string &p = params[paramIdx];
            // SOH 前缀（\x01）表示该值为原始 SQL 片段（不转义、不加引号）
            // 用于传递 SQL NULL 关键字等特殊值
            if (!p.empty() && p[0] == '\x01')
            {
                result += p.substr(1);  // 去掉前缀，直接拼接 SQL 片段
            }
            else
            {
                bool isNull = (paramIdx < nulls.size()) ? nulls[paramIdx] : false;
                if (isNull)
                {
                    result += "NULL";
                }
                else
                {
                    result += "'";
                    result += Database::escapeString(p);
                    result += "'";
                }
            }
            ++paramIdx;
        }
        else
        {
            result += sql[i];
        }
    }
    return result;
}

MYSQL_RES *Database::queryPrepared(const std::string &sql,
                                    const std::vector<std::string> &params,
                                    const std::vector<bool> &nulls)
{
    return query(buildSql(*this, sql, params, nulls));
}

int Database::executePrepared(const std::string &sql,
                               const std::vector<std::string> &params,
                               const std::vector<bool> &nulls)
{
    return execute(buildSql(*this, sql, params, nulls));
}

MYSQL_RES *Database::queryPrepared(const std::string &sql,
                                    const std::vector<std::string> &params)
{
    return queryPrepared(sql, params, {});
}

int Database::executePrepared(const std::string &sql,
                               const std::vector<std::string> &params)
{
    return executePrepared(sql, params, {});
}

// ---------------------------------------------------------------------------
// Transactions
// ---------------------------------------------------------------------------
void Database::beginTransaction() { execute("START TRANSACTION"); }
void Database::commit() { execute("COMMIT"); }
void Database::rollback() { execute("ROLLBACK"); }

// ---------------------------------------------------------------------------
// String escaping
// ---------------------------------------------------------------------------
std::string Database::escapeString(const std::string &input)
{
    // Need a connection for mysql_real_escape_string, but for static use
    // we do a simple manual escape if no connection available
    // This is used in static context where we may not have a connection,
    // so we do basic escaping
    std::string result;
    result.reserve(input.size() * 2);
    for (char c : input)
    {
        switch (c)
        {
        case '\'': result += "''"; break;
        case '\\': result += "\\\\"; break;
        case '\0': result += "\\0"; break;
        case '\n': result += "\\n"; break;
        case '\r': result += "\\r"; break;
        case '\x1a': result += "\\Z"; break;
        default: result += c; break;
        }
    }
    return result;
}

// ---------------------------------------------------------------------------
// Row reading helpers
// ---------------------------------------------------------------------------
std::string Database::readString(MYSQL_ROW row, int col)
{
    if (!row || !row[col]) return "";
    return std::string(row[col]);
}

int Database::readInt(MYSQL_ROW row, int col)
{
    if (!row || !row[col]) return 0;
    try { return std::stoi(row[col]); }
    catch (...) { return 0; }
}

double Database::readDouble(MYSQL_ROW row, int col)
{
    if (!row || !row[col]) return 0.0;
    try { return std::stod(row[col]); }
    catch (...) { return 0.0; }
}

bool Database::readBool(MYSQL_ROW row, int col)
{
    if (!row || !row[col]) return false;
    return row[col][0] == '1';
}

long long Database::lastInsertId()
{
    if (!conn) return 0;
    return mysql_insert_id(conn);
}
