#include "Database/Inc/db_sqlite.h"
#include <Database/Inc/db_messages.h>

#include <QSqlError>
#include <QDir>
#include <QRegularExpression>


/* ************************
 * Local Functions Prototypes - Begin
 *************************/
static bool isDbNameValid(const ConfigSQLite &config);

static bool isDbPathValid(const ConfigSQLite &config);

static bool existDbFile(const ConfigSQLite &config);

/* ************************
 * Local Functions Prototypes - End
 *************************/


DbSQLite::DbSQLite(const ConfigSQLite &config):
    m_config(config)
{
    m_db_name = m_config.db_name;
}


/**
 * Database Initialization
 *
 * Note:SQLITE3 database needs only path of database
 *
 * Checks if database already exists as QSQLDatabase
 * If so - opens it
 * If not - Add it to QSQLDatabase and open it
 *
 * @return true if Initialization is succesfull and was able to open database
 */
bool DbSQLite::initDb() noexcept
{
    try
    {
        if(!validateDb())
        {
            throw std::invalid_argument("Invalid Database file or path");
        }

        if(!QSqlDatabase::contains(m_db_name))
        {
            auto db = QSqlDatabase::addDatabase("QSQLITE", m_db_name);

            db.setDatabaseName(m_config.db_path + QDir::separator() + m_config.db_name);


            if(!db.open())
            {
                m_last_error = db.lastError().text();

                db_msg::showDbCriticalError(QObject::tr("Could not establish connection with database.\n")
                                                        + m_last_error);
            }

            m_available = db.isOpen();
        }
        else
        {
            auto db = QSqlDatabase::database(m_db_name);

            m_available = !(!db.isOpen() && !db.open());
        }

        return m_available;
    }

    catch (std::invalid_argument &e)
    {
        if(badConfigHandler())
        {
            return this->initDb();
        }
        else
        {
            db_msg::showDbCriticalError(e.what());
        }

        return false;
    }
}


/**
 * Database validator
 *
 * Note:SQLITE3 database needs only path of database
 *
 * @return true if find file at given path and it's correct datafile
 */
bool DbSQLite::validateDb()
{
    if(!(isDbPathValid(m_config) && isDbNameValid(m_config)
         && existDbFile(m_config)))
    {
        m_last_error = QObject::tr("Unable to find database file: \n");
        m_last_error += m_config.db_path + QDir::separator() + m_config.db_name;

        db_msg::showDbCriticalError(m_last_error);

        return false;
    }

    return true;
}



/**
 * Handler for Incorrect Configuration of Database
 *
 * Note:SQLITE3 database needs only path of database
 *
 * @return If path or name was bad - returns true if were changed succesfully
 * otherwise - return false
 */
bool DbSQLite::badConfigHandler() noexcept
{
    bool result(false);

    if (existDbFile(m_config))
    {
        if(!isDbNameValid(m_config))
        {
            result = db_msg::showInputDialogText("Database filename",
                                                 m_config.db_name);
            if(result)
            {
                m_db_name = m_config.db_name;
            }
        }
    }
    else
    {
        result = db_msg::showInputDialogText("Database filename",
                                             m_config.db_name);

        result &= db_msg::showInputDialogText("Database path",
                                              m_config.db_path);
    }

    return result;
}



/* ************************
 * Local Functions - Begin
 *************************/


/**
 * Regex validator of database's name
 *
 * Checks if database has proper extension to be considered as database
 *
 * @param config - Struct with database configuration
 * @return returns true if database's name has valid extension as SQLite3 database
 */
static bool isDbNameValid(const ConfigSQLite &config)
{

    if(!config.db_name.isEmpty())
    {
        QRegularExpression regex("^.*\\.(db|sdb|sqlite|db3|s3db|sqlite3|sl3)$");

        return regex.match(config.db_name).hasMatch();
    }

    return false;
}


/**
 * Checks if given path exist
 *
 * @param config - Struct with database configuration
 * @return returns true if path exists
 */
static bool isDbPathValid(const ConfigSQLite &config)
{

    if(!config.db_path.isEmpty())
    {
        QDir path (config.db_path);
        return path.exists();
    }
    return false;
}

/**
 * Checks if at given path exists database file
 *
 * @param config - Struct with database configuration
 * @return returns true if file exists at given path
 */
static bool existDbFile(const ConfigSQLite &config)
{
    return QFile::exists(config.db_path + QDir::separator() + config.db_name);
}

/* ************************
 * Local Functions - End
 *************************/




