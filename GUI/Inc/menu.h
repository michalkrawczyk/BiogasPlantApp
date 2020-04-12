#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include "Database/Inc/database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Menu; }
QT_END_NAMESPACE

class Menu : public QMainWindow
{
    Q_OBJECT

public:
    Menu(const unsigned int &user_id, DbSQL db_ptr, QWidget *parent = nullptr);
    ~Menu();


private slots:   
    void on_pushButton_biogas_calculator_clicked();

    void on_pushButton_update_data_clicked();
    void on_pushButton_change_password_clicked();

    void on_pushButton_edit_address_clicked();
    void on_pushButton_phone_numbers_clicked();



private:
    Ui::Menu *ui;
    unsigned int m_user_id;
    DbSQL m_db_ptr;

    void logInUser(const unsigned int &user_id);

    bool loadCorespondanceAddress();
    bool loadPersonalData();

};
#endif // MENU_H