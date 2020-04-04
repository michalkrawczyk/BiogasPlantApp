#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <QString>
#include <QVariant>

namespace validator{
    bool isEmail(const QString &email);
    bool isAddressNumber(const QString &addr_number);
    bool isPhoneNumber (const QString &phone_number);

    bool isEmptyOrWhitespace(const QVariant &value);
    bool isPositiveDouble(const QVariant &value);
    bool isPositivePercentage(const QVariant &value);
    bool beginsWithWhitespace(const QVariant &value);

}// namespace validator

#endif // VALIDATORS_H
