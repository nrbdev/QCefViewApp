#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCefContext.h>
#include <QCefSetting.h>
#include <QDebug>
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
    connect(cefViewWidget, &QCefView::loadEnd, this, &MainWindow::onLoadEnd);
    connect(cefViewWidget, &QCefView::loadStart, this, &MainWindow::onLoadStart);
    connect(cefViewWidget, &QCefView::loadingStateChanged, this, &MainWindow::onLoadingStateChanged);
}

void MainWindow::onBtnShowDevToolsClicked()
{
    if (cefViewWidget) {
        cefViewWidget->showDevTools();
    }
}

void MainWindow::onLoadStart(const QCefBrowserId &browserId,
                             const QCefFrameId &frameId,
                             bool isMainFrame,
                             int transitionType)
{
    qDebug() << "onLoadStart, browserId:" << browserId << ", frameId:" << frameId
             << ", isMainFrame:" << isMainFrame << ", transitionType:" << transitionType;
}

void MainWindow::onLoadEnd(const int &browserId,
                           const QString &frameId,
                           bool isMainFrame,
                           int httpStatusCode)
{
    qDebug() << "onLoadEnd, browserId:" << browserId << ", frameId:" << frameId
             << ", isMainFrame:" << isMainFrame << ", httpStatusCode:" << httpStatusCode;
}

void MainWindow::onLoadingStateChanged(const QCefBrowserId &browserId,
                                       bool isLoading,
                                       bool canGoBack,
                                       bool canGoForward)
{
    qDebug() << "onLoadingStateChanged, browserId:" << browserId << ", isLoading:" << isLoading
             << ", canGoBack:" << canGoBack << ", canGoForward:" << canGoForward;
}

void MainWindow::onInvokeMethod(const QCefBrowserId &browserId,
                                const QCefFrameId &frameId,
                                const QString &method,
                                const QVariantList &arguments)
{
    if (method == "TestMethod") {
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
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
