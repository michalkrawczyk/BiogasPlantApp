#include "Misc/Inc/utils.h"

#include <QByteArray>
#include <QCryptographicHash>

namespace encoding{
/**
 * Encrypt word with MD5 algorithm
 *
 * @param word - word to encrypt
 * @return encoded word as QString
 */
    QString MD5(const QString &word)
    {
        QByteArray byte_array(QString("a6b9" + word + "x7d8").toUtf8() );
        return QString(QCryptographicHash::hash(byte_array, QCryptographicHash::Md5).toHex() );
    };

}//namespace encoding
