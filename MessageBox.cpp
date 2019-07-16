#include <QLabel>
#include <QPushButton>
#include <QFontDatabase>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QFrame>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>

#include "config.h"
#include "MessageBox.h"



MessageBox::MessageBox(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    this->infotext = NULL;
    this->labico = NULL;
    this->confirm_button = NULL;
    this->cancel_button = NULL;
    this->centralTitle = NULL;
    this->record_mousePressed = false;
    this->record_mousePoint = QPoint(0, 0);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->initWidget();
    this->setObjectName(QString::fromUtf8("MessageBox"));
    this->resize(280, 150);

}

int MessageBox::warning(QWidget* parent, const QString& title, const QString& text)
{
//    MessageBox tmp(parent);
    QApplication::beep();
    MessageBox* messageBox_ptr = new MessageBox(parent);
    messageBox_ptr->setWindowTitle(title);
    messageBox_ptr->setInfoText(text);
    messageBox_ptr->setModal(true);
    messageBox_ptr->setInfoIcoType(MessageBox::Warning);
    messageBox_ptr->cancel_button->setHidden(true);
    int ret = messageBox_ptr->exec();
    messageBox_ptr->deleteLater();
    return ret;
}

int MessageBox::info(QWidget* parent, const QString& title, const QString& text)
{
    MessageBox* messageBox_ptr = new MessageBox(parent);
    messageBox_ptr->setWindowTitle(title);
    messageBox_ptr->setInfoText(text);
    messageBox_ptr->setModal(true);
    messageBox_ptr->setInfoIcoType(MessageBox::Information);
    messageBox_ptr->cancel_button->setHidden(true);
    int ret = messageBox_ptr->exec();
    messageBox_ptr->deleteLater();
    return ret;
}

int MessageBox::about(QWidget* parent, const QString& title, const QString& text)
{
    MessageBox* messageBox_ptr = new MessageBox(parent);
    messageBox_ptr->setWindowTitle(title);
    messageBox_ptr->setInfoText(text);
    messageBox_ptr->setModal(true);
    messageBox_ptr->setInfoIcoType(MessageBox::NoIcon);
    messageBox_ptr->cancel_button->setHidden(true);
    int ret = messageBox_ptr->exec();
    messageBox_ptr->deleteLater();
    return ret;
}

bool MessageBox::eventFilter(QObject* obj, QEvent* ev)
{
    if(obj == this->centralTitle)
    {
        QMouseEvent* event = static_cast<QMouseEvent*>(ev);
        QEvent::Type type = ev->type();
        switch(type)
        {
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

void MessageBox::initWidget()
{
    QVBoxLayout* vLayoutParent = new QVBoxLayout(this);
    vLayoutParent->setSpacing(0);
    vLayoutParent->setObjectName(QString::fromUtf8("msgLayoutParent"));
    vLayoutParent->setContentsMargins(1, 1, 1, 1);

    //创建标题 和主窗口容器
    QWidget* widgetTitle = new QWidget(this);
    widgetTitle->setObjectName(QString::fromUtf8("msgWidgetTitle"));

    QWidget* widgetMain = new QWidget(this);
    widgetMain->setObjectName(QString::fromUtf8("widgetMain"));

    QVBoxLayout* vLayoutMain = new QVBoxLayout(widgetMain);
    vLayoutMain->setSpacing(5);
    vLayoutMain->setObjectName(QString::fromUtf8("vLayoutMain"));
    vLayoutMain->setContentsMargins(5, 5, 5, 5);

    QFrame* frame = new QFrame(widgetMain);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Sunken);
    vLayoutMain->addWidget(frame);

    QVBoxLayout* frame_layout = new QVBoxLayout(frame);
    QHBoxLayout* infoLayout = new QHBoxLayout();
    this->infotext = new QLabel(frame);
    this->labico = new QLabel(frame);
    this->labico->setObjectName(QString::fromUtf8("labIcoMsg"));
    this->labico->setMinimumSize(QSize(30, 30));
    this->labico->setMaximumSize(QSize(30, 30));

    infoLayout->addWidget(this->labico);
    infoLayout->addWidget(this->infotext);
    frame_layout->addLayout(infoLayout);
    frame_layout->addStretch(1);

    QHBoxLayout* frame_layout_hLayout = new QHBoxLayout();

    frame_layout_hLayout->setSpacing(0);
    frame_layout_hLayout->setContentsMargins(1, 1, 1, 1);
    frame_layout_hLayout->addStretch(1);
    this->confirm_button = new QPushButton("确认", frame);
    this->cancel_button = new QPushButton("取消", frame);
    frame_layout_hLayout->addWidget(this->confirm_button);
    frame_layout_hLayout->addSpacing(2);
    frame_layout_hLayout->addWidget(this->cancel_button);
    frame_layout->addLayout(frame_layout_hLayout);
    QObject::connect(this->confirm_button, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(this->cancel_button, SIGNAL(clicked()), this, SLOT(reject()));
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
    QLabel* titleIcon = new QLabel(widgetTitle);
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

    this->centralTitle = new QLabel(this);
    centralTitle->setObjectName(QString::fromUtf8("centralTitle"));
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(centralTitle->sizePolicy().hasHeightForWidth());
    centralTitle->setSizePolicy(sizePolicy);
    centralTitle->setAlignment(Qt::AlignLeading | Qt::AlignCenter);
    centralTitle->setText("Title");
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




    QPushButton* closeBtn = new QPushButton(widgetMenu);
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


    centralTitle->installEventFilter(this);


    //设置文字和图标
    //设置图形字体
    //初始化 font
    int fontId = QFontDatabase::addApplicationFont(":/ui/css/fontawesome-webfont.ttf");
    QStringList fontName = QFontDatabase::applicationFontFamilies(fontId);
    QFont iconFont;
    if(fontName.count() > 0)
    {
        iconFont = QFont(fontName.at(0));
    }
    else
    {
        qDebug() << "load fontawesome-webfont.ttf error";
    }
    iconFont.setPointSize(FontSize);



    closeBtn->setFont(iconFont);
    closeBtn->setText(IconClose);
}


bool MessageBox::setInfoText(const QString& text)
{
    if(NULL != this->infotext)
    {
        this->infotext->setText(text);
        return true;
    }
    return false;
}

void MessageBox::setInfoIcoType(const MessageBox::Icon icon)
{
    switch(icon)
    {
        case MessageBox::NoIcon:
            this->labico->setHidden(true);
            break;
        case MessageBox::Warning:
            this->labico->setStyleSheet(QString::fromUtf8("image: url(:/ui/image/msg_error.png);"));
            this->labico->setHidden(false);
            break;
        case MessageBox::Information:
            this->labico->setStyleSheet(QString::fromUtf8("image: url(:/ui/image/msg_info.png);"));
            this->labico->setHidden(false);
            break;
        case MessageBox::Question:
            this->labico->setStyleSheet(QString::fromUtf8("image: url(:/ui/image/msg_question.png);"));
            this->labico->setHidden(false);
            break;
        case MessageBox::Critical:
            this->labico->setStyleSheet(QString::fromUtf8("border-image: url(:/ui/image/msg_info.png);"));
            this->labico->setHidden(false);
            break;
        default:
            break;
    }
}

void MessageBox::setWindowTitle(const QString& title)
{
    this->centralTitle->setText(title);
}




