#ifndef BIOGAS_CALCULATOR_H
#define BIOGAS_CALCULATOR_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include "Database/Inc/database.h"
#include <QTableView>

namespace Ui {
class BiogasCalculator;
}

class BiogasCalculator : public QDialog
{
    Q_OBJECT

public:
    explicit BiogasCalculator(unsigned int user_id, DbSQL db_ptr, QWidget *parent = nullptr);
    ~BiogasCalculator();

signals:
    void exitSignal();

private slots:
    void on_comboBox_pick_plant_currentIndexChanged(int index);

    void on_pushButton_select_substrate_clicked();

    void on_pushButton_remove_substrate_clicked();

    void on_pushButton_clear_all_clicked();

    void on_pushButton_menu_clicked();

private:
    Ui::BiogasCalculator *ui;
    DbSQL m_db_ptr;

    unsigned int m_user_id;
    unsigned int m_plant_picked;

    double m_max_volume;
    double m_available_volume;

    std::vector<unsigned int> m_plants;

    std::unique_ptr<QSqlQueryModel> m_model_substrates_available_ptr;
    std::unique_ptr<QStandardItemModel> m_model_substrates_picked_ptr;

    void mInitWindow();
    void mClearTable(QTableView *table_ptr);
    void mConfigureTable(QTableView *table_ptr);
    void mLoadTableAppearance(QTableView *table_ptr);

    bool mLoadPlantVolume();
    bool mLoadAvailablePlants();
    bool mLoadAvailableSubstrates();
    void mLoadAvailableVolume();
    void mLoadExpectedResults(const double &methane, const double &biogas);

    void mUpdateAvailableVolume();
    void mUpdateExpectedResults();
    void mUpdateTable(QTableView *table, bool show = false);
    bool mUpdateTableAvailableSubstrates(QSqlQuery &qry);

    bool mIsAdditionPossible(const QString &substrate_name);
    bool mIsAvailableSubstrateRecorded(const qlonglong id);

    void reject() override;

    void mBlockWindow();
    //functions below will be consided to be Exceptions
    void noPlantsHandler();


};

#endif // BIOGAS_CALCULATOR_H
