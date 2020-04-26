#include "GUI\Inc\biogas_calculator.h"
#include "ui_biogas_calculator.h"
#include "Misc/Inc/validators.h"
#include <memory>
#include <QMessageBox>
#include <QSqlError>
#include <stack>

#include <QDebug>


BiogasCalculator::BiogasCalculator(unsigned int user_id, DbSQL db_ptr, QWidget *parent):
    QDialog(parent),
    ui(new Ui::BiogasCalculator),
    m_db_ptr(db_ptr),
    m_user_id(user_id),
    m_plant_picked(0),
    m_max_volume(0),
    m_available_volume(0)
{
    ui->setupUi(this);
    mInitWindow();
}

BiogasCalculator::~BiogasCalculator()
{
    delete ui;
}


/**
 * Performs switching plants on picking from ComboBox
 *
 * @param index - index of value in ComboBox
 */
void BiogasCalculator::on_comboBox_pick_plant_currentIndexChanged(int index)
{
    if(index >= 0 && m_plants.size() != 0)
    {
        m_plant_picked = m_plants[unsigned(index)];
        mLoadPlantVolume();
        mLoadAvailableSubstrates();

        mClearTable(ui->tableView_chosen_substrates);
        mUpdateAvailableVolume();
    }
}

/**
 * Performs operation on selected substrate from list of available
 * Push them to picked list and calculates expected values
 */
void BiogasCalculator::on_pushButton_select_substrate_clicked()
{
    QItemSelectionModel *select = ui->tableView_available_substrates
            ->selectionModel();

    double amount = ui->lineEdit_substrate_amount->text().toDouble(); //TODO:Add handler for ","
    double ts = ui->lineEdit_TS->text().toDouble();

    assert(m_model_substrates_picked_ptr);

    if(select && select->hasSelection())
    {
        int columns_count(select->model()->columnCount());
        int index(0);

        for(auto &it: select->selectedRows())
        {
            if(!mIsAdditionPossible(select->selectedRows(1).value(index).data().toString()) ||
                    mIsAvailableSubstrateRecorded(it.data().toLongLong()))
            {
                ++index;
                continue;
            }

            int row(m_model_substrates_picked_ptr->rowCount());

            m_model_substrates_picked_ptr->insertRow(row);

            for (int column = 0; column < columns_count; column++)
            {
                QVariant var = select->selectedRows(column).value(index).data();
                m_model_substrates_picked_ptr->setData(m_model_substrates_picked_ptr->index(row,column), var.toString());
            }
            m_model_substrates_picked_ptr->setData(m_model_substrates_picked_ptr->index(row, 5), amount);
            m_model_substrates_picked_ptr->setData(m_model_substrates_picked_ptr->index(row, 6), ts);

            mUpdateAvailableVolume();
            mUpdateExpectedResults();

            ++index;
        }

        mUpdateTable(ui->tableView_chosen_substrates);
    }
    else
    {
        QMessageBox::information(this,
                                 "No selected records",
                                 "Please select substrate first");

    }
}


/**
 * Performs removing selected substrates from table of picked substrates
 * Note:Stack avoids bug situations on picking more than 1 substrate
 */
void BiogasCalculator::on_pushButton_remove_substrate_clicked()
{
    QItemSelectionModel *selection(ui->tableView_chosen_substrates
                                   ->selectionModel());

    if(selection && selection->hasSelection())
    {
        std::stack<int> rows;
        for(auto picked:selection->selectedRows())
        {
            rows.push(picked.row());
        }

        while(!rows.empty())
        {
            m_model_substrates_picked_ptr->removeRow(rows.top());
            rows.pop();

            mUpdateAvailableVolume();
            mUpdateExpectedResults();
        }
    }

    else
    {
        QMessageBox::information(this,
                                 "Provide information first",
                                 "Please select first record you want to remove");
    }
}

/**
 * Clears table from picked substrates
 */
void BiogasCalculator::on_pushButton_clear_all_clicked()
{
    if(m_model_substrates_picked_ptr)
    {
        int row_count(m_model_substrates_picked_ptr->rowCount());
        m_model_substrates_picked_ptr->removeRows(0, row_count);
    }

    mUpdateAvailableVolume();
    mUpdateExpectedResults();
}

/**
 * Goes Back to menu by emiting exitSignal
 */
void BiogasCalculator::on_pushButton_menu_clicked()
{
    emit exitSignal();
    QDialog::reject();
}

/**
 * Initialize window
 * -Configures Tables
 * -Load Available Plants and Substrates
 * -Load Plant Volume
 */
void BiogasCalculator::mInitWindow()
{
    m_model_substrates_picked_ptr = std::unique_ptr<QStandardItemModel>(new QStandardItemModel(0,7));

    mConfigureTable(ui->tableView_available_substrates);
    mConfigureTable(ui->tableView_chosen_substrates);

    if(mLoadAvailablePlants())
    {
        mLoadAvailableSubstrates();
    }
    mLoadPlantVolume();     //bool
    mUpdateAvailableVolume();

}

/**
 * Clears Table Content
 * @param table_ptr - pointer to table that will be cleared
 */
void BiogasCalculator::mClearTable(QTableView *table_ptr)
{
    if(!table_ptr->model())
    {
        if(table_ptr == ui->tableView_chosen_substrates)
        {
            m_model_substrates_picked_ptr->clear();
        }
        else if (table_ptr == ui->tableView_available_substrates)
        {
            m_model_substrates_available_ptr->clear();
        }
    }

    table_ptr->show();
}

/**
 * Sums volume from all containers asigned to plant
 * and loads it to lineEdit
 */
bool BiogasCalculator::mLoadPlantVolume()
{
    m_max_volume = 0;

    QSqlQuery qry(m_db_ptr->getDatabase());

    qry.prepare("SELECT SUM(volume) "
                "FROM biogas_server_container "
                "WHERE fromPlant_id = :plant");
    qry.bindValue(":plant", m_plant_picked);


    if (!qry.exec())
    {
        QMessageBox::warning(this,
                             "Failed to Load Volume",
                             qry.lastError().text());
    }

    else
    {
        qry.next();
        m_max_volume = qry.value(0).toInt();
    }

    ui->lineEdit_max_volume
            ->setText(QString::number(m_max_volume));

    return qry.isActive();
}

/**
 * Loads Plants assigned to user to ComboBox
 * Note: If error occur - noPlantsHandler is activated
 */
bool BiogasCalculator::mLoadAvailablePlants()
{
    ui->comboBox_pick_plant->clear();
    m_plants.clear();

    QSqlQuery qry(m_db_ptr->getDatabase());

    qry.prepare("SELECT PlantID, location "
                "FROM biogas_server_plant "
                "WHERE owner_id = :user");
    qry.bindValue(":user", m_user_id);

    if(qry.exec())
    {
        while(qry.next())
        {
            QString temp = qry.value(0).toString() + " - " + qry.value(1).toString();
            ui->comboBox_pick_plant->addItem(temp);

            m_plants.push_back(qry.value(0).toInt());
        }
        if(!m_plants.empty())
        {
            m_plant_picked = m_plants[0];
            return true;
        }
    }
    else
    {
        QMessageBox::warning(this,
                             "Failed to Load Plants",
                             qry.lastError().text());
    }

    noPlantsHandler();

    return false;

}

/**
 * Loads Plants assigned to user to ComboBox
 * Note: If error occur - noPlantsHandler is activated
 */
bool BiogasCalculator::mLoadAvailableSubstrates()
{
    QSqlQuery qry(m_db_ptr->getDatabase());

    qry.prepare("SELECT * FROM biogas_server_substrate AS substrates "
                "WHERE NOT EXISTS (SELECT * FROM biogas_server_substrate_owner AS owners "
                "WHERE substrates.substrateID = owners.substrate_id)"
                "UNION "
                "SELECT * FROM biogas_server_substrate AS substrates WHERE substrateID IN"
                " (SELECT substrate_id FROM biogas_server_substrate_owner AS owners"
                " WHERE owners.user_id = :user)");

    qry.bindValue(":user", m_user_id);

    if (!qry.exec())
    {
        QMessageBox::warning(this,
                             "Failed to Load Substrates",
                             qry.lastError().text());
        qDebug()<<qry.lastQuery();
        return false;
    }

    qry.next();

    return mUpdateTableAvailableSubstrates(qry);
}

/**
 * Loads available volume to lineEdit
 */
void BiogasCalculator::mLoadAvailableVolume()
{
    ui->lineEdit_available_volume
            ->setText(QString::number(m_available_volume));
}


/**
 * Loads calculated expected products to proper lineEdits
 */
void BiogasCalculator::mLoadExpectedResults(const double &methane, const double &biogas)
{
    ui->lineEdit_methane_expected
            ->setText(QString::number(methane));
    ui->lineEdit_biogas_expected
            ->setText(QString::number(biogas));
}

/**
 * Calculates Available Volume
 * Available Volume means volume that left from substracting picked substrates
 * from Maximum Volume (Sum of volume of all containers assigned to plant)
 */
void BiogasCalculator::mUpdateAvailableVolume()
{
    int row_count(0);

    if (m_model_substrates_picked_ptr.get())
    {
       row_count = m_model_substrates_picked_ptr->rowCount();
    }

    m_available_volume = m_max_volume;


    if (row_count != 0)
    {
        for (int row = 0; row < row_count; row++)
        {
            m_available_volume -= m_model_substrates_picked_ptr
                    ->data(m_model_substrates_picked_ptr->index(row,5))
                    .toDouble();
        }
    }

    mLoadAvailableVolume();
}


/**
 * Calculates Expected Result (Simplified for now)
 *
 */
void BiogasCalculator::mUpdateExpectedResults()
{
    double temp_methane(0),
           temp_biogas(0),
           ts(0),
           ots(0),
           total_methane(0),
           amount(0),
           total_biogas(0);

    int row_count(m_model_substrates_picked_ptr->rowCount());

    if (row_count != 0)
    {
        for (int row = 0; row < row_count; row++)
        {

            ots = m_model_substrates_picked_ptr->index(row, 2).data().toDouble();

            temp_biogas = m_model_substrates_picked_ptr->index(row, 3).data().toDouble();

            temp_methane = m_model_substrates_picked_ptr->index(row, 4).data().toDouble();

            amount = m_model_substrates_picked_ptr->index(row, 5).data().toDouble();

            ts = m_model_substrates_picked_ptr->index(row, 6).data().toDouble();

            total_methane += ots/100. * ts/100. * amount * temp_methane;
            total_biogas += ots/100. * ts/100. * amount * temp_biogas;
        }
    }

    mLoadExpectedResults(total_methane, total_biogas);
}

/**
 * Updates Tables by reloading paired pointers and appearance
 * @param table_ptr - pointer to table that will be updated
 * @param show (default false) - tells if effect should be visible in table
 * on function finished
 */
void BiogasCalculator::mUpdateTable(QTableView *table_ptr, bool show)
{
    bool isValid(false);

    if (table_ptr == ui->tableView_available_substrates)
    {
        if (m_model_substrates_available_ptr)
        {
            table_ptr->setModel(m_model_substrates_available_ptr.get());

            isValid = true;
        }
    }
    else if (table_ptr == ui->tableView_chosen_substrates)
    {
        if (m_model_substrates_picked_ptr)
        {
            table_ptr->setModel(m_model_substrates_picked_ptr.get());

            isValid = true;
        }
    }

    mLoadTableAppearance(table_ptr);

    if (isValid && show)
    {
        table_ptr->show();
    }

}

/**
 * Updates Tables by reloading paired pointers and appearance
 * Operation will be aborted if querry is not SELECT type or invalid
 * @param qry - SQL querry to be executed (reference)
 */
bool BiogasCalculator::mUpdateTableAvailableSubstrates(QSqlQuery &qry)
{
    if(!(qry.isSelect() && qry.isValid()))
    {
        QMessageBox::warning(this,
                             "Failed to Load Substrates",
                             qry.lastError().text());
        return false;
    }

    m_model_substrates_available_ptr->setQuery(qry);



    mUpdateTable(ui->tableView_available_substrates, true);
    return true;
}


/**
 * Validates if set Ammount, TS and Availible Volume allows to perform operation
 * of calculating results (are they valid):
 * -Ts have to be positive Percentage value
 * - Ammount have to be positive Double Value
 * -Ammount of taken substrate can't be higher than Available Volume
 * @param substrate_name - name of substrate that is checked
 */
bool BiogasCalculator::mIsAdditionPossible(const QString &substrate_name)
{
    double ammount = ui->lineEdit_substrate_amount->text().toDouble();

    if(!validator::isPositiveDouble(ammount))
    {
        QMessageBox::warning(this,
                             "Operation cannot be executed",
                             "Amount has to be positive value");
        return false;
    }

    if(!validator::isPositivePercentage(ui->lineEdit_TS->text()))
    {
        QMessageBox::warning(this,
                             "Operation cannot be executed",
                             "TS has to be positive, percentage value");
        return false;
    }

    if(ammount > m_available_volume)
    {
        QMessageBox::warning(this,
                             "Operation cannot be executed",
                             substrate_name +
                             "- Picked Amount exceeds available volume");
        return false;
    }

    return true;
}

/**
 *  Checks if selected substrate is already in table with picked substrates
 * @param id - substrate ID
 *
 */
bool BiogasCalculator::mIsAvailableSubstrateRecorded(const qlonglong id)
{
    qlonglong row_count(0);
    if(m_model_substrates_picked_ptr)
    {
        row_count = m_model_substrates_picked_ptr->rowCount();
    }

    for (auto row = 0; row < row_count; row++)
    {
        if(m_model_substrates_picked_ptr->data(m_model_substrates_picked_ptr->index(row, 0)).toLongLong() == id)
        {
            QMessageBox::information(this,
                                     "Record Already exists",
                                     "That substrate already exists in table");

            return true;
        }
    }

    return false;
}

/**
 * Override of reject function (operation made on closing window)
 * emiting exitSignal allows to go back to Menu
 */
void BiogasCalculator::reject()
{
    emit exitSignal();
    QDialog::reject();
}


/**
 * Configures Tables (sets Behavior, allocates data etc.)
 * @param table_ptr - pointer to table that will be configured
 */
void BiogasCalculator::mConfigureTable(QTableView *table_ptr)
{
    if(table_ptr == ui->tableView_available_substrates)
    {
        table_ptr->setSelectionBehavior(QTableView::SelectRows);

        m_model_substrates_available_ptr = std::unique_ptr<QSqlQueryModel>(new QSqlQueryModel());

    }
    else if(table_ptr == ui->tableView_chosen_substrates)
    {

        table_ptr->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table_ptr->setSelectionBehavior(QTableView::SelectRows);

    }
}

/**
 * Loads Table Appearance (Column width, and Column Headers)
 * @param table_ptr - pointer to table which will change its appearance
 */
void BiogasCalculator::mLoadTableAppearance(QTableView *table_ptr)
{
    if(table_ptr == ui->tableView_available_substrates)
    {
        assert(m_model_substrates_available_ptr);

        m_model_substrates_available_ptr->setHeaderData(0, Qt::Horizontal, "ID");
        m_model_substrates_available_ptr->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
        m_model_substrates_available_ptr->setHeaderData(2, Qt::Horizontal, "oTS (%)");
        m_model_substrates_available_ptr->setHeaderData(3, Qt::Horizontal, QObject::tr("Biogas"));
        m_model_substrates_available_ptr->setHeaderData(4, Qt::Horizontal, QObject::tr("Methane") + " (%)");


        table_ptr->setColumnWidth(0,20);     //ID
        table_ptr->setColumnWidth(2,60);     //oTS
        table_ptr->setColumnWidth(3,100);    //biogas
        table_ptr->setColumnWidth(4,100);    //methane

    }

    else if(table_ptr == ui->tableView_chosen_substrates)
    {
        assert(m_model_substrates_picked_ptr);

        m_model_substrates_picked_ptr->setHeaderData(0, Qt::Horizontal, "ID");
        m_model_substrates_picked_ptr->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
        m_model_substrates_picked_ptr->setHeaderData(2, Qt::Horizontal, "oTS (%)");
        m_model_substrates_picked_ptr->setHeaderData(3, Qt::Horizontal, QObject::tr("Biogas"));
        m_model_substrates_picked_ptr->setHeaderData(4, Qt::Horizontal, QObject::tr("Methane") + " (%)");
        m_model_substrates_picked_ptr->setHeaderData(5, Qt::Horizontal, QObject::tr("Amount"));
        m_model_substrates_picked_ptr->setHeaderData(6, Qt::Horizontal, "TS (%)");

        table_ptr->setColumnWidth(0,20);     //ID
        table_ptr->setColumnWidth(2,60);     //oTS
        table_ptr->setColumnWidth(3,100);    //biogas
        table_ptr->setColumnWidth(4,100);    //methane
        table_ptr->setColumnWidth(5,80);     //amount
        table_ptr->setColumnWidth(6,60);     //Ts
    }
}

/**
 * Blocks Window Functions
 * This functions is made in case of not allowed behaviors
 */
void BiogasCalculator::mBlockWindow()
{
    ui->comboBox_pick_plant->setDisabled(true);

    ui->lineEdit_substrate_amount->setDisabled(true);
    ui->lineEdit_TS->setDisabled(true);

    ui->pushButton_clear_all->setDisabled(true);
    ui->pushButton_remove_substrate->setDisabled(true);
    ui->pushButton_select_substrate->setDisabled(true);
}


/**
 * Handles situation when no Plants are assigned to user or unavailable
 * shows Warning and Blocks whole window with mBlockWindow()
 */
void BiogasCalculator::noPlantsHandler()
{
    QMessageBox::warning(this,
                         QObject::tr("Service Unavailable"),
                         QObject::tr("No Plants are available"));

    mBlockWindow();
}


