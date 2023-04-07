#pragma once

#include <QFileSystemWatcher>
#include <QObject>
#include <QSet>
#include <QSettings>
#include <QSharedPointer>
#include <QVariant>

#include "ValueHandler.h"

#define CONFIG_GROUP_GENERAL "General"

#define CONFIG_GETTER(KEY, TYPE)                                  \
        TYPE KEY()                                                \
        {                                                         \
            return Value(QStringLiteral(#KEY)).value<TYPE>();     \
        }

#define CONFIG_SETTER(FUNC, KEY, TYPE)                            \
        void FUNC(const TYPE& val)                                \
        {                                                         \
            QString key = QStringLiteral(#KEY);                   \
            if (QVariant::fromValue(val) != Value(key)) {         \
                SetValue(key, QVariant::fromValue(val));          \
            }                                                     \
        }

#define CONFIG_GETTER_SETTER(GETFUNC, SETFUNC, TYPE)              \
        CONFIG_GETTER(GETFUNC, TYPE)                              \
        CONFIG_SETTER(SETFUNC, GETFUNC, TYPE)

class ConfigHandler : public QObject {
    Q_OBJECT

public:
    ConfigHandler(QObject *parent = nullptr);

    static ConfigHandler* GetInstance();

    CONFIG_GETTER_SETTER(name, SetName, QString)

    CONFIG_GETTER_SETTER(path, SetPath, QString)

    CONFIG_GETTER_SETTER(shortCut, SetShortCut, QString)

    void SetValue(const QString &key, const QVariant &value);

    QVariant Value(const QString &key) const;

    void Remove(const QString &key);

    void ClearSettings();

    void ResetValue(const QString &key);

    QSet<QString> KeysFromGroup(const QString &group) const;

    static QSet<QString> RecognizedGeneralKeys();

    bool CheckErrors() const;

    void CheckAndHandleError() const;

    bool CheckUnrecognizedSettings(QList<QString> *offenders = nullptr) const;

    bool CheckValid(QList<QString> *offenders = nullptr) const;

    void SetErrorState(bool error) const;

    QString ErrorMessage() const;

    bool HasError() const;

signals:
    void Error() const;

    void ErrorResolved() const;

    void FileChanged() const;

private:
    mutable QSettings m_settings;

    static bool m_hasErr;

    static bool m_skipNextErrorCheck;

    static bool m_errorCheckPending;

    static QSharedPointer<QFileSystemWatcher> m_configWatcher;

    void EnsureFileWatched() const;

    QString BaseName(const QString &key) const;

    QSharedPointer<ValueHandler> GetValueHandler(const QString &key) const;
};
