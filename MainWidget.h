#ifndef MAINWIDGET_H
#define MAINWIDGET_H


#include <QDialog>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
class QLabel;
class QToolButton;
class QPushButton;
class QMainWindow;
class MainWidget : public QDialog
{
    Q_OBJECT
protected:
    void removeFrame(void);
    bool record_mousePressed;
    bool is_mined;
    QPoint record_mousePoint;
    QWidget* widgetMain;
    QWidget* widgetTitle;
    QLabel* titleIcon;
    QLabel* centralTitle;
//    QToolButton* titleMenuBtn;
    QPushButton* minBtn;
    QPushButton* maxBtn;
    QPushButton* closeBtn;
    QFont iconFont;
    void initWidget(void);

    QMainWindow* mainWindow;
public:
    explicit MainWidget(QWidget* parent = 0);
    bool eventFilter(QObject*, QEvent*);
    bool setMainWindow(QMainWindow* mainWindow);
    void keyPressEvent(QKeyEvent* e);
signals:

public slots:
};

#endif // MAINWIDGET_H
