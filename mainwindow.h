#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDBusInterface>
#include <QDBusReply>
#include <QTableView>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QDBusInterface *m_interface = nullptr;
    QStandardItemModel * m_model = nullptr;
    void initConnection();
    void setWifiList(const QVector<QStringList> &list);

private:
    Ui::MainWindow *ui;

private slots:
    void getWifiListDone();
};
#endif // MAINWINDOW_H
