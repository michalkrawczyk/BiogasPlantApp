#include "Delegates/Inc/service_delegate.h"
#include <QDate>
#include <QtDebug>
#include <QColor>

namespace sqlModels {

ServiceTable::ServiceTable(QObject *parent_ptr):
    QSqlQueryModel(parent_ptr),
    attorney_ptr(new ServiceTableAttorney)
{
}

/**
  * @brief Override of data function
  *     Changes Display of Table column
  *     For column 3 (Done) - changes 1/0 data into Yes/No
  *     For column 0 (Date) - If date has expired and in column 3 the value is "No"
  *         Changes Color Background to Red
  * @param item - Table models in QT stores data in Model Indexes
  *     This specifies which cell from table will be modified
  * @param role - Each item in the model has a set of data elements associated with it, each with its own role.
  *     The roles are used by the view to indicate to the model which type of data it needs.
  *     This roles are specified in enum Qt::ItemDataRole
  * @retval QVariant with Modified Params
  */
QVariant ServiceTable::data(const QModelIndex &item, int role) const
{
    QVariant val(QSqlQueryModel::data(item, role));

    if (val.isValid() && role == Qt::DisplayRole)
    {
        if (item.column() == 3) //Done
        {
            if((val.toInt() == 1))
            {
                val = QObject::tr("Yes");
            }
            else
            {
                val = QObject::tr("No");
                attorney_ptr->selected_rows.push_back(item.row());
            }
        }
    }

    if (role == Qt::BackgroundRole && item.column() == 0) //Date
    {
        if (val.toDate() < QDate::currentDate())
        {
            for(auto row = attorney_ptr->selected_rows.begin(); row !=attorney_ptr->selected_rows.end(); ++row)
            {
                if(*row == item.row())
                {
                    return QVariant::fromValue(QColor(180, 0, 0, 255));
                }
            }
        }
    }

    return val;
}



}//namespace sqlModels
