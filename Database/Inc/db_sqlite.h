#ifndef DBSQLITE_H
#define DBSQLITE_H

#include "Database/Inc/db_manager.h"


struct ConfigSQLite
{
    QString db_name;
    QString db_path;
};


class DbSQLite final: public DbManager
{
public:
    explicit DbSQLite(const ConfigSQLite &config);

    bool initDb() noexcept override;
    bool validateDb() override;

private:
    ConfigSQLite m_config;

    bool badConfigHandler() noexcept override;
};

#endif // DBSQLITE_H
