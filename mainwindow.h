#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCefView.h>
#include <QString>
#include <QVariantList>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected slots:
    void onInvokeMethod(const QCefBrowserId& browserId,
                        const QCefFrameId& frameId,
                        const QString& method,
                        const QVariantList& arguments);

    void onLoadEnd(const int &browserId,
                   const QString &frameId,
                   bool isMainFrame,
                   int httpStatusCode);

    void onLoadStart(const QCefBrowserId &browserId,
                     const QCefFrameId &frameId,
                     bool isMainFrame,
                     int transitionType);

    void onLoadingStateChanged(const QCefBrowserId &browserId,
                               bool isLoading,
                               bool canGoBack,
                               bool canGoForward);

    void onBtnShowDevToolsClicked();

private:
    Ui::MainWindow *ui;
    QCefView* cefViewWidget = nullptr;
};
#endif // MAINWINDOW_H
