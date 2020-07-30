#ifndef SERVICES_H
#define SERVICES_H

#include "Database/Inc/database.h"
#include "Delegates/Inc/service_delegate.h"

#include <QDialog>
#include <vector>

namespace Ui {
class Services;
}

class Services final: public QDialog
{
    Q_OBJECT

public:
    explicit Services(const unsigned int &user_id, DbSQL db_ptr,QWidget *parent = nullptr);
    ~Services();

signals:
    void exitSignal();

private slots:
    void on_comboBox_plants_currentIndexChanged(int index);

    void on_pushButton_menu_clicked();

private:
    Ui::Services *ui;
    std::unique_ptr<sqlModels::ServiceTable> m_svcs_mdl_ptr;
    DbSQL m_db_ptr;
    const unsigned int m_user_id;
    unsigned long long m_plant_picked;
    std::vector<unsigned long long> m_plants_available;

    bool mLoadPlants() noexcept;
    bool mLoadSvcs() noexcept;
    void mConfigSvcsTable();

    void mBlockWindow();
    void reject() override;

};

#endif // SERVICES_H
