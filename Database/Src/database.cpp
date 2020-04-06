#include "Database/Inc/database.h"

#include "Database/Inc/db_sqlite.h"
#include "Database/Inc/db_mysql.h"

#include "Misc/Inc/validators.h"
#include <QMessageBox>

using namespace validator;

namespace database {
    /**
     * Creates new SQLite Database instance
     *
     * @param path - absolute path to db file
     * @param filename - name of db file
     * @return new instance of SQLite Database as smart pointer (unique)
     */
    DbSQL createSQLiteDatabase(const QString &path,
                               const QString &filename)
    {
        ConfigSQLite config{filename, path};

        return DbSQL(new DbSQLite(config));
    }

    /**
     * Creates new MySQL Database instance
     *
     * @param db_name - name of database to connect
     * @param hostname - MySQL host
     * @param username - MySQL username (to reconsider for safety)
     * @param password - MySQL password (to reconsider for safety)
     * @return new instance of MySQL Database as smart pointer (unique)
     */
    DbSQL createMySQLDatabase(const QString &db_name,
                              const QString &hostname,
                              const QString &username,
                              const QString &password,
                              const unsigned short &port)
    {
        ConfigMySQL config{db_name, hostname, username, password, port};

        return DbSQL(new DbMySQL(config));
    }

    /**
     * Checks if connection is still open
     * if it's not - tries to reconnect
     *
     * @param db_ptr - pointer to database
     * @return true if connection is open or reopened
     * if unavailable - false and throws window with critical message
     */
    bool isConnEstablished(DbManager *db_ptr)
    {
        if (db_ptr->isDatabaseAvailable())
        {
            if(!db_ptr->getDatabase().isOpen()) //sometimes connection might be lost
            {
                if (!db_ptr->getDatabase().open() )
                {
                    QMessageBox::critical(nullptr,
                                          "Connection Error",
                                          "Failed to connect with database - please try again later");
                }
            }

            return db_ptr->getDatabase().isOpen();
        }
        return false;
    }

} //namespace database


namespace qry_helper {
    /**
     * Handler to bind value properly in SQL querries
     * Typical querry passes binded values when empty or null as " "
     * This function make sure that in those situations instead
     * of whitespace - NULL will be sent to database
     *
     *
     * @param qry - reference to querry, what uses binded value
     * @param placeholder - reference to placeholder (word in SQL Querry that
     * will be replaced)
     * @param bind_value - value that will replace placeholder if it's not NULL
     * @param paramType - enum that is used to specify the type of a bind parameter.
     * (default QSql::In that means "The bind parameter is used to put data into the database."
     */
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
