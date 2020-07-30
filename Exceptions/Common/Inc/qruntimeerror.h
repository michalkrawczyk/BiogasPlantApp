#ifndef QRUNTIMEERROR_H
#define QRUNTIMEERROR_H

#include <QException>
#include <QString>
#include <QWidget>

namespace QError
{

class QRuntimeError : public QException
{
public:
    QRuntimeError(const QString &msg = "Unknown");
    QRuntimeError *clone() const override;

    void raise() const override;
    const char* what() const noexcept override;

    void showWarningWindow(QWidget *parent_ptr = nullptr, const QString &title = "Runtime Error") const;

private:
    QString m_msg;
};

}//namespace QErrors

#endif // QRUNTIMEERROR_H
