#ifndef PHONE_TABLE_H
#define PHONE_TABLE_H

#include <QDialog>
#include "Database/Inc/database.h"
#include <QSqlTableModel>

namespace Ui {
class PhoneTable;
}

class PhoneTable : public QDialog
{
    Q_OBJECT

public:
    explicit PhoneTable(const unsigned int &user_id, DbSQL db_ptr,QWidget *parent = nullptr);
    ~PhoneTable();
signals:
    void exitSignal();

private slots:
    void on_pushButton_menu_clicked();

    void on_pushButton_add_number_clicked();

    void on_pushButton_remove_clicked();

private:
    Ui::PhoneTable *ui;
    unsigned int m_user_id;
    DbSQL m_db_ptr;
    std::unique_ptr<QSqlTableModel> m_table_model_ptr;

    bool mLoadPhoneNumbers();
    bool mFindRecordPhoneID(QSqlQuery &qry, const QString &phone_number);
    void reject() override;
};

#endif // PHONE_TABLE_H
