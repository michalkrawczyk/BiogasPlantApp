#include "../Inc/layouts.h"
#include <QApplication>
#include <QStyleFactory>
#include <QColor>
#include <QPalette>


namespace layout {

/**
 * Sets dark theme on App with green highlights
 */
    void darkTheme()
    {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette p = qApp->palette();

        p.setColor(QPalette::Window, QColor(53, 53, 53));
        p.setColor(QPalette::Button, QColor(53, 53, 53));
        p.setColor(QPalette::Highlight, QColor(34, 139, 34));
        p.setColor(QPalette::ButtonText, QColor(255, 255, 255));
        p.setColor(QPalette::WindowText, QColor(192, 192, 192));
        p.setColor(QPalette::ToolTipBase, QColor(34, 139, 34));
        p.setColor(QPalette::Base, QColor(60, 60, 60));
        p.setColor(QPalette::Text, QColor(255, 255, 255));


        qApp->setPalette(p);
    }


}//namespace layout
