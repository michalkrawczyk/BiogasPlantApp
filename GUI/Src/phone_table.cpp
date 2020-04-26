#include "GUI\Inc\phone_table.h"
#include "ui_phone_table.h"
#include "Delegates/Inc/phone_table_delegate.h"
#include "Misc/Inc/validators.h"
#include <QMessageBox>
#include <QSqlError>


PhoneTable::PhoneTable(const unsigned int &user_id, DbSQL db_ptr, QWidget *parent):
    QDialog(parent),
    ui(new Ui::PhoneTable),
    m_user_id(user_id),
    m_db_ptr(db_ptr),
    m_table_model_ptr(new QSqlTableModel(parent, m_db_ptr->getDatabase()))
{
    assert(db_ptr);
    assert(user_id > 0);
    assert(m_table_model_ptr);

    ui->setupUi(this);
    if(!mLoadPhoneNumbers())
    {
        QMessageBox::warning(this,
                             "Database Error",
                             "Unable To Fetch Data from Database");
    }
}

PhoneTable::~PhoneTable()
{
    delete ui;
}

/**
 * Goes Back to menu by emiting exitSignal
 */
void PhoneTable::on_pushButton_menu_clicked()
{
    emit exitSignal();
    this->hide();
}


/**
 * Performs assigning new phone number to user
 * If given number match E.164 Standard and is not recorded already
 * - Assigning to user is done via adding record in SQL Database
 */
void PhoneTable::on_pushButton_add_number_clicked()
{
    QString number(ui->lineEdit_phone->text());

    if(!validator::isPhoneNumber(number))
    {
        QMessageBox::warning(this,
                             "Invalid Phone Number",
                             "Please Provide Valid Number");
        return;
    }

    QSqlQuery qry(m_db_ptr->getDatabase());


    if(mFindRecordPhoneID(qry, number))
    {
        int resultCount(0);
        while(qry.next())
            resultCount++;

        if (resultCount > 0)
        {
            QMessageBox::warning(this,
                                 "Failed to Add Phone",
                                 "Phone Number already exists");
            return;
        }
        else
        {
            qry.clear();

            qry.prepare("INSERT INTO biogas_server_phonenumber "
                        "(phoneNumber, owner_id) "
                        "VALUES (:number, :user)");
            qry.bindValue(":number", number);
            qry.bindValue(":user", m_user_id);

            if(!qry.exec())
            {
                QMessageBox::critical(this,
                                      "Unable to add Phone Number",
                                      qry.lastError().text());
                return;
            }
            else
                QMessageBox::information(this,
                                         "Data Changed",
                                         "New Phone Number was added");
        }
    }
    else
    {
        QMessageBox::warning(this,
                             "Unable to execute querry",
                             "Please contact with support");
        return;
    }

    if(!mLoadPhoneNumbers())
    {
        QMessageBox::warning(this,
                             "Database Error",
                             "Unable To Refresh Data in Table");
    }
}

/**
 * Loads User's list of phone numbers to table
 */
bool PhoneTable::mLoadPhoneNumbers()
{
    m_table_model_ptr->setTable("biogas_server_phonenumber");
    m_table_model_ptr->setFilter("owner_id = " + QString::number(m_user_id));
    m_table_model_ptr->setEditStrategy(QSqlTableModel::OnFieldChange);

    m_table_model_ptr->setHeaderData(1, Qt::Horizontal, tr("Phone Number"));

    if(!m_table_model_ptr->select())
        return false;

    ui->tableView->setModel(m_table_model_ptr.get());
    ui->tableView->setColumnWidth(1, 188);

    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(2);

    auto *delegate_ptr = new delegate::PhoneTableDelegate();
    ui->tableView->setItemDelegateForColumn(1, delegate_ptr);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);

    ui->tableView->show();
    return true;
}

/**
 * Preperares querry to check if phone number is not already assigned to user
 */
bool PhoneTable::mFindRecordPhoneID(QSqlQuery &qry, const QString &phone_number)
{
    qry.clear();
    qry.prepare("SELECT phoneID "
                "FROM biogas_server_phonenumber "
                "WHERE owner_id = :user "
                "AND phoneNumber = :number ");
    qry.bindValue(":user", m_user_id);
    qry.bindValue(":number", phone_number);

    return qry.exec();
}

/**
 * Override of reject function (operation made on closing window)
 * emiting exitSignal allows to go back to Menu
 */
void PhoneTable::reject()
{
    emit exitSignal();
    QDialog::reject();
}

/**
 * Removes selected numbers from user and table with button clicked
 */
void PhoneTable::on_pushButton_remove_clicked()
{
    QItemSelectionModel *selection(ui->tableView->selectionModel());

    if(selection && selection->hasSelection())
    {
        for (auto picked : selection->selectedRows())
        {
            m_table_model_ptr->removeRow(picked.row());
        }
    }
    else
    {
        QMessageBox::information(this,
                                 "Provide information first",
                                 "Please select first record you want to remove");
    }

    mLoadPhoneNumbers();
}
