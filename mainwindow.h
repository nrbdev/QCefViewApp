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

    void onBtnShowDevToolsClicked();

    void onBtnChangeColorClicked();

private:
    Ui::MainWindow *ui;
    QCefView* cefViewWidget = nullptr;
};
#endif // MAINWINDOW_H
