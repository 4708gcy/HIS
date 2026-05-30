#ifndef HIS_DATABASE_H
#define HIS_DATABASE_H

#include <mysql.h>
#include <string>
#include <vector>
#include <memory>

class Database
{
private:
    MYSQL *conn = nullptr;
    std::string host, user, password, dbName;
    int port = 3306;
    bool connected = false;

    void loadConfig(const std::string &configPath);

public:
    Database();
    ~Database();

    bool connect();
    void disconnect();
    bool isConnected() const { return connected; }
    MYSQL *getRaw() { return conn; }

    // SELECT — caller must mysql_free_result()
    MYSQL_RES *query(const std::string &sql);
    // INSERT/UPDATE/DELETE — returns affected rows, -1 on error
    int execute(const std::string &sql);

    // Parameterized (escapes params, replaces ? left-to-right)
    // With explicit NULL markers (nulls[i]=true → SQL NULL for param i)
    MYSQL_RES *queryPrepared(const std::string &sql,
                             const std::vector<std::string> &params,
                             const std::vector<bool> &nulls);
    int executePrepared(const std::string &sql,
                        const std::vector<std::string> &params,
                        const std::vector<bool> &nulls);

    // Overload without NULL markers (all params are quoted strings)
    MYSQL_RES *queryPrepared(const std::string &sql,
                             const std::vector<std::string> &params);
    int executePrepared(const std::string &sql,
                        const std::vector<std::string> &params);

    // Transactions
    void beginTransaction();
    void commit();
    void rollback();

    // Utilities
    static std::string escapeString(const std::string &input);
    static std::string quote(const std::string &input)
    {
        return "'" + escapeString(input) + "'";
    }

    // Row reading helpers (NULL-safe)
    static std::string readString(MYSQL_ROW row, int col);
    static int readInt(MYSQL_ROW row, int col);
    static double readDouble(MYSQL_ROW row, int col);
    static bool readBool(MYSQL_ROW row, int col);

    long long lastInsertId();
};

// Global singleton accessor
Database &GetDB();

#endif // HIS_DATABASE_H
