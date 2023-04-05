#include "ValueHandler.h"

#include <QFileInfo>
#include <QStandardPaths>

ValueHandler::~ValueHandler() = default;

QVariant ValueHandler::Value(const QVariant &val)
{
    if (!val.isValid() || !Check(val)) {
        return FallBack();
    } else {
        return Process(val);
    }
}

QVariant ValueHandler::FallBack()
{
    return {};
}

QVariant ValueHandler::Representation(const QVariant &val)
{
    return val.toString();
}

QString ValueHandler::Expected()
{
    return {};
}

QVariant ValueHandler::Process(const QVariant &val)
{
    return val;
}

String::String(QString def): m_def(std::move(def))
{
}

bool String::Check(const QVariant &val)
{
    return true;
}

QVariant String::FallBack()
{
    return m_def;
}

QString String::Expected()
{
    return QStringLiteral("string");
}

bool ExistingDir::Check(const QVariant &val)
{
    if (!val.canConvert(QVariant::String) || val.toString().isEmpty()) {
        return false;
    }
    const QFileInfo fileInfo(val.toString());
    return fileInfo.isDir() && fileInfo.exists();
}

QVariant ExistingDir::FallBack()
{
    for (const auto location : {
             QStandardPaths::PicturesLocation, QStandardPaths::HomeLocation,
             QStandardPaths::TempLocation
         }) {
        QString path = QStandardPaths::writableLocation(location);
        if (QFileInfo(path).isDir()) {
            return path;
        }
    }
    return {};
}

QString ExistingDir::Expected()
{
    return QStringLiteral("existing directory");
}

KeySequence::KeySequence(const QKeySequence &fallback): m_fallback(fallback)
{
}

bool KeySequence::Check(const QVariant &val)
{
    const QString str = val.toString();
    if (!str.isEmpty() && QKeySequence(str).toString().isEmpty()) {
        return false;
    }
    return true;
}

QVariant KeySequence::FallBack()
{
    return Process(m_fallback);
}

QString KeySequence::Expected()
{
    return QStringLiteral("keyboard shortcut");
}

QVariant KeySequence::Representation(const QVariant &val)
{
    QString str(val.toString());
    if (QKeySequence(str) == QKeySequence(Qt::Key_Return)) {
        return QStringLiteral("Enter");
    }
    return str;
}

QVariant KeySequence::Process(const QVariant &val)
{
    QString str(val.toString());
    if (str == "Enter") {
        return QKeySequence(Qt::Key_Return).toString();
    }
    if (str.length() > 0) {
        const QCharRef& lastChar = str[str.length() - 1];
        str.replace(str.length() - 1, 1, lastChar.toLower());
    }
    return str;
}
