#include "Misc/Inc/validators.h"

#include <QRegularExpression>

namespace validator{

    /**
     * Check if given string is an e-mail
     *
     * @param email - word that will be checked
     * @return true if 'email' meets criteria to be considered as e-mail
     */
    bool isEmail(const QString &email)
    {
        if (email.length() == 0)
            return false;

        QRegularExpression regex("^[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}$");
        return regex.match(email).hasMatch();
    }

    /**
    * Check if given string is an address number
    *
    * @param addr_number - word that will be checked
    * @return true if 'addr_number' meets criteria to be considered as address number
    */
    bool isAddressNumber(const QString &addr_number)
    {
        if (addr_number.length() == 0)
            return false;

        QRegularExpression regex("^[0-9]*");
        return regex.match(addr_number).hasMatch();
    }

    /**
    * Check if given string is a phone_number
    *
    * @param phone_number - word that will be checked
    * @return true if 'phone_number' meets criteria to be considered as phone
    */
    bool isPhoneNumber(const QString &phone_number)
    {
        if (phone_number.length() == 0)
            return false;

//        QRegularExpression regex("^[0-9]{9,10}$"); //Poland

        QRegularExpression regex("^\\+?[1-9][0-9]{4,15}$"); //Acording to E.164 Standard
        return regex.match(phone_number).hasMatch();
    }

    /**
    * Check if given variable can be considered as empty string or containing whitespaces
    *
    * @param value - variable that will be checked
    * @return true if value is empty or contains whitespace
    */
    bool isEmptyOrWhitespace(const QVariant &value)
    {
        if(!value.canConvert(QMetaType::QString))
            throw std::invalid_argument("Value cannot be converted to QString");

        QString word(value.toString());

        if (word.length() == 0)
            return true;

        QRegularExpression regex("^\\s*$");
        return regex.match(word).hasMatch();
    }

    /**
    * Check if given variable can be considered as positive double (>0.0)
    *
    * @param value - variable that will be checked
    * @return true if value is a positive double number
    */
    bool isPositiveDouble(const QVariant &value)
    {
        if(!value.canConvert(QMetaType::Double))
            return false;

        return value.toDouble() > 0;
    }

    /**
     * Check if given variable can be considered as positive percantage value (0-100%)
     *
     * @param value - variable that will be checked
     * @return true if value is a positive percentage value
     */
    bool isPositivePercentage(const QVariant &value)
    {
        return isPositiveDouble(value) && (value.toDouble() < 100);
    }

    /**
     * Check if given variable can be considered as string what begins with whitespace
     *
     * @param value - variable that will be checked
     * @return true if value begins with whitespace
     */
    bool beginsWithWhitespace(const QVariant &value)
    {
        if(!value.canConvert(QMetaType::QString))
            throw std::invalid_argument("Value cannot be converted to QString");


        QString word(value.toString());

        if (word.length() == 0)
            return false;

        QRegularExpression regex("^\\s");
        return regex.match(word).hasMatch();
    }

}//namespace validator

//Some functions takes as argument QVariant and some QString. Both aproaches have their pros and cons so it has been left to consider
