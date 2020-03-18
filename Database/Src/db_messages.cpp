#include "Database/Inc/db_messages.h"

#include <QInputDialog>


namespace db_msg{

/**
 * Shows window informing that critical error with database has occured
 *
 * @param error_msg - Error Message
 * @param *parent - pointer to parent QWidget, leave nullptr if there's no parent
 * @param optoinal_msg - Optional Message to show if exist
 * @return nothing
 */
void showDbCriticalError(const QString &error_msg,
                         QWidget *parent,
                         const QString &optional_msg)
{
    QMessageBox::critical(parent,
                          QObject::tr("Database Error"),
                          error_msg + '\n' + optional_msg,
                          QMessageBox::Abort);
}


/**
 * Shows Window with Input to set new value for variable 'var'
 *
 * @param var_name - Name of Variable to display in Window
 * @param var - Variable to overwrite
 * @param *parent - Pointer to parent QWidget, leave nullptr if there's no parent
 */
bool showInputDialogText(const QString &var_name,
                         QString &var,
                         QWidget *parent)
{
    bool result(false);

    QString text = QInputDialog::getText(parent,
                                QObject::tr("Set Variable"),
                                var_name,
                                QLineEdit::Normal,
                                var,
                                &result);
    if(result)
    {
        var = text;
    }

    return result;
}

/**
 * Shows Window with Input to set new value for variable 'var'
 *
 * @param var_name - Name of Variable to display in Window
 * @param var - Variable to overwrite
 * @param min - Minimal available value to set
 * @param max - Maximal available value to set
 * @param step - Step size
 * @param *parent - Pointer to parent QWidget, leave nullptr if there's no parent
 */
bool showInputDialogInt(const QString &var_name,
                        int &var,
                        int min, int max, int step,
                        QWidget *parent)
{
    bool result(false);

    int number = QInputDialog::getInt(parent,
                                      QObject::tr("Set Variable"),
                                      var_name,
                                      var,
                                      min,
                                      max,
                                      step,
                                      &result);

    if(result)
    {
        var = number;
    }

    return result;
}

}//namespace db_msg
