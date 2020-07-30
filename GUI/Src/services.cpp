#include "GUI\Inc\services.h"
#include "ui_services.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <exception>

#include "Exceptions/Common/Inc/qruntimeerror.h"

Services::Services(const unsigned int &user_id, DbSQL db_ptr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Services),
    m_db_ptr(db_ptr),
    m_user_id(user_id)
{
    ui->setupUi(this);

    if(mLoadPlants())
    {
        m_svcs_mdl_ptr.reset(new sqlModels::ServiceTable);

        if(mLoadSvcs())
        {
            mConfigSvcsTable();
        }
    }
}

Services::~Services()
{
    delete ui;
}

/**
  * @brief Plant in ComboBox changed handler.
  * On Plant picked - refresh service table to show services assigned to that plant
  * @param index - index of plant in combobox
  */
void Services::on_comboBox_plants_currentIndexChanged(int index)
{
       if(!m_plants_available.empty())
       {
           m_plant_picked = m_plants_available[(unsigned)index];
           mLoadSvcs();
       }
}

/**
  * @brief Emits exit signal at pressing "Back to Menu" button
  */
void Services::on_pushButton_menu_clicked()
{
    reject();
}

/**
  * @brief Load Plants that are available to logged in user (With using SQL statement)
  * Note: If operation will end with failure - Service Window will be blocked
  * @retval True - if operartion will complete without any failures
  */
bool Services::mLoadPlants() noexcept
{
    try
    {
        ui->comboBox_plants->clear();

        QSqlQuery qry(m_db_ptr.get()->getDatabase());

        qry.prepare("SELECT PlantID, location "
                    "FROM biogas_server_plant "
                    "WHERE owner_id = :user");
        qry.bindValue(":user", m_user_id);

        if(qry.exec())
        {
            m_plants_available.clear();

            while(qry.next())
            {
                QString temp = qry.value(0).toString() + " - " + qry.value(1).toString();
                ui->comboBox_plants->addItem(temp);

                m_plants_available.push_back(qry.value(0).toULongLong());
            }

            if(!m_plants_available.empty())
            {
                m_plant_picked = m_plants_available[0];
            }
            else
            {
                throw QError::QRuntimeError(QObject::tr("No Plants are available"));
            }
        }
    }
    catch (const QError::QRuntimeError &e)
    {
        e.showWarningWindow(this, QObject::tr("Service Unavailable"));

        mBlockWindow();

        return false;
    }
    catch (const std::exception &e)
    {
        QMessageBox::warning(this,
                             QObject::tr("Failed to Load Plants"),
                             e.what());
        mBlockWindow();

        return false;
    }

  return true;
}

/**
  * @brief Load Services that are saved for picked plant (With using SQL statement)
  * Note: If operation will end with failure - Service Window will be blocked
  * @retval True - if operartion will complete without any failures
  */
bool Services::mLoadSvcs() noexcept
{
    try
    {
        QSqlQuery qry(m_db_ptr.get()->getDatabase());

        qry.prepare("SELECT date, title, description, done, notice "
                    "FROM biogas_server_service "
                    "WHERE forPlant_id = :plant "
                    "GROUP BY date");
        qry.bindValue(":plant", m_plant_picked);

        if(qry.exec())
        {
            m_svcs_mdl_ptr->setQuery(qry);
            ui->tableView_services->setModel(m_svcs_mdl_ptr.get());
        }

        else
        {
            throw QError::QRuntimeError(qry.lastError().text());
        }
    }
    catch (const QError::QRuntimeError &e)
    {
        e.showWarningWindow(this, QObject::tr("Failed to Load Services"));

        mBlockWindow();

        return false;
    }
    catch (std::exception &e)
    {
        QMessageBox::warning(this,
                             "Failed to Load Services",
                             e.what());
        mBlockWindow();

        return false;
    }

    return true;
}

/**
  * @brief Configures Display of Table with Services
  *     - Sets name for each column
  *     - Sets width of each column
  *     - Sets height of each row
  */
void Services::mConfigSvcsTable()
{
    assert(m_svcs_mdl_ptr);

    m_svcs_mdl_ptr->setHeaderData(0, Qt::Horizontal, QObject::tr("Date"));
    m_svcs_mdl_ptr->setHeaderData(1, Qt::Horizontal, QObject::tr("Title"));
    m_svcs_mdl_ptr->setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));
    m_svcs_mdl_ptr->setHeaderData(3, Qt::Horizontal, QObject::tr("Done"));
    m_svcs_mdl_ptr->setHeaderData(4, Qt::Horizontal, QObject::tr("Notice"));

    ui->tableView_services->setColumnWidth(0, 90); //Date
    ui->tableView_services->setColumnWidth(1, 200); //Title
    ui->tableView_services->setColumnWidth(2, 400); //Description
    ui->tableView_services->setColumnWidth(3, 20); //Done
    ui->tableView_services->setColumnWidth(4, 200); //Notice

    ui->tableView_services->verticalHeader()->setDefaultSectionSize(50);
    ui->tableView_services->show();
}

/**
  * @brief Performs "Block Window" operation
  *     Disables Ui components placed in Window
  */
void Services::mBlockWindow()
{
    ui->comboBox_plants->setDisabled(true);
    ui->tableView_services->setDisabled(true);
}

/**
 * @brief Override of reject function (operation made on closing window)
 *      emiting exitSignal allows to go back to Menu
 */
void Services::reject()
{
    emit exitSignal();
    QDialog::reject();
}
