#include "Database/Inc/database.h"
#include <QVariant>
#include <map>

#include "Database/Inc/db_sqlite.h"
#include "Database/Inc/db_mysql.h"

#include "Misc/Inc/validators.h"

using namespace validator;

namespace database {
    DbSQL createSQLiteDatabase(const QString &path,
                               const QString &filename)
    {
        ConfigSQLite config{filename, path};

        return DbSQL(new DbSQLite(config));
    }

    DbSQL createMySQLDatabase(const QString &db_name,
                              const QString &hostname,
                              const QString &username,
                              const QString &password,
                              const unsigned short &port)
    {
        ConfigMySQL config{db_name, hostname, username, password, port};

        return DbSQL(new DbMySQL(config));
    }


} //namespace database


namespace qry_helper {
    void bindValueOrNull(QSqlQuery &qry,
                        const QString &placeholder,
                        const QVariant &bind_value,
                        QSql::ParamType paramType)
    {
        if (bind_value.isNull() || isEmptyOrWhitespace(bind_value))
        {
            qry.bindValue(placeholder, QVariant(), paramType);
        }
        else
        {
            qry.bindValue(placeholder, bind_value, paramType);
        }
    }
}//namespace qry_helper
