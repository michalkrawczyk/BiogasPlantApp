#include "GUI/Inc/menu.h"
#include "Database/Inc/database.h"
#include "Misc/Inc/layouts.h"
#include "GUI/Inc/login.h"

#include <QSqlDatabase>
#include <QApplication>
#include <memory>

auto dbp = database::createSQLiteDatabase("H:\\Databases",
                                          "db.sqlite3");
std::unique_ptr<Menu> main_menu;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    layout::darkTheme();

    main_menu = std::unique_ptr<Menu>(new Menu);

    Login w(nullptr, dbp.get());
    dbp->initDb();

    w.show();

    return a.exec();
}
