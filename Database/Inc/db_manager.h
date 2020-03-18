#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QString>


class DbManager
{
public:
    DbManager();
    ~DbManager();

    virtual bool initDb() noexcept = 0;
    virtual bool validateDb() = 0;

    QSqlDatabase getDatabase() const;
    bool isDatabaseAvailable() const;

    DbManager(const DbManager&) = delete;
    DbManager &operator= (const DbManager&) = delete;

protected:
    QString m_db_name;
    QString m_last_error;

    bool m_available;

    virtual bool badConfigHandler() noexcept = 0;
};




#endif // DBMANAGER_H
