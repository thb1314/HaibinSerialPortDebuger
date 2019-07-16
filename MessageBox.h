#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>
#include <QDialog>
#include <QPoint>
class QPushButton;
class QLabel;

class MessageBox : public QDialog
{
    Q_OBJECT
public:
    explicit MessageBox(QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    enum Icon
    {
        // keep this in sync with QMessageDialogOptions::Icon
        NoIcon = 0,
        Information = 1,
        Warning = 2,
        Critical = 3,
        Question = 4
    };
    Q_ENUM(Icon)
    void initWidget(void);
    bool setInfoText(const QString& text);
    void setInfoIcoType(const Icon icon);
    void setWindowTitle(const QString& title);
    QPushButton* confirm_button;
    QPushButton* cancel_button;
    static int warning(QWidget* parent, const QString& title, const QString& text);
    static int info(QWidget* parent, const QString& title, const QString& text);
    static int about(QWidget* parent, const QString& title, const QString& text);
    bool eventFilter(QObject*, QEvent*);

private:
    QLabel* infotext;
    QLabel* labico;
    QLabel* centralTitle;
    bool record_mousePressed;
    QPoint record_mousePoint;
signals:

public slots:
};

#endif // MESSAGEBOX_H
