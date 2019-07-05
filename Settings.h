#ifndef SETTING_H
#define SETTING_H

#include <QSettings>
#include <QMutex>
#define qSetting (Settings::getInstance())
class Settings : public QSettings
{

    Q_OBJECT
private:
    static QAtomicPointer<Settings> instance;
    static QMutex mutex;
private:
    Settings(const QString& fileName, Format format, QObject* parent = Q_NULLPTR):
        QSettings(fileName, format, parent) {}
    Settings() {} //禁止构造函数。
    Settings(const Settings&); //禁止拷贝构造函数。
    Settings& operator==(const Settings&); //禁止赋值拷贝函数
public:
    static Settings& getInstance(void);
    static void initSettings(void);
};

#endif // SETTING_H
