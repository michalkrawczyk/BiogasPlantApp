#include "GUI/Inc/menu.h"
#include "Database/Inc/database.h"
#include "Misc/Inc/layouts.h"
#include "GUI/Inc/login.h"

#include <QSqlDatabase>
#include <QApplication>
#include <memory>


std::unique_ptr<Menu> main_menu;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto dbp = database::createSQLiteDatabase("H:\\Databases",
                                              "db.sqlite3");

    layout::darkTheme();

    Login w(dbp);

    assert(dbp->initDb());

    w.show();

    return a.exec();
}
