#include <QApplication>
#include <QTextCodec>
#include <QFile>
#include <QDebug>
#include "MessageBox.h"
#include "MainWidget.h"
#include "config.h"
#include "MainWindow.h"



int main(int argc, char* argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);
    //加载css
    QFile file(":/ui/css/lightblue.css");
    if(file.open(QFile::ReadOnly))
    {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        app.setPalette(QPalette(QColor(paletteColor)));
        app.setStyleSheet(qss);
        file.close();
    }
    else
    {
        qDebug() << file.errorString();
    }

    app.setFont(QFont(FontName, FontSize));

    MainWidget* w = new MainWidget;
    MainWindow* mainWindow = new MainWindow(w);
    w->setMainWindow(mainWindow);
    w->show();

    return app.exec();
}
