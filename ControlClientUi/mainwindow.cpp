#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    servoPacket(0, 0),
    client("127.0.0.1")
{
    ui->setupUi(this);
    ui->spinBox->setValue(0);

    client.setDebug(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    servoPacket.channelNumber = ui->comboBox->currentIndex();
    servoPacket.targetPosition = ui->spinBox->value();

    client.send(servoPacket);
}

void MainWindow::on_spinBox_editingFinished()
{
    ui->pushButton->click();
}
