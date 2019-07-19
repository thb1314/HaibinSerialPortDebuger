#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QMouseEvent>
#include <QAction>
#include <QFontDatabase>
#include <QMainWindow>

#include "MainWidget.h"
#include "Settings.h"
#include "config.h"

void MainWidget::removeFrame()
{
    //设置Qt window
    Qt::WindowFlags flag = this->windowFlags();
    flag &= ~(Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint |
              Qt::WindowCloseButtonHint | Qt::WindowFullscreenButtonHint |
              Qt::WindowContextHelpButtonHint);
    flag |= (Qt::FramelessWindowHint |
             Qt::WindowSystemMenuHint |
             Qt::WindowMinMaxButtonsHint);
    this->setWindowFlags(flag);
//    qDebug() << flag;
}

MainWidget::MainWidget(QWidget* parent) : QDialog(parent)
{
    is_mined = false;
    record_mousePressed = false;
    record_mousePoint = QPoint(0, 0);
    mainWindow = NULL;
    //去掉边框
    this->removeFrame();
    //设置可以拖拽设置大小
    this->setSizeGripEnabled(true);
    //设置鼠标拖动生效
    this->setMouseTracking(true);
    //自定义标题栏
    this->setObjectName(QString::fromUtf8("QUIWidget"));
    this->resize(900, 750);
    this->initWidget();


    qSetting.beginGroup("MyGeneral");
    QVariant is_init = qSetting.value("is_init");
    qSetting.endGroup();
    //qDebug() << is_init.toString();
    if(is_init.toString() != "ok")
    {
        //初始化参数 默认参数在config.h中
        //qDebug() << "init arg";
        Settings::initSettings();
    }

}


void MainWidget::initWidget(void)
{
    QVBoxLayout* vLayoutParent = new QVBoxLayout(this);
    vLayoutParent->setSpacing(0);
    vLayoutParent->setObjectName(QString::fromUtf8("vLayoutParent"));
    vLayoutParent->setContentsMargins(0, 0, 0, 0);

    //创建标题 和主窗口容器
    widgetTitle = new QWidget(this);
    widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));

    widgetMain = new QWidget(this);
    widgetMain->setObjectName(QString::fromUtf8("widgetMain"));

    QVBoxLayout* vLayoutMain = new QVBoxLayout(widgetMain);
    vLayoutMain->setSpacing(0);
    vLayoutMain->setObjectName(QString::fromUtf8("vLayoutMain"));
    vLayoutMain->setContentsMargins(0, 0, 0, 0);



    //设置window_title的大小策略
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
    widgetTitle->setSizePolicy(sizePolicy);
    widgetTitle->setMinimumSize(QSize(0, 30));


    // 将WindowTitle 水平布局
    QHBoxLayout* hLayoutTitle = new QHBoxLayout(widgetTitle);
    hLayoutTitle->setSpacing(0);
    hLayoutTitle->setContentsMargins(0, 0, 0, 0);

    //创建Icon
    titleIcon = new QLabel(widgetTitle);
    titleIcon->setObjectName(QString::fromUtf8("titleIcon"));
    sizePolicy.setHorizontalPolicy(QSizePolicy::Minimum);
    sizePolicy.setVerticalPolicy(QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(titleIcon->sizePolicy().hasHeightForWidth());
    titleIcon->setSizePolicy(sizePolicy);
    titleIcon->setMinimumSize(QSize(30, 0));
    titleIcon->setAlignment(Qt::AlignCenter);
    hLayoutTitle->addWidget(titleIcon);

    centralTitle = new QLabel(this);
    centralTitle->setObjectName(QString::fromUtf8("centralTitle"));
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(centralTitle->sizePolicy().hasHeightForWidth());
    centralTitle->setSizePolicy(sizePolicy);
    centralTitle->setAlignment(Qt::AlignLeading | Qt::AlignCenter);
    centralTitle->setText("海滨串口调试助手");
    centralTitle->setMouseTracking(true);
    hLayoutTitle->addWidget(centralTitle);


    QWidget* widgetMenu = new QWidget(widgetTitle);
    widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
    sizePolicy.setHorizontalPolicy(QSizePolicy::Minimum);
    sizePolicy.setVerticalPolicy(QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
    widgetMenu->setSizePolicy(sizePolicy);
    QHBoxLayout* menu_hLayout = new QHBoxLayout(widgetMenu);
    menu_hLayout->setObjectName(QString::fromUtf8("menu_hLayout"));
    menu_hLayout->setSpacing(0);
    menu_hLayout->setContentsMargins(0, 0, 0, 0);

    /*
    titleMenuBtn = new QToolButton(widgetMenu);
    titleMenuBtn->setObjectName(QString::fromUtf8("titleMenuBtn"));
    sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(titleMenuBtn->sizePolicy().hasHeightForWidth());
    titleMenuBtn->setSizePolicy(sizePolicy);
    titleMenuBtn->setMinimumSize(QSize(30, 0));
    titleMenuBtn->setMaximumSize(QSize(30, 16777215));
    titleMenuBtn->setFocusPolicy(Qt::NoFocus);
    titleMenuBtn->setPopupMode(QToolButton::InstantPopup);
    menu_hLayout->addWidget(titleMenuBtn);
    */

    minBtn = new QPushButton(widgetMenu);
    minBtn->setObjectName(QString::fromUtf8("minBtn"));
    sizePolicy.setHorizontalPolicy(QSizePolicy::Minimum);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(minBtn->sizePolicy().hasHeightForWidth());
    minBtn->setSizePolicy(sizePolicy);
    minBtn->setMinimumSize(QSize(30, 0));
    minBtn->setMaximumSize(QSize(30, 16777215));
    minBtn->setCursor(QCursor(Qt::ArrowCursor));
    minBtn->setFocusPolicy(Qt::NoFocus);
    menu_hLayout->addWidget(minBtn);

    maxBtn = new QPushButton(widgetMenu);
    maxBtn->setObjectName(QString::fromUtf8("maxBtn"));
    sizePolicy.setHorizontalPolicy(QSizePolicy::Minimum);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(maxBtn->sizePolicy().hasHeightForWidth());
    maxBtn->setSizePolicy(sizePolicy);
    maxBtn->setMinimumSize(QSize(30, 0));
    maxBtn->setMaximumSize(QSize(30, 16777215));
    maxBtn->setCursor(QCursor(Qt::ArrowCursor));
    maxBtn->setFocusPolicy(Qt::NoFocus);
    menu_hLayout->addWidget(maxBtn);

    closeBtn = new QPushButton(widgetMenu);
    closeBtn->setObjectName(QString::fromUtf8("closeBtn"));
    sizePolicy.setHorizontalPolicy(QSizePolicy::Minimum);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(closeBtn->sizePolicy().hasHeightForWidth());
    closeBtn->setSizePolicy(sizePolicy);
    closeBtn->setMinimumSize(QSize(30, 0));
    closeBtn->setMaximumSize(QSize(30, 16777215));
    closeBtn->setCursor(QCursor(Qt::ArrowCursor));
    closeBtn->setFocusPolicy(Qt::NoFocus);
    menu_hLayout->addWidget(closeBtn);

    hLayoutTitle->addWidget(widgetMenu);



    //添加标题 和主窗口容器 到vLayout
    vLayoutParent->addWidget(widgetTitle);
    vLayoutParent->addWidget(widgetMain);

    QObject::connect(closeBtn, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(maxBtn, &QPushButton::clicked, [ = ]()
    {
        if(this->isMaximized())
        {
            this->maxBtn->setText(IconMax);
            this->showNormal();
        }
        else
        {
            this->maxBtn->setText(IconNormal);

            this->showMaximized();
        }
    });
    QObject::connect(minBtn, SIGNAL(clicked()), this, SLOT(showMinimized()));


    centralTitle->installEventFilter(this);


    //设置文字和图标
    //设置图形字体
    //初始化 font
    int fontId = QFontDatabase::addApplicationFont(":/ui/css/fontawesome-webfont.ttf");
    QStringList fontName = QFontDatabase::applicationFontFamilies(fontId);

    if(fontName.count() > 0)
    {
        iconFont = QFont(fontName.at(0));
    }
    else
    {
        qDebug() << "load fontawesome-webfont.ttf error";
    }
    iconFont.setPointSize(FontSize);


    this->maxBtn->setFont(iconFont);
    this->maxBtn->setText(IconMax);

    this->minBtn->setText(IconMin);
    this->minBtn->setFont(iconFont);

    this->closeBtn->setFont(iconFont);
    this->closeBtn->setText(IconClose);
    /*
    //添加换肤菜单
    QStringList theme_names;
    theme_names << "银色" << "蓝色" << "浅蓝色" << "深蓝色" << "灰色" << "浅灰色" << "深灰色" << "黑色"
                << "浅黑色" << "深黑色" << "PS黑色" << "黑色扁平" << "白色扁平";

    foreach(QString theme_name, theme_names)
    {
        QAction* action = new QAction(theme_name, this);
        this->titleMenuBtn->addAction(action);
        //connect(action, SIGNAL(triggered(bool)), this, SLOT(changeStyle()));
    }
    //this->titleMenuBtn->hide();
    */


}
bool MainWidget::eventFilter(QObject* obj, QEvent* ev)
{
    if(obj == this->centralTitle)
    {
        QMouseEvent* event = static_cast<QMouseEvent*>(ev);
        QEvent::Type type = event->type();
        switch(type)
        {
            case QEvent::MouseButtonDblClick:
                this->maxBtn->click();
                return true;
                break;
            case QEvent::MouseButtonPress:
                if(event->button() == Qt::LeftButton)
                {
                    //qDebug() << "Qt::LeftButton Pressed";
                    //qDebug() << event->globalPos();
                    //qDebug() << this->pos();
                    record_mousePressed = true;
                    record_mousePoint = event->globalPos() - this->pos();
                }
                event->ignore();
                return true;
                break;
            case QEvent::MouseButtonRelease:
                if(event->button() == Qt::LeftButton)
                {
                    //qDebug() << "Qt::LeftButton Released";
                    //qDebug() << event->globalPos();
                    //qDebug() << this->pos();
                    //qDebug() << centralTitle->geometry().topLeft();
                    record_mousePressed = false;
                }
                event->ignore();
                return true;
                break;
            case QEvent::MouseMove:

                if(this->record_mousePressed)
                {
                    if(this->isMaximized())
                    {
                        //this->showNormal();
                        //qDebug() << "record_mousePoint" << record_mousePoint;
                        //record_mousePressed = false;
                    }
                    else
                    {
                        //qDebug() << "event->globalPos()" << event->globalPos();
                        //qDebug() << "record_mousePoint" << record_mousePoint;
                        this->move(event->globalPos() - record_mousePoint);
                    }
                    event->ignore();
                    return true;

                }

                break;
            default:
                break;
        }

    }
    return QDialog::eventFilter(obj, ev);
}

bool MainWidget::setMainWindow(QMainWindow* mainWindow)
{
    if(NULL == this->mainWindow)
    {
        //将子窗体添加到布局
        this->widgetMain->layout()->addWidget(mainWindow);
        //自动设置大小
        resize(mainWindow->width(), mainWindow->height() + this->widgetTitle->height());
        this->mainWindow = mainWindow;
        return true;
    }
    return false;
}

void MainWidget::keyPressEvent(QKeyEvent* e)
{
    if(!e->modifiers() || (e->modifiers() & Qt::KeypadModifier && e->key() == Qt::Key_Enter))
    {
        switch(e->key())
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
                //reject();
                break;
            default:
                e->ignore();
                return;
        }
    }
    else
    {
        QDialog::keyPressEvent(e);
    }
}


