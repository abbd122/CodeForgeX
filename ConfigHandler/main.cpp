#include <qapplication.h>

#include "ConfigHandler.h"
#include "ValueHandler.h"

void Test1()
{
    ConfigHandler::GetInstance();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Test1();
    QApplication::exec();
}
