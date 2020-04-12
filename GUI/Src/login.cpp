#include "GUI/Inc/login.h"
#include "ui_login.h"
#include "Database/Inc/database.h"
#include "Misc/Inc/utils.h"
#include "GUI/Inc/menu.h"

#include <QSqlQuery>
#include <QMessageBox>

extern std::unique_ptr<Menu> main_menu;


Login::Login(DbSQL db_ptr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login),
    m_db_ptr(db_ptr)
{
    if (!m_db_ptr)
    {
       throw std::invalid_argument("db_ptr cannot be null");
    }

    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

/**
 * Performs sign in operation on button clicked
 *
 * Note: Password is normaly stored as MD5 encoded string
 *  But on creation of user it might have password set by admin
 *
 * Note: In this prototype password is being sent still directly
 *  which is not safe - for final form of project will be changed
 *
 */
void Login::on_pushButton_sign_in_clicked()
{
    if(!database::isConnEstablished(m_db_ptr))
    {
        return;
    }

    QSqlQuery qry(m_db_ptr->getDatabase());

    qry.prepare("SELECT userID FROM biogas_server_user "
                "WHERE userID = :username AND (password = :password OR password = :hashed_password)");
    qry.bindValue(":username",ui->lineEdit_user->text());
    qry.bindValue(":password",ui->lineEdit_password->text());
    qry.bindValue(":hashed_password", encoding::MD5(ui->lineEdit_password->text()));
    qry.exec();

    int result_count(0);
    while(qry.next())
        result_count++;

    if(result_count == 1)
    {
        this->hide();

        main_menu = std::unique_ptr<Menu>(new Menu(ui->lineEdit_user->text().toUInt(),
                                                   m_db_ptr) );
        main_menu->show();
    }
    else
    {
        // Primary key of database do not allow to record same UserID
        // Works also when there's no user with that login or password
        QMessageBox::warning(nullptr,
                             "Failed To Log In",
                             "Invalid username or password - Please Try Again");
        ui->lineEdit_password->clear();
    }


}


