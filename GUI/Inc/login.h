#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "Database/Inc/database.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(DbSQL db_ptr, QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_pushButton_sign_in_clicked();

private:
    Ui::Login *ui;
    DbSQL m_db_ptr;
};

#endif // LOGIN_H
