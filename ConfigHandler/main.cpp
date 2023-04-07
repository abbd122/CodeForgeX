#include <qapplication.h>

#include "ConfigHandler.h"
#include "Test.hpp"

int main(int argc, char *argv[])
{
    // 默认会生成在%appdata%下
    QCoreApplication::setApplicationName(QStringLiteral("config_handler_app")); // 配置文件名
    QCoreApplication::setOrganizationName(QStringLiteral("config_handler")); // 所在文件夹
    QApplication app(argc, argv);
    Test::Test5();
    QApplication::exec();
}
