#include "Exceptions\Common\Inc\qruntimeerror.h"
#include <QMessageBox>

namespace QError
{

QRuntimeError::QRuntimeError(const QString &msg):
    m_msg(msg)
{
}

QRuntimeError *QRuntimeError::clone() const
{
    return new QRuntimeError(*this);
}

void QRuntimeError::raise() const
{
    throw *this;
}

const char *QRuntimeError::what() const noexcept
{
    return m_msg.toLocal8Bit().data();
}

/**
  * @brief Shows Exception as Warning Window
  * @param parent_ptr - pointer to parent Widget of object (for QDialog constructor)
  * @param title - Text, which will be displayed as title of window
  */
void QRuntimeError::showWarningWindow(QWidget *parent_ptr, const QString &title) const
{
    QMessageBox::warning(parent_ptr,
                         title,
                         m_msg);
}

}//namespace QErrors
