#include <QDebug>

#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Qt::WindowFlags flags = this->windowFlags();
    //qDebug() << flags;
    //flags &= ~(Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint |
    //Qt::WindowCloseButtonHint | Qt::WindowFullscreenButtonHint);
    //flags |= Qt::FramelessWindowHint;
    ui->setupUi(this);
    ui->menuHelp->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
