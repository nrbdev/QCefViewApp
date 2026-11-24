#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCefSetting.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCefSetting setting;

    cefViewWidget = new QCefView("https://www.testufo.com", &setting, this);
    ui->cefViewContainer->layout()->addWidget(cefViewWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
