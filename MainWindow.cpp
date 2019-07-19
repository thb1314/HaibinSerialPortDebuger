#include <QDebug>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMap>
#include <QEvent>
#include <QMouseEvent>
#include <QTextCodec>
#include <QFileDialog>
#include <QTimer>
#include <QDateTime>
#include <QScrollBar>
#include <QFile>


#include "MessageBox.h"
#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), is_open_serailport(false),
    serial_port(new QSerialPort(this)), ui(new Ui::MainWindow)
{
    this->last_combox_index = 0;
    this->rcv_data_map = new QMap<quint64, QMap<SEND_TYPE, QByteArray>>();
    ui->setupUi(this);
    ui->menuHelp->setMouseTracking(true);

    initSerialPortList();
    initBautrateList();
    initDataBitList();
    initStopBitList();
    initCheckBitList();

    QObject::connect(this->ui->pB_clearLog, &QPushButton::clicked, [this]
    {
        this->rcv_data_map->clear();
    });
    QObject::connect(this->ui->actionAbout, &QAction::triggered, [this]
    {
        MessageBox::about(this, "关于", "本软件由海滨开发且开放源代码，遵循LGPL协议。");
    });
    QObject::connect(this->ui->actionAbout_QT, &QAction::triggered, &qApp->aboutQt);

    QObject::connect(this->ui->pB_saveRevData, SIGNAL(clicked()), this, SLOT(slotOnSaveData()));
    QObject::connect(this->ui->pB_saveSendData, SIGNAL(clicked()), this, SLOT(slotOnSaveData()));
    QObject::connect(this->ui->pB_saveAllData, SIGNAL(clicked()), this, SLOT(slotOnSaveData()));

    QObject::connect(this->ui->rB_rcvMode_text, &QRadioButton::toggled, [this](bool is_checked)
    {
        if(this->rcv_data_map->size() > 0)
        {
            this->ui->tE_rcvcon->clear();
            foreach(qint64 unix_timestamp, this->rcv_data_map->keys())
            {
                QString rcvStr;

                const QMap<SEND_TYPE, QByteArray>& map_data = this->rcv_data_map->value(unix_timestamp);
                const SEND_TYPE& type = map_data.firstKey();
                const QByteArray& rcv_data = map_data.value(type);
                if(type == Serial_RCV)
                {
                    rcvStr += "[收] ";
                }
                else
                {
                    rcvStr += "[发] ";
                }
                rcvStr += QDateTime::fromMSecsSinceEpoch(unix_timestamp).toString("yyyy-MM-dd hh:mm:ss zzz");
                rcvStr += "\r\n";
                if(is_checked)
                {
                    rcvStr += this->GetCorrectUnicode(rcv_data);
                }
                else
                {
                    foreach(quint8 data, rcv_data)
                    {
                        if(data < 16)
                        {
                            rcvStr += "0" + QString::number(data, 16) + " ";
                        }
                        else
                        {
                            rcvStr += QString::number(data, 16) + " ";
                        }
                    }
                }

                this->ui->tE_rcvcon->append(rcvStr);
            }
        }
    });
    QObject::connect(this->serial_port, &QSerialPort::readyRead, [this]
    {
        if(!this->is_open_serailport) return;
        const QByteArray& rcv_data = this->serial_port->readAll();
        if(rcv_data.length() <= 0) return;
        quint64 unix_timestamp = QDateTime::currentMSecsSinceEpoch();   //获取当前时间
        QMap <SEND_TYPE, QByteArray> insert_data;
        insert_data.insert(Serial_RCV, rcv_data);
        this->rcv_data_map->insert(unix_timestamp, insert_data);
        QString rcvStr("[收] ");
        rcvStr += QDateTime::fromMSecsSinceEpoch(unix_timestamp).toString("yyyy-MM-dd hh:mm:ss zzz");
        rcvStr += "\r\n";

        if(this->ui->rB_rcvMode_text->isChecked())
        {
            rcvStr += this->GetCorrectUnicode(rcv_data);
        }
        else if(this->ui->rB_rcvMode_hex->isChecked())
        {
            foreach(quint8 data, rcv_data)
            {
                if(data < 16)
                {
                    rcvStr += "0" + QString::number(data, 16) + " ";
                }
                else
                {
                    rcvStr += QString::number(data, 16) + " ";
                }
            }
        }

        this->ui->tE_rcvcon->append(rcvStr);

        this->rcv_cnt += rcv_data.length();
        this->ui->lE_rcv_cnt->setText(QString::number(this->rcv_cnt));

        QScrollBar* scrollBar = this->ui->tE_rcvcon->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    });


    QObject::connect(ui->pB_openSerail, &QPushButton::clicked, [this]
    {
        if(this->is_open_serailport)
        {
            if(serial_port->isOpen())
            {
                serial_port->close();
            }
            if(this->ui->pB_openSerail->text() == "关闭")
            {
                this->ui->pB_openSerail->setText("打开");
            }
            if(!this->ui->cB_serailport->isEnabled())
            {
                this->ui->cB_serailport->setEnabled(true);
            }

            this->is_open_serailport = false;
        }
        else
        {
            QString portname = this->ui->cB_serailport->currentText();
            if(portname.isEmpty())
            {
                return;
            }
            if(serial_port->isOpen())
            {
                serial_port->close();
            }
            bool ret = true;
            // 获取串口号
            serial_port->setPortName(portname);
            // 获取波特率
            ret &= serial_port->setBaudRate(this->ui->cB_bautrate->currentData().toInt());
            if(!ret)
            {

                qDebug() << "设置波特率失败，失败原因：" << serial_port->errorString();
                MessageBox::warning(this, "设置波特率失败", QString("原因:") + serial_port->errorString());
                return;
            }
            // 获取数据位
            QSerialPort::DataBits databits = static_cast<QSerialPort::DataBits>(this->ui->cB_databits->currentData().toInt());
            ret &= serial_port->setDataBits(databits);
            if(!ret)
            {

                qDebug() << "设置数据位失败，失败原因：" << serial_port->errorString();
                MessageBox::warning(this, "设置数据位失败", QString("原因:") + serial_port->errorString());
                return;
            }
            // 获取停止位
            QSerialPort::StopBits stopbits = static_cast<QSerialPort::StopBits>(this->ui->cB_stopbit->currentData().toInt());
            ret &= serial_port->setStopBits(stopbits);
            if(!ret)
            {

                qDebug() << "设置停止位失败，失败原因：" << serial_port->errorString();
                MessageBox::warning(this, "错误", QString("设置停止位失败，失败原因：") + serial_port->errorString());
                return;
            }
            // 获取校验位
            QSerialPort::Parity checkbits = static_cast<QSerialPort::Parity>(this->ui->cB_checkbit->currentData().toInt());
            ret &= serial_port->setParity(checkbits);
            if(!ret)
            {

                qDebug() << "设置校验位失败，失败原因：" << serial_port->errorString();
                MessageBox::warning(this, "设置校验位失败", QString("原因:") + serial_port->errorString());
                return;
            }
            // 设置流控
            ret &= serial_port->setFlowControl(QSerialPort::NoFlowControl);
            if(!ret)
            {

                qDebug() << "设置流控失败，失败原因：" << serial_port->errorString();
                MessageBox::warning(this, "设置流控失败", QString("原因:") + serial_port->errorString());
                return;
            }
            // 设置读写
            ret &= serial_port->open(QSerialPort::ReadWrite);
            if(ret)
            {
                this->ui->pB_openSerail->setText("关闭");
                if(this->ui->cB_serailport->isEnabled())
                {
                    this->ui->cB_serailport->setEnabled(false);
                }
                this->is_open_serailport = true;


            }
            else
            {
                qDebug() << "自定义MessageBox提示打开错误" << serial_port->errorString();
                MessageBox::warning(this, "打开串口失败", QString("原因:") + serial_port->errorString());
            }
        }
    });



    this->send_timer = new QTimer(this);
    QObject::connect(this->send_timer, SIGNAL(timeout()), this, SLOT(slotOnSendSerialContent()));
    QObject::connect(ui->pB_senddata, SIGNAL(clicked()), this, SLOT(slotOnSendSerialContent()));
    QObject::connect(ui->pB_autoSend, &QPushButton::clicked, [this]
    {

        if(this->ui->pB_autoSend->text() == "自动发送")
        {
            bool is_ok = false;
            int send_inter_ms = this->ui->lE_send_ms->text()
            .toInt(&is_ok);
            is_ok &= !this->ui->tE_sendcon->toPlainText().isEmpty();
            is_ok &= this->is_open_serailport;
            if(is_ok)
            {

//                this->send_timer->setInterval(send_inter_ms);
                this->send_timer->start(send_inter_ms);
                this->ui->pB_autoSend->setText("停止发送");
                this->ui->pB_senddata->setDisabled(true);
                this->ui->pB_sendfile->setDisabled(true);
            }
            else
            {
                qDebug() << "报错";
                MessageBox::warning(this, "错误", "请查看串口是否打开以及时间间隔和内容是否设置！");
            }

        }
        else
        {
            this->ui->pB_senddata->setDisabled(false);
            this->ui->pB_sendfile->setDisabled(false);
            this->ui->pB_autoSend->setText("自动发送");
            this->send_timer->stop();
        }
    });

    QObject::connect(this->ui->pB_clear_count, &QPushButton::clicked, [this]()
    {
        this->send_cnt = 0;
        this->ui->lE_send_cnt->setText(QString::number(this->send_cnt));
        this->rcv_cnt = 0;
        this->ui->lE_rcv_cnt->setText(QString::number(this->rcv_cnt));
    });
    this->ui->pB_clear_count->click();
    QObject::connect(this->ui->pB_sendfile, &QPushButton::clicked, [this]
    {
        if(!this->is_open_serailport) return;
        QString fileName = QFileDialog::getOpenFileName(this, "打开文件");
        if(!fileName.isEmpty())
        {
            QFile file(fileName);
            if(!file.open(QIODevice::ReadOnly))
            {
                qDebug() << "open fail!";
                return;
            }
            const QByteArray& send_buffer = file.readAll();
            if(send_buffer.length() > 0)
            {
                QMap<SEND_TYPE, QByteArray> insert_data;
                const quint64& unix_timestamp = QDateTime::currentMSecsSinceEpoch();   //获取当前时间
                insert_data.insert(Serial_SEND, send_buffer);

                QString sendStr("[发] ");
                sendStr += QDateTime::fromMSecsSinceEpoch(unix_timestamp).toString("yyyy-MM-dd hh:mm:ss zzz");
                sendStr += "\r\n";

                if(this->ui->rB_rcvMode_text->isChecked())
                {
                    if(this->ui->rB_sendMode_hex->isChecked())
                    {
                        sendStr += send_buffer;
                    }
                    else
                    {
                        sendStr += QString(send_buffer);
                    }

                }
                else if(this->ui->rB_rcvMode_hex->isChecked())
                {
                    foreach(quint8 data, send_buffer)
                    {
                        if(data < 16)
                        {
                            sendStr += "0" + QString::number(data, 16) + " ";
                        }
                        else
                        {
                            sendStr += QString::number(data, 16) + " ";
                        }
                    }
                }
                this->ui->tE_rcvcon->append(sendStr);
                this->rcv_data_map->insert(unix_timestamp, insert_data);
                this->serial_port->write(send_buffer);

                this->send_cnt += send_buffer.length();
                this->ui->lE_send_cnt->setText(QString::number(this->send_cnt));

                QScrollBar* scrollBar = this->ui->tE_rcvcon->verticalScrollBar();
                scrollBar->setValue(scrollBar->maximum());
                MessageBox::info(this, "成功", "发送文件成功");
            }

            file.close();

        }
    });
}

MainWindow::~MainWindow()
{
    if(is_open_serailport)
    {
        serial_port->close();
    }
    delete serial_port;
    delete ui;
    delete rcv_data_map;
}



QString MainWindow::GetCorrectUnicode(const QByteArray& ba)
{
    QTextCodec::ConverterState state;
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
    QString text = codec->toUnicode(ba.constData(), ba.size(), &state);
    if(state.invalidChars > 0)
    {
        text = QTextCodec::codecForName("GBK")->toUnicode(ba);
    }
    else
    {
        text = ba;
    }
    return text;
}

void MainWindow::initSerialPortList()
{
    QList<QSerialPortInfo> infolist = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo& info, infolist)
    {
        ui->cB_serailport->addItem(info.portName());
    }
}

void MainWindow::initBautrateList()
{
    QStringList bautrate_list;
    bautrate_list << "600" << "1200" << "2400" << "4800" << "9600" << "14400"
                  << "19200" << "28800" << "38400" << "57600" << "115200"
                  << "230400" << "460800";
    foreach(const QString& bautrate_str, bautrate_list)
    {
        ui->cB_bautrate->addItem(bautrate_str, bautrate_str.toInt());
    }
    ui->cB_bautrate->setCurrentText("9600");
    ui->cB_bautrate->installEventFilter(this);
    QObject::connect(ui->cB_bautrate, &QComboBox::currentTextChanged, [this](QString)
    {
        if(this->is_open_serailport)
        {
            bool ret = this->serial_port->setBaudRate(this->ui->cB_bautrate->currentData().toInt());
            if(!ret)
            {
                qDebug() << "切换失败";
                this->ui->cB_bautrate->setCurrentIndex(this->last_combox_index);
                MessageBox::warning(this, "错误", "切换失败，可能当前硬件不支持这种波特率！");
            }
        }
    });
}

void MainWindow::initDataBitList()
{
    QMap<QString, QSerialPort::DataBits> databits_list;
    databits_list.insert("8", QSerialPort::Data8);
    databits_list.insert("7", QSerialPort::Data7);
    databits_list.insert("6", QSerialPort::Data6);
    databits_list.insert("5", QSerialPort::Data5);
    foreach(const QString& key, databits_list.keys())
    {
        ui->cB_databits->addItem(key, databits_list[key]);
    }
    if(ui->cB_databits->count() == databits_list.count())
    {
        ui->cB_databits->setCurrentIndex(ui->cB_databits->count() - 1);
    }
    ui->cB_databits->installEventFilter(this);
    QObject::connect(ui->cB_databits, &QComboBox::currentTextChanged, [this](QString)
    {
        if(this->is_open_serailport)
        {
            QSerialPort::DataBits databits = static_cast<QSerialPort::DataBits>(this->ui->cB_databits->currentData().toInt());
            bool ret = this->serial_port->setDataBits(databits);
            if(!ret)
            {
                qDebug() << "切换失败";
                this->ui->cB_databits->setCurrentIndex(this->last_combox_index);
                MessageBox::warning(this, "错误", "切换失败，可能当前硬件不支持这种数据位！");
//                return false;
            }
        }
    });
}

void MainWindow::initStopBitList()
{

    QMap<QString, QSerialPort::StopBits> stopbit_list;
    stopbit_list.insert("1", QSerialPort::OneStop);
    stopbit_list.insert("1.5", QSerialPort::OneAndHalfStop);
    stopbit_list.insert("2", QSerialPort::TwoStop);
    foreach(const QString& stopbit_key, stopbit_list.keys())
    {
        ui->cB_stopbit->addItem(stopbit_key, stopbit_list[stopbit_key]);
    }
    ui->cB_stopbit->installEventFilter(this);
    QObject::connect(ui->cB_stopbit, &QComboBox::currentTextChanged, [this](QString)
    {
        if(this->is_open_serailport)
        {
            QSerialPort::StopBits stopbits = static_cast<QSerialPort::StopBits>(this->ui->cB_stopbit->currentData().toInt());
            bool ret = this->serial_port->setStopBits(stopbits);
            if(!ret)
            {
                qDebug() << "切换失败";
                MessageBox::warning(this, "错误", "切换失败，可能当前硬件不支持这种停止位！");
                this->ui->cB_stopbit->setCurrentIndex(this->last_combox_index);
//                this->ui->cB_stopbit->
//                return false;
            }
        }
    });
}

void MainWindow::initCheckBitList()
{
    QMap<QString, QSerialPort::Parity> checkbit_list;
    checkbit_list.insert("无校验", QSerialPort::NoParity);
    checkbit_list.insert("奇校验", QSerialPort::OddParity);
    checkbit_list.insert("偶校验", QSerialPort::EvenParity);
    checkbit_list.insert("1 校验", QSerialPort::MarkParity);
    checkbit_list.insert("0 校验", QSerialPort::SpaceParity);
    foreach(const QString& key, checkbit_list.keys())
    {
        ui->cB_checkbit->addItem(key, checkbit_list[key]);
    }
    ui->cB_checkbit->setCurrentText("无校验");
    ui->cB_checkbit->installEventFilter(this);
    QObject::connect(ui->cB_checkbit, &QComboBox::currentTextChanged, [this](QString)
    {
        if(this->is_open_serailport)
        {
            QSerialPort::Parity checkbits = static_cast<QSerialPort::Parity>(this->ui->cB_checkbit->currentData().toInt());
            bool ret = this->serial_port->setParity(checkbits);
            if(!ret)
            {
                qDebug() << "切换失败";
                MessageBox::warning(this, "错误", "切换失败，可能当前硬件不支持这种校验位！");
                this->ui->cB_checkbit->setCurrentIndex(this->last_combox_index);
//                return false;
            }
        }
    });
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    bool ret = (watched == ui->cB_bautrate);
    ret |= (watched == ui->cB_checkbit);
    ret |= (watched == ui->cB_databits);
    ret |= (watched == ui->cB_stopbit);
    if(ret)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);
            if(mouse_event->button() == Qt::LeftButton)
            {
                this->last_combox_index = static_cast<QComboBox*>(watched)->currentIndex();
            }
        }
    }


    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::slotOnSendSerialContent()
{

    if(this->is_open_serailport)
    {
        QString contentString = this->ui->tE_sendcon->toPlainText();
        if(!contentString.isEmpty())
        {
            QByteArray send_arr;

            if(this->ui->rB_sendMode_text->isChecked())
            {
                QTextCodec* gbk = QTextCodec::codecForName("gbk");
                send_arr = gbk->fromUnicode(contentString.toLocal8Bit().data());
            }
            else if(this->ui->rB_sendMode_hex->isChecked())
            {
                int content_length = contentString.length();
                char hexchar, hexchar_next;
                for(int i = 0; i < content_length; i++)
                {
                    hexchar = contentString[i].toLower().toLatin1();
//                    qDebug("Test:%#x", hexchar);
                    switch(hexchar)
                    {
                        case '\r':
                        case '\n':
                        case ' ':
                            break;
                        default:
                            if(hexchar >= '0' && hexchar <= '9')
                            {
                                if(i + 1 < content_length)
                                {
                                    hexchar_next = contentString[i + 1].toLower().toLatin1();
                                    if(hexchar_next >= '0' && hexchar_next <= '9')
                                    {
                                        send_arr.append((char)(((hexchar - 0x30) << 4) | (hexchar_next - 0x30)));
                                        i++;
                                    }
                                    else if(hexchar_next >= 'a' && hexchar_next <= 'f')
                                    {
                                        send_arr.append((char)(((hexchar - 0x30) << 4) | (hexchar_next - 'a' + 10)));
                                        i++;
                                    }
                                    else if(hexchar_next == '\n' || hexchar_next == '\r' || hexchar_next == ' ')
                                    {
                                        send_arr.append((char)(hexchar - 0x30));
                                        i++;
                                    }
                                    else
                                    {
                                        send_arr.append((char)(hexchar - 0x30));
                                        i = content_length;
                                    }
                                }
                                else
                                {
                                    send_arr.append((char)(hexchar - 0x30));
                                }
                            }
                            else if(hexchar >= 'a' && hexchar <= 'f')
                            {
                                if(i + 1 < content_length)
                                {
                                    hexchar_next = contentString[i + 1].toLower().toLatin1();
                                    if(hexchar_next >= '0' && hexchar_next <= '9')
                                    {
                                        send_arr.append((char)(((hexchar - 'a' + 10) << 4) | (hexchar_next - 0x30)));
                                        i++;
                                    }
                                    else if(hexchar_next >= 'a' && hexchar_next <= 'f')
                                    {
                                        send_arr.append((char)(((hexchar - 'a' + 10) << 4) | (hexchar_next - 'a' + 10)));
                                        i++;
                                    }
                                    else if(hexchar_next == '\n' || hexchar_next == '\r' || hexchar_next == ' ')
                                    {
                                        send_arr.append((char)(hexchar - 'a' + 10));
                                        i++;
                                    }
                                    else
                                    {
                                        send_arr.append((char)(hexchar - 'a' + 10));
                                        i = content_length;
                                    }
                                }
                                else
                                {
                                    send_arr.append((char)(hexchar - 'a' + 10));
                                }
                            }
                            else
                            {
                                i = content_length;
                            }
                            break;
                    }
                }
                //end for
            }
            //end else if(this->ui->rB_sendMode_hex->isChecked())
            if(send_arr.length() > 0)
            {
                QMap<SEND_TYPE, QByteArray> insert_data;
                const quint64& unix_timestamp = QDateTime::currentMSecsSinceEpoch();   //获取当前时间
                insert_data.insert(Serial_SEND, send_arr);

                QString sendStr("[发] ");
                sendStr += QDateTime::fromMSecsSinceEpoch(unix_timestamp).toString("yyyy-MM-dd hh:mm:ss zzz");
                sendStr += "\r\n";

                if(this->ui->rB_rcvMode_text->isChecked())
                {
                    if(this->ui->rB_sendMode_hex->isChecked())
                    {
                        sendStr += send_arr;
                    }
                    else
                    {
                        sendStr += contentString;
                    }

                }
                else if(this->ui->rB_rcvMode_hex->isChecked())
                {
                    foreach(quint8 data, send_arr)
                    {
                        if(data < 16)
                        {
                            sendStr += "0" + QString::number(data, 16) + " ";
                        }
                        else
                        {
                            sendStr += QString::number(data, 16) + " ";
                        }
                    }
                }
                this->ui->tE_rcvcon->append(sendStr);
                this->rcv_data_map->insert(unix_timestamp, insert_data);
                this->serial_port->write(send_arr);

                this->send_cnt += send_arr.length();
                this->ui->lE_send_cnt->setText(QString::number(this->send_cnt));

                QScrollBar* scrollBar = this->ui->tE_rcvcon->verticalScrollBar();
                scrollBar->setValue(scrollBar->maximum());
            }
        }
        //end !contentString.isEmpty()
    }
    // end this->is_open_serial


}

void MainWindow::slotOnSaveData()
{

    QString fileName = QFileDialog::getSaveFileName(this, "保存文件");
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "open fail!";
            return;
        }
        QDataStream w_stream(&file);
        QByteArray w_data;
        QMap<quint64, QMap<SEND_TYPE, QByteArray>>::Iterator iter = this->rcv_data_map->begin();
        QObject* sender = this->sender();
        while(iter != this->rcv_data_map->end())
        {
            SEND_TYPE type = iter.value().firstKey();

            if(sender == this->ui->pB_saveRevData)
            {
                if(type == Serial_RCV)
                {
                    w_data.append(iter.value().value(type));
                }
            }
            else if(sender == this->ui->pB_saveSendData)
            {
                if(type == Serial_SEND)
                {
                    w_data.append(iter.value().value(type));
                }

            }
            else if(sender == this->ui->pB_saveAllData)
            {
                w_data.append(iter.value().value(type));
            }

            iter++;
        }
        w_stream.writeRawData(w_data.data(), w_data.length());
        file.close();
        MessageBox::info(this, "成功", "保存文件成功");
    }
}

