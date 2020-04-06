#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "Database/Inc/db_manager.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr, DbManager *db_ptr = nullptr);
    ~Login();

private slots:
    void on_pushButton_sign_in_clicked();

private:
    Ui::Login *ui;
    DbManager *m_db_ptr;
};

#endif // LOGIN_H
