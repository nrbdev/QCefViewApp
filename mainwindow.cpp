#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCefContext.h>
#include <QCefSetting.h>
#include <QDir>
#include <QMessageBox>
#include <QPushButton>
#include <QString>

#define URL_ROOT "http://ViteApp"
#define PROD_URL URL_ROOT "/index.html"
#define DEV_URL "http://localhost:5173"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCefSetting setting;

    QString appDir = QCoreApplication::applicationDirPath() + "/app";
    QCefContext::instance()->addLocalFolderResource(appDir, URL_ROOT);

#ifdef APP_DEV
    cefViewWidget = new QCefView(DEV_URL, &setting, this);
#else
    cefViewWidget = new QCefView(PROD_URL, &setting, this);
#endif

    ui->cefViewContainer->layout()->addWidget(cefViewWidget);
    connect(ui->btn_openDevTools,
            &QPushButton::clicked,
            this,
            &MainWindow::onBtnShowDevToolsClicked);
    connect(cefViewWidget, &QCefView::invokeMethod, this, &MainWindow::onInvokeMethod);
}

void MainWindow::onBtnShowDevToolsClicked()
{
    if (cefViewWidget) {
        cefViewWidget->showDevTools();
    }
}

void MainWindow::onInvokeMethod(const QCefBrowserId &browserId,
                                const QCefFrameId &frameId,
                                const QString &method,
                                const QVariantList &arguments)
{
    if (method == "TestMethod") {
        QMetaObject::invokeMethod(
            this,
            [=]() {
                QString title("QCef InvokeMethod Notify");
                QString text = QString("Current Thread: QT_UI\r\n"
                                       "Method: %1\r\n"
                                       "Arguments:\r\n")
                                   .arg(method);

                for (int i = 0; i < arguments.size(); i++) {
                    text.append(QString("%1 Type:%2, Value:%3\r\n")
                                    .arg(i)
                                    .arg(arguments[i].typeName())
                                    .arg(arguments[i].toString()));
                }

                QMessageBox::information(this->window(), title, text);
            },
            Qt::QueuedConnection);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
