#include "GUI/Inc/menu.h"
#include "ui_menu.h"
#include "Misc/Inc/validators.h"
#include "Misc/Inc/utils.h"
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>

Menu::Menu(const unsigned int &user_id, DbSQL db_ptr, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::Menu),
    m_db_ptr(db_ptr)
{
    ui->setupUi(this);
    ui->tab_menu->setAutoFillBackground(true);
    ui->tab_personal_data->setAutoFillBackground(true);
    ui->tab_contact_data->setAutoFillBackground(true);

    logInUser(user_id);
}

Menu::~Menu()
{
    delete ui;
}

/**
 *  Perform LogIn action - Loads User Data to form
 */
void Menu::logInUser(const unsigned int &user_id)
{
    assert(user_id > 0);

    m_user_id = user_id;
    ui->label_username->setText(QString::number(m_user_id));

    loadPersonalData();
    loadCorespondanceAddress();
}

/**
 *  Invokes execution of biogas calculator window
 */
void Menu::on_pushButton_biogas_calculator_clicked()
{
    m_biogas_calc_ptr = std::make_shared<BiogasCalculator>(m_user_id, m_db_ptr);

    m_biogas_calc_ptr->setModal(true);

    this->setDisabled(true);

    QObject::connect(m_biogas_calc_ptr.get(),
                     &BiogasCalculator::exitSignal,
                     this,
                     [this](){this->setDisabled(false); });

    m_biogas_calc_ptr->show();
}

/**
 *  Updates Personal Data of user (name, surname and email)
 *
 * The action will be aborted in cases:
 * - Connection with database has been lost
 * - Text in E-mail lineEdit is not valid mail
 */
void Menu::on_pushButton_update_data_clicked()
{
    if(!database::isConnEstablished(m_db_ptr))
    {
        return;
    }

    if (!validator::isEmail(ui->lineEdit_email->text()) )
    {
        QMessageBox::warning(this,
                             "Invalid E-mail",
                             "Please provide valid e-mail");
        return;
    }

    QSqlQuery qry(m_db_ptr->getDatabase());

    qry.prepare("UPDATE biogas_server_user "
                "SET name = :name ,"
                "surname = :surname ,"
                "eMail= :email "
                "WHERE userID = :user");

    qry_helper::bindValueOrNull(qry, ":name", ui->lineEdit_name->text());
    qry_helper::bindValueOrNull(qry, ":surname", ui->lineEdit_surname->text());
    qry_helper::bindValueOrNull(qry, ":email", ui->lineEdit_email->text());
    qry.bindValue(":user", m_user_id);

    if (!qry.exec() )
    {
        QMessageBox::critical(this,
                              "Unable to change Data",
                              qry.lastError().text());
        return;
    }

    QMessageBox::information(this,
                             "Data changed",
                             "Personal Data has been changed");
}

/**
 *  Changes user's password
 *
 * The action will be aborted in cases:
 * - Connection with database has been lost
 * - Text in new password lineEdit is empty or whitespace
 * - Wrong old password has been provided
 *
 * Note: Password should be sent only encoded with MD5.
 * That will be changed in future
 */
void Menu::on_pushButton_change_password_clicked()
{
    if(validator::isEmptyOrWhitespace(ui->lineEdit_new_password->text()))
    {
        QMessageBox::warning(this,
                             "Failed to change password",
                             "New password cannot be blank");
        return;
    }

    if(!database::isConnEstablished(m_db_ptr))
    {
        return;
    }

    QSqlQuery qry(m_db_ptr->getDatabase());

    qry.prepare("SELECT userID FROM biogas_server_user "
                "WHERE userID = :username AND (password = :password OR password = :hashed_password)");

    qry.bindValue(":username", m_user_id);
    qry.bindValue(":password", ui->lineEdit_old_password->text());
    qry.bindValue(":hashed_password", encoding::MD5(ui->lineEdit_old_password->text()));

    if(qry.exec())
    {
        int result_count(0);

        while(qry.next())
            result_count++;

        if(result_count == 1)
        {
            qry.clear();
            qry.prepare("UPDATE biogas_server_user "
                        "SET password = :new_password "
                        "WHERE userID = :user");

            qry.bindValue(":new_password", encoding::MD5(ui->lineEdit_new_password->text()));
            qry.bindValue(":user", m_user_id);
        }
        else
        {
            QMessageBox::warning(this,
                                 "Failed to change password",
                                 "Wrong old password was provided");
            return;
        }
    }
    else
    {
        QMessageBox::critical(this,
                              "Unable to change Data",
                              qry.lastError().text());
        return;
    }

    QMessageBox::information(this,
                             "Data Changed",
                             "New Password has been set");

}

/**
 *  Changes/Adds user's address
 *
 * The action will be aborted in cases:
 * - Connection with database has been lost
 * - Address number has invalid format
 * - Street or City is Empty or Whitespace
 *
 * If there's no record associated with user - creates new one
 *
 */
void Menu::on_pushButton_edit_address_clicked()
{
    QString street(ui->lineEdit_street->text()),
            city(ui->lineEdit_city->text()),
            number(ui->lineEdit_adress_number->text()),
            postal_code(ui->lineEdit_postal_code->text()),
            country(ui->lineEdit_country->text());

    if(!validator::isAddressNumber(number))
    {
        QMessageBox::warning(this,
                             "Invalid Address",
                             "Please provide valid address number");
        return;
    }

    if(validator::isEmptyOrWhitespace(street) || validator::isEmptyOrWhitespace(city))
    {
        QMessageBox::warning(this,
                             "Invalid Address",
                             "City and Street fields cannot be blank");
        return;
    }

    if(validator::beginsWithWhitespace(street) || validator::beginsWithWhitespace(city)
            || validator::beginsWithWhitespace(number)
            || validator::beginsWithWhitespace(country)
            || validator::beginsWithWhitespace(postal_code))
    {
        QMessageBox::warning(this,
                             "Invalid Address",
                             "Address field cannot begin with whitespace");
        return;
    }

    if(!database::isConnEstablished(m_db_ptr))
    {
        return;
    }

    QSqlQuery qry(m_db_ptr->getDatabase());

    qry.prepare("SELECT userID_id "
                "FROM biogas_server_corespondanceaddres "
                "WHERE userID_id = :user");

    qry.bindValue(":user", m_user_id);

    if(!qry.exec())
    {
        QMessageBox::critical(this,
                              "Unable to load Data",
                              qry.lastError().text());
        return;
    }
    else
    {
        int result_count(0);
        while(qry.next())
            result_count++;

        qry.clear();

        if (result_count == 1)
        {
            qry.prepare("UPDATE biogas_server_corespondanceaddres "
                        "SET city= :city, street = :street, number = :number, postalCode = :code, country = :country "
                        "WHERE userID_id = :user");
        }
        else
        {
            qry.prepare("INSERT INTO biogas_server_corespondanceaddres "
                        "(userID_id, city, street, number, postalCode, country) "
                        "VALUES (:user, :city, :street, :number, :code, :country)");
        }

        qry.bindValue(":user", m_user_id);
        qry_helper::bindValueOrNull(qry, ":city", city);
        qry_helper::bindValueOrNull(qry, ":street", street);
        qry_helper::bindValueOrNull(qry, ":number", number);
        qry_helper::bindValueOrNull(qry, ":code", postal_code);
        qry_helper::bindValueOrNull(qry, ":country", country);

        if(!qry.exec())
        {
            QMessageBox::critical(this,
                                  "Unable to update Corespondance Address",
                                  qry.lastError().text());
            return;
        }

        QMessageBox::information(this,
                                 "Data Changed",
                                 "New Address has been set");

    }
}

/*
 *Invokes execution of phone table window
 *
*/
void Menu::on_pushButton_phone_numbers_clicked()
{
    m_phone_window_ptr = std::make_shared<PhoneTable>(m_user_id,m_db_ptr);

    m_phone_window_ptr.get()->setModal(true);
    this->setDisabled(true);

    QObject::connect(m_phone_window_ptr.get(),
                     &PhoneTable::exitSignal,
                     this,
                     [this](){this->setDisabled(false); });

    m_phone_window_ptr->show();
}

/**
 * Retrieves and loads to form Corespondance Address of user
 * that is saved in database
 *
 * @return true if operation went succesfully
 */
bool Menu::loadCorespondanceAddress()
{
    if(!database::isConnEstablished(m_db_ptr))
    {
        return false;
    }

    QString street(""),
            city(""),
            number(""),
            postal_code(""),
            country("");

    QSqlQuery qry(m_db_ptr->getDatabase());

    qry.prepare("SELECT city, street, number, postalCode AS post, country "
                "FROM biogas_server_corespondanceaddres "
                "WHERE userID_id = :user");
    qry.bindValue(":user", m_user_id);

    if (qry.exec())
    {
        int result_count(0);

        while(qry.next())
            result_count++;

        if(result_count == 1)
        {
            qry.previous();
            city = qry.value(0).toString();
            street = qry.value(1).toString();
            number = qry.value(2).toString();
            postal_code = qry.value(3).toString();
            country = qry.value(4).toString();
        }

        ui->lineEdit_city->setText(city);
        ui->lineEdit_street->setText(street);
        ui->lineEdit_adress_number->setText(number);
        ui->lineEdit_postal_code->setText(postal_code);
        ui->lineEdit_country->setText(country);
    }
    else
    {
        return false;
    }

    return true;
}

/**
 * Retrieves and loads to form Personal Data of user
 * that is saved in database
 *
 * @return true if operation went succesfully
 */
bool Menu::loadPersonalData()
{
    if(!database::isConnEstablished(m_db_ptr))
    {
        return false;
    }

    QSqlQuery qry(m_db_ptr->getDatabase());

    qry.prepare("SELECT name, surname, email FROM biogas_server_user "
                "where userID = :username");
    qry.bindValue(":username", m_user_id);

    if(qry.exec() )
    {
        qry.next();

        ui->lineEdit_name->setText(qry.value(0).toString() );
        ui->lineEdit_surname->setText(qry.value(1).toString() );
        ui->lineEdit_email->setText(qry.value(2).toString() );

        return true;
    }

    return false;
}


void Menu::on_pushButton_services_clicked()
{
//    m_svcs_ptr.reset(new Services(m_user_id, m_db_ptr));
    m_svcs_ptr = std::make_shared<Services>(m_user_id, m_db_ptr);

    m_svcs_ptr->setModal(true);

    this->setDisabled(true);

    QObject::connect(m_svcs_ptr.get(),
                     &Services::exitSignal,
                     this,
                     [this](){this->setDisabled(false); });

    m_svcs_ptr->show();
}
