#include "Delegates\Inc\phone_table_delegate.h"
#include "Misc/Inc/validators.h"

#include <QLineEdit>
#include <QMessageBox>


namespace delegate {
    PhoneTableDelegate::PhoneTableDelegate(QObject *parent):
        QItemDelegate(parent)
    {
    }

    /**
     * Changes Behavior of editor to accept only values matching E.164 standard
     * Note: This is used for Phone Table to do not accept strings that are not phone numbers
     */
    QWidget *PhoneTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        auto *editor = new QLineEdit(parent);

        QRegularExpression regex("^\\+?[1-9]\\d{3,15}$"); //Acording to E.164 Standard
        auto *validator = new QRegularExpressionValidator(regex,parent);

        editor->setMaxLength(17);
        editor->setValidator(validator);

        return editor;
    }


    /**
     * This function applies editor to QLineEdit of given model
     * Note: It is used for Phone Table to do not accept strings that are not phone numbers
     */
    void PhoneTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        QLineEdit *line = static_cast<QLineEdit*>(editor);

        QString data = line->text();

        if(validator::isPhoneNumber(data))
        {
            model->setData(index, data);
        }
        else
        {
             QMessageBox::warning(nullptr,
                                  QObject::tr("Invalid Phone Number"),
                                  QObject::tr("Given number does not match pattern"));
        }
    }

}
