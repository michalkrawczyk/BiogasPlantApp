#include "Database/Inc/db_mysql.h"
#include "Database/Inc/db_messages.h"

#include <algorithm>
#include <limits>

#include <QSqlError>
#include <QMessageBox>
#include <QRegularExpression>

/* ************************
 * Local Functions Prototypes - Begin
 *************************/

static bool isValidDbPort(const ConfigMySQL &config);

static bool isValidDbName(const ConfigMySQL &config);

/* ************************
 * Local Functions Prototypes - End
 *************************/

DbMySQL::DbMySQL(const ConfigMySQL &config):
    m_config(config)
{
    m_db_name = m_config.db_name;
}

/**
 * Database Initialization
 *
 * Checks if database already exists as QSQLDatabase
 * If so - opens it
 * If not - Add it to QSQLDatabase and open it
 *
 * @return true if Initialization is succesfull and was able to open database
 */
bool DbMySQL::initDb() noexcept
{
    try
    {
      if(!validateDb())
      {
          throw std::invalid_argument("Invalid Database Configuration");
      }

      if(!QSqlDatabase::contains(m_db_name))
      {
          auto db = QSqlDatabase::addDatabase("QMYSQL", m_db_name);

          db.setDatabaseName(m_config.db_name);
          db.setPort(m_config.port);
          db.setHostName(m_config.hostname);

          db.setUserName(m_config.username); // consider move user_name and password from config
          db.setPassword(m_config.username);

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
 * @return true if database has valid port and name
 */
bool DbMySQL::validateDb()
{
    if(!(isValidDbName(m_config) && isValidDbPort(m_config)))
    {
        m_last_error = QObject::tr("Invalid Database Configuration: \n");
        m_last_error += QObject::tr("Name: ") + m_config.db_name + '\n';
        m_last_error += "Port: " + QString(m_config.port) + 'n';

        db_msg::showDbCriticalError(m_last_error);

        return false;
    }

    return true;
}

/**
 * Handler for Incorrect Configuration of Database
 *
 * @return If db_name or port was bad - returns true if were changed succesfully
 * otherwise - return false
 */
bool DbMySQL::badConfigHandler() noexcept
{
    bool result(false);

    if(!isValidDbName(m_config))
    {
        result = db_msg::showInputDialogText(QObject::tr("Database name"), m_config.db_name);
    }

    if(!isValidDbPort(m_config))
    {
        int new_port(m_config.port);

        result = db_msg::showInputDialogInt(QObject::tr("Database port"),
                                            new_port,
                                            std::numeric_limits<unsigned short>::min(),
                                            std::numeric_limits<unsigned short>::max());

        if(result)
        {
            m_config.port = new_port;
        }
    }

    return result;
}

/* ************************
 * Local Functions - Begin
 *************************/

/**
 * Database port validator
 *
 * Checks if database has valid port from array of allowed ports
 *
 * @param config - Struct with database configuration
 * @return returns true if database's port is allowed
 */
static bool isValidDbPort(const ConfigMySQL &config)
{
    unsigned short int allowed_ports[] = {
        3306,
        33060
    };
    short int port_count(sizeof (allowed_ports) / sizeof (short));

    return std::any_of(allowed_ports,
                       allowed_ports + port_count,
                       [&config](unsigned short int x){return config.port == x;});
}

/**
 * Regex validator of database's name
 *
 * Checks if database has proper extension to be considered as database
 *
 * @param config - Struct with database configuration
 * @return returns true if database's name has valid name as MYSQL database
 */
static bool isValidDbName(const ConfigMySQL &config)
{
    if(!config.db_name.isEmpty())
    {
        QRegularExpression regex("^[^\\/?%*:|\"<>.]{1,64}$");

        return regex.match(config.db_name).hasMatch();
    }

    return false;
}
/* ************************
 * Local Functions - End
 *************************/
