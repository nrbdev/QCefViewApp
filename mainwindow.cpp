#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCefSetting.h>

#include <QCefContext.h>
#include <QColor>
#include <QDir>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QString>

#define URL_ROOT "http://ViteApp"
#define PROD_URL URL_ROOT "/index.html"
#define DEV_URL "http://localhost:5500"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QCefSetting setting;

    QDir dir = QCoreApplication::applicationDirPath();
    QString webResourceDir = QDir::toNativeSeparators(dir.filePath("webres"));
    QCefContext::instance()->addLocalFolderResource(webResourceDir, URL_ROOT);

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
    connect(ui->btn_ChangeColor, &QPushButton::clicked, this, &MainWindow::onBtnChangeColorClicked);
    connect(cefViewWidget, &QCefView::invokeMethod, this, &MainWindow::onInvokeMethod);
}

void MainWindow::onBtnShowDevToolsClicked()
{
    if (cefViewWidget) {
        cefViewWidget->showDevTools();
    }
}

void MainWindow::onBtnChangeColorClicked()
{
    if (cefViewWidget) {
        QColor color(QRandomGenerator::global()->generate());
        QCefEvent event("colorChange");

        event.arguments().append(QVariant::fromValue(color.name(QColor::HexArgb)));
        cefViewWidget->broadcastEvent(event);
    }
}

void MainWindow::onInvokeMethod(const QCefBrowserId &browserId,
                                const QCefFrameId &frameId,
                                const QString &method,
                                const QVariantList &arguments)
{
    if (0 == method.compare("TestMethod")) {
        QMetaObject::invokeMethod(
            this,
            [=]() {
                QString title("QCef InvokeMethod Notify");
                QString text = QString("Current Thread: QT_UI\r\n"
                                       "Method: %1\r\n"
                                       "Arguments:\r\n")
                                   .arg(method);

                for (int i = 0; i < arguments.size(); i++) {
                    // clang-format off
                    text.append(QString("%1 Type:%2, Value:%3\r\n")
                                    .arg(i)
                                    .arg(arguments[i].typeName())
                                    .arg(arguments[i].toString()));
                    // clang-format on
                }

                QMessageBox::information(this->window(), title, text);
            },
            Qt::QueuedConnection);
    } else {
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
