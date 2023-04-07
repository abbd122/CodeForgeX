#include "ConfigHandler.h"

#include <qapplication.h>
#include <QDebug>
#include <QFileInfo>

#include "ValueHandler.h"

#define OPTION(KEY, TYPE)                                          \
{                                                                  \
    QStringLiteral(KEY), QSharedPointer<ValueHandler>(new TYPE)    \
}

#define SHORTCUT(NAME, DEFAULT_VALUE)                                          \
    {                                                                          \
        QStringLiteral(NAME), QSharedPointer<KeySequence>(new KeySequence(     \
                                QKeySequence(QLatin1String(DEFAULT_VALUE))))   \
    }

static QMap<QString, QSharedPointer<ValueHandler>> recognizedGeneralOptions = {
    OPTION("name", String("Config Handler")),
    OPTION("path", ExistingDir()),
    SHORTCUT("shortCut", "Ctrl+S")
};

bool ConfigHandler::m_hasErr{false};

bool ConfigHandler::m_skipNextErrorCheck{false};

bool ConfigHandler::m_errorCheckPending{true};

QSharedPointer<QFileSystemWatcher> ConfigHandler::m_configWatcher;

ConfigHandler::ConfigHandler(QObject *parent)
    : QObject(parent),
      m_settings(QSettings::IniFormat,
                 QSettings::UserScope,
                 qApp->organizationName(),
                 qApp->applicationName())
{
    static bool firstInit = true;
    if (firstInit) {
        m_configWatcher.reset(new QFileSystemWatcher);
        EnsureFileWatched();
        connect(m_configWatcher.data(), &QFileSystemWatcher::fileChanged,
                [](const QString &filepath) {
                    emit GetInstance()->FileChanged();

                    if (QFile(filepath).exists()) {
                        m_configWatcher->addPath(filepath);
                    }

                    if (m_skipNextErrorCheck) {
                        m_skipNextErrorCheck = false;
                        return;
                    }

                    GetInstance()->CheckAndHandleError();

                    if (!QFile(filepath).exists()) {
                        m_errorCheckPending = true;
                    }
                });
    }
    firstInit = false;
}

ConfigHandler* ConfigHandler::GetInstance()
{
    static ConfigHandler config;
    return &config;
}

void ConfigHandler::SetValue(const QString &key, const QVariant &value)
{
    if (!HasError()) {
        m_skipNextErrorCheck = true;
        const QVariant val = GetValueHandler(key)->Representation(value);
        m_settings.setValue(key, val);
    }
}

QVariant ConfigHandler::Value(const QString &key) const
{
    const auto val = m_settings.value(key);
    const auto handler = GetValueHandler(key);
    if (!val.isValid() || handler->Check(val)) {
        m_hasErr = true;
        qDebug().noquote() << "Invalid val: " << val;
    }
    if (m_hasErr) {
        return handler->FallBack();
    }
    return handler->Value(val);
}

void ConfigHandler::Remove(const QString &key)
{
    m_settings.remove(key);
}

void ConfigHandler::ClearSettings()
{
    for (const auto &key : m_settings.allKeys()) {
        m_settings.remove(key);
    }
    m_settings.sync();
}

void ConfigHandler::ResetValue(const QString &key)
{
    m_settings.setValue(key, GetValueHandler(key)->FallBack());
}

QSharedPointer<ValueHandler> ConfigHandler::GetValueHandler(const QString &key) const
{
    QSharedPointer<ValueHandler> handler = recognizedGeneralOptions.value(key);
    return handler;
}

QSet<QString> ConfigHandler::KeysFromGroup(const QString &group) const
{
    QSet<QString> keys;
    for (const auto &key : m_settings.allKeys()) {
        if (group == CONFIG_GROUP_GENERAL && !key.contains("/")) {
            keys.insert(key);
        } else if (key.startsWith(group + "/")) {
            keys.insert(BaseName(key));
        }
    }
    return keys;
}

QSet<QString> ConfigHandler::RecognizedGeneralKeys()
{
    auto items = ::recognizedGeneralOptions.keys();
    static QSet<QString> keys = QSet<QString>(items.begin(), items.end());
    return keys;
}

bool ConfigHandler::CheckErrors() const
{
    return CheckUnrecognizedSettings() && CheckValid();
}

void ConfigHandler::CheckAndHandleError() const
{
    if (!QFile(m_settings.fileName()).exists()) {
        SetErrorState(false);
    } else {
        SetErrorState(!CheckErrors());
    }
    EnsureFileWatched();
}

bool ConfigHandler::CheckUnrecognizedSettings(QList<QString> *offenders) const
{
    QSet<QString> generalKeys = KeysFromGroup(CONFIG_GROUP_GENERAL);
    const QSet<QString> recognizedGeneralKeys = RecognizedGeneralKeys();
    generalKeys.subtract(recognizedGeneralKeys);
    const bool res = generalKeys.isEmpty();
    if (!res) {
        for (const auto &key : generalKeys) {
            if (offenders) {
                offenders->append(key);
            }
        }
    }
    return res;
}

bool ConfigHandler::CheckValid(QList<QString> *offenders) const
{
    bool res = true;
    for (const auto &key : m_settings.allKeys()) {
        if (!RecognizedGeneralKeys().contains(key)) {
            continue;
        }
        QVariant val = m_settings.value(key);
        const QSharedPointer<ValueHandler> handler = GetValueHandler(key);
        if (val.isValid() && !handler->Check(val)) {
            res = false;
            if (offenders) {
                offenders->append(key);
            }
        }
    }
    return res;
}

void ConfigHandler::SetErrorState(bool error) const
{
    const bool hadErr = m_hasErr;
    m_hasErr = error;
    if (!hadErr && m_hasErr) {
        emit GetInstance()->Error();
    } else if (hadErr && !m_hasErr) {
        emit GetInstance()->ErrorResolved();
    }
}

QString ConfigHandler::ErrorMessage() const
{
    return tr("The configuration contains an error. Open configuration to resolve.");
}

bool ConfigHandler::HasError() const
{
    if (m_errorCheckPending) {
        CheckAndHandleError();
        m_errorCheckPending = false;
    }
    return m_hasErr;
}

void ConfigHandler::EnsureFileWatched() const
{
    QFile file(m_settings.fileName());
    if (!file.exists()) {
        file.open(QFileDevice::WriteOnly);
        file.close();
    }
    if (m_configWatcher != nullptr && m_configWatcher->files().isEmpty() &&
        qApp != nullptr
    ) {
        m_configWatcher->addPath(m_settings.fileName());
    }
}

QString ConfigHandler::BaseName(const QString &key) const
{
    return QFileInfo(key).baseName();
}
