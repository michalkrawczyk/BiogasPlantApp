#ifndef DB_MYSQL_H
#define DB_MYSQL_H

#include "Database/Inc/db_manager.h"


struct ConfigMySQL
{
    QString db_name;
    QString hostname;
    QString username;
    QString password;
    unsigned short int port;
};


class DbMySQL final : public DbManager
{
public:
    explicit DbMySQL(const ConfigMySQL &config);

    bool initDb() noexcept override;
    bool validateDb() override;

private:
    ConfigMySQL m_config;

    bool badConfigHandler() noexcept override;
};

#endif // DB_MYSQL_H
