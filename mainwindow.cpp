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

#if APP_DEV
#include <QTcpSocket>
#include <QTimer>
#endif

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
    QCefContext::instance()->addLocalFolderResource(appDir, "http://ViteApp");

#ifdef APP_DEV
    startVite();
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
/* VITE DEV SERVER  */
#if APP_DEV
void MainWindow::startVite()
{
    viteProcess = new QProcess(this);
    viteProcess->setWorkingDirectory(QStringLiteral(VITE_APP_DIR));
    viteProcess->setProgram("cmd.exe");
    viteProcess->setArguments({"/C", "npm run dev"});

    viteProcess->setProcessChannelMode(QProcess::MergedChannels);

    connect(viteProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        qDebug().noquote() << viteProcess->readAllStandardOutput();
    });

    connect(viteProcess, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
        qWarning() << "Vite process error:" << error;
    });

    connect(viteProcess, &QProcess::finished, this, [](int exitCode, QProcess::ExitStatus status) {
        qDebug() << "Vite exited:" << exitCode << status;
    });

    viteProcess->start();

    if (!viteProcess->waitForStarted(5000)) {
        qWarning() << "Failed to start Vite!";
        return;
    }

    qDebug() << "Starting Vite development server...";

    waitForVite();
}

void MainWindow::waitForVite()
{
    if (!viteProcess || viteProcess->state() == QProcess::NotRunning) {
        qWarning() << "Vite process is not running!";
        return;
    }

    auto *socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, [this, socket]() {
        socket->deleteLater();

        qDebug() << "Vite server is ready!";
    });

    connect(socket, &QTcpSocket::errorOccurred, this, [this, socket](QAbstractSocket::SocketError) {
        socket->deleteLater();

        QTimer::singleShot(300, this, [this]() { waitForVite(); });
    });

    socket->connectToHost("127.0.0.1", 5173);
}

void MainWindow::stopVite()
{
    if (!viteProcess)
        return;

    if (viteProcess->state() != QProcess::NotRunning) {
        QString pid = QString::number(viteProcess->processId());

        QProcess::execute("taskkill", {"/PID", pid, "/T", "/F"});

        viteProcess->waitForFinished(3000);
    }

    viteProcess->deleteLater();
    viteProcess = nullptr;
}

#endif

MainWindow::~MainWindow()
{
    delete ui;
}
