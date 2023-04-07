#pragma once
#include <QDebug>
#include <QDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "Test.hpp"

class MyTest : public QWidget {
    Q_OBJECT

public:
    MyTest(QWidget *parent = nullptr): QWidget(parent)
    {
        auto *layout = new QVBoxLayout(this);
        auto *btn = new QPushButton;
        btn->setText("button");
        btn->setEnabled(!ConfigHandler().HasError());
        layout->addWidget(btn);
    }
};

class MyShortCut : public QDialog {
    Q_OBJECT

public:
    MyShortCut(QWidget *parent = nullptr): QDialog(parent)
    {
        auto *layout = new QVBoxLayout(this);
        auto *label = new QLabel;
        layout->addWidget(label);
        connect(this, &MyShortCut::HadSet, [this, label]() {
            const QString text = m_ks.toString();
            ConfigHandler().SetShortCut(text);
            label->setText(text);
        });
    }

    const QKeySequence& ShortCut();

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

private:
    QString m_modifier;

    QKeySequence m_ks;

signals:
    void HadSet();
};

inline const QKeySequence& MyShortCut::ShortCut()
{
    return m_ks;
}

inline void MyShortCut::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier) {
        m_modifier += "Shift+";
    }
    if (event->modifiers() & Qt::ControlModifier) {
        m_modifier += "Ctrl+";
    }
    if (event->modifiers() & Qt::AltModifier) {
        m_modifier += "Alt+";
    }
    if (event->modifiers() & Qt::MetaModifier) {
        m_modifier += "Meta+";
    }

    const QString key = QKeySequence(event->key()).toString();
    m_ks = QKeySequence(m_modifier + key);
}

inline void MyShortCut::keyReleaseEvent(QKeyEvent *event)
{
    if (m_ks == QKeySequence(Qt::Key_Escape)) {
        reject();
    }
    emit HadSet();
}

namespace Test {
    inline void Test1()
    {
        ConfigHandler().SetName("custom name");

        const QString name = ConfigHandler().name();
        qDebug().noquote() << name;
    }

    inline void Test2()
    {
        // Value�л��Ȼ�ȡhandler��Ȼ����path�Ƿ�Ϸ�����󷵻�һ��Ĭ��ֵ
        const QString path = ConfigHandler().path();
        qDebug().noquote() << path;
    }

    inline void Test3()
    {
        /*
         * 1. GetInstance��Ҫ���ڰ��źŲ�
         * 2. ͨ��SetErrorState���õ�ǰ״̬����һ��״̬���бȽϣ�Ȼ�󷢳���ͬ�ź�
         */
        QObject::connect(ConfigHandler::GetInstance(), &ConfigHandler::Error, []() {
            qDebug().noquote() << "get error signal";
        });
        QObject::connect(ConfigHandler::GetInstance(), &ConfigHandler::Error, []() {
            qDebug().noquote() << "get Error resolved signal";
        });
        ConfigHandler().SetErrorState(true);
        ConfigHandler().SetErrorState(false);
    }

    inline void Test4()
    {
        // �ڲ�����һ����ť���ڳ�ʼ�����޴���Ϳ��ã��д�����û�
        MyTest *myTest = new MyTest();
        myTest->show();
    }

    inline void Test5()
    {
        // MyShortCut�������µĿ�ݼ���ϣ���������ʾ����Ļ�ϣ����д�������ļ�
        MyShortCut *shortCut = new MyShortCut;
        shortCut->show();
    }
}
