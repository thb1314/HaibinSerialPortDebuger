#include <QMutexLocker>
#include <QAtomicPointer>
#include <QMutex>
#include <QDebug>
#include "Settings.h"
#include "config.h"
QAtomicPointer<Settings> Settings::instance = NULL;
QMutex Settings::mutex;

Settings& Settings::getInstance()
{
    if(instance.testAndSetOrdered(0, 0))//第一次检测
    {
        QMutexLocker locker(&mutex);//加互斥锁。

        instance.testAndSetOrdered(0, new Settings(ConfigFile, Settings::IniFormat)); //第二次检测。
        if(instance != NULL)
        {
            (*instance).setObjectName("Settings");
        }
    }

    return *instance;
}

void Settings::initSettings(void)
{
    qSetting.beginGroup("MyGeneral");
    qSetting.setValue("is_init", "ok");
    qSetting.endGroup();

    qSetting.beginGroup("Base");
    qSetting.setValue("TextColor", TextColor);
    qSetting.setValue("PanelColor", PanelColor);
    qSetting.setValue("BorderColor", BorderColor);
    qSetting.setValue("NormalColorStart", NormalColorStart);
    qSetting.setValue("DarkColorStart", DarkColorStart);
    qSetting.setValue("DarkColorEnd", DarkColorEnd);
    qSetting.setValue("HighColor", HighColor);
    qSetting.endGroup();
}
