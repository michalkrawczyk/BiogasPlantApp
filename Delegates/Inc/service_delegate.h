#ifndef SERVICE_DELEGATE_H
#define SERVICE_DELEGATE_H

#include <QSqlQueryModel>
#include <memory>

namespace sqlModels {

    class ServiceTableAttorney;

    class ServiceTable final: public QSqlQueryModel
    {
        Q_OBJECT

    public:
        ServiceTable(QObject *parent_ptr = nullptr);

        QVariant data(const QModelIndex &item, int role) const override;

    private:
        std::shared_ptr<ServiceTableAttorney> attorney_ptr;
    };

    class ServiceTableAttorney final
    {
        std::vector<int> selected_rows;
        friend class ServiceTable;
    };

} //namespace sqlModels


#endif // ServiceSQLModel
