#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../libmodbus_helper.h"

namespace Ui {
class MainWindow;
}

struct ServoPacket
{
    size_t channelNumber;

public:
    size_t targetPosition;

    ServoPacket(size_t channel, size_t position) : channelNumber(channel), targetPosition(position) {}
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_spinBox_editingFinished();

private:
    Ui::MainWindow *ui;

    ServoPacket servoPacket;

    ClientLibModBus client;
};

#endif // MAINWINDOW_H
