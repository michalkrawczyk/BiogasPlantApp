#ifndef PHONETABLEDELEGATE_H
#define PHONETABLEDELEGATE_H
#include <QItemDelegate>
#include <QLineEdit>


namespace delegate
{
    class PhoneTableDelegate: public QItemDelegate
    {
        Q_OBJECT
    public:
        PhoneTableDelegate(QObject *parent = nullptr);

    protected:
        QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    };
}//namaspace delegate



#endif // PHONETABLEDELEGATE_H
