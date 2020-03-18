#ifndef DB_MESSAGES_H
#define DB_MESSAGES_H

#include <QString>
#include <QMessageBox>


namespace db_msg{
    void showDbCriticalError(const QString &error_msg,
                             QWidget *parent = nullptr,
                             const QString &optional_msg = "");

    bool showInputDialogText(const QString &var_name,
                             QString &var,
                             QWidget *parent = nullptr);

    bool showInputDialogInt(const QString &var_name,
                            int &var,
                            int min = -2147483647,
                            int max = 214783647,
                            int step = 1,
                            QWidget *parent = nullptr);

}//namespace db_msg

#endif // DB_MESSAGES_H
