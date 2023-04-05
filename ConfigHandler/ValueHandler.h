#pragma once

#include <QVariant>
#include <QKeySequence>

class ValueHandler {
public:
    virtual ~ValueHandler();

    virtual bool Check(const QVariant &val) = 0;

    virtual QVariant Value(const QVariant &val);

    virtual QVariant FallBack();

    virtual QVariant Representation(const QVariant &val);

    virtual QString Expected();

protected:
    virtual QVariant Process(const QVariant &val);
};

class String : public ValueHandler {
public:
    explicit String(QString def);

    bool Check(const QVariant &val) override;

    QVariant FallBack() override;

    QString Expected() override;

private:
    QString m_def;
};

class ExistingDir : public ValueHandler {
    bool Check(const QVariant &val) override;

    QVariant FallBack() override;

    QString Expected() override;
};

class KeySequence : public ValueHandler {
public:
    KeySequence(const QKeySequence &fallback = {});

    bool Check(const QVariant &val) override;

    QVariant FallBack() override;

    QString Expected() override;

    QVariant Representation(const QVariant &val) override;

private:
    QKeySequence m_fallback;

    QVariant Process(const QVariant &val) override;
};
