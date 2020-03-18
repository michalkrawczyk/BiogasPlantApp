#ifndef DATABASE_H
#define DATABASE_H

#include <memory>
#include <QSqlQuery>

#include "Database/Inc/db_manager.h"

typedef std::unique_ptr<DbManager> DbSQL; //consider shared_ptr


namespace database{
    DbSQL createSQLiteDatabase(const QString &path,
                               const QString &filename);

    DbSQL createMySQLDatabase(const QString &db_name,
                              const QString &hostname,
                              const QString &username,
                              const QString &password,
                              const unsigned short &port);

}//namespace database


namespace qry_helper{
    void bindValueOrNull(QSqlQuery &qry,
                         const QString &placeholder,
                         const QVariant &bind_value,
                         QSql::ParamType paramType = QSql::In);

}//namespace qry_helper

#endif // DATABASE_H