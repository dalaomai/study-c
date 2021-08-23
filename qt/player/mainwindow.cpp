#include "mainwindow.h"
#include "ffmpegwidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnPlay_clicked()
{
    ui->player->setUrl(ui->lineUrl->text());
    ui->player->play();
}

void MainWindow::on_btnStop_clicked()
{
    ui->player->stop();
}