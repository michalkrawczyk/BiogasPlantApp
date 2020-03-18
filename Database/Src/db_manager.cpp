#include "Database/Inc/db_manager.h"


DbManager::DbManager():
    m_last_error(""),
    m_available(false)
{}

DbManager::~DbManager()
{
    if(m_available)
    {
        QSqlDatabase::removeDatabase(m_db_name);
    }
}

/**
 * Database Getter
 *
 * @return QSQLDatabase Object with open database to use in queries
 */
QSqlDatabase DbManager::getDatabase() const
{
    if (!m_available)
    {
        throw std::runtime_error("Database not initialized");
    }

    return QSqlDatabase::database(m_db_name);
}

/**
 * Database Availabilty Getter
 *
 * Checks if database is available
 * That means - it's configured and connection with it is opened
 *
 * @return true if Database is available
 */
bool DbManager::isDatabaseAvailable() const
{
    return m_available;
}

