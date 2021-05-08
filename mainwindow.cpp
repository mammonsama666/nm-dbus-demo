#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDebug>
#include <QDBusMetaType>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDBusRegisterMetaType<QVector<QStringList>>();
    m_interface = new QDBusInterface("com.kylin.network", "/com/kylin/network",
                                     "com.kylin.network",
                                     QDBusConnection::sessionBus());
    if(!m_interface->isValid()) {
        qWarning() << qPrintable(QDBusConnection::sessionBus().lastError().message());
    }
    this->initConnection();

    this->m_model = new QStandardItemModel;
    ui->wifiList->setModel(m_model);
    m_model->setHorizontalHeaderItem(0, new QStandardItem("in-use"));
    m_model->setHorizontalHeaderItem(1, new QStandardItem("ssid"));
    m_model->setHorizontalHeaderItem(2, new QStandardItem("signal"));
    m_model->setHorizontalHeaderItem(3, new QStandardItem("security"));
    m_model->setHorizontalHeaderItem(4, new QStandardItem("max_freq"));
    m_model->setHorizontalHeaderItem(5, new QStandardItem("min_freq"));
    m_model->setHorizontalHeaderItem(6, new QStandardItem("category"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initConnection()
{
    connect(m_interface, SIGNAL(getWifiListFinished()), this, SLOT(getWifiListDone()));

    connect(ui->refreshBtn, &QPushButton::clicked, this, [ = ]() {
        ui->refreshBtn->setEnabled(false);
        ui->tipLabel->clear();
        QTimer::singleShot(5*1000, this, [ = ]() { //超时时间
            if (!ui->refreshBtn->isEnabled()) {
                ui->tipLabel->setText("Timeout, refresh failed!");
                ui->refreshBtn->setEnabled(true);
            }
        });
        if (m_interface) {
            m_interface->call("requestRefreshWifiList");
        }
    });
}

void MainWindow::getWifiListDone()
{
    ui->refreshBtn->setEnabled(true);
    ui->tipLabel->setText("Refresh succeed!");
    QDBusReply<QVector<QStringList>> reply = m_interface->call("getWifiList");
    if(reply.isValid()) {
        setWifiList(reply.value());
    } else {
        qWarning() << "value method called failed!";
    }
}

void MainWindow::setWifiList(const QVector<QStringList> &list)
{
    m_model->clear();
    if (list.at(0).at(0) != "--") { //如果列表第一个wifi不是--，就是已连接wifi
        m_model->setItem(0, 0, new QStandardItem("*"));
        for (int i = 0; i < list.length(); i++) {
            for (int j = 0; j < list.at(i).length(); j++) {
                m_model->setItem(i, j + 1, new QStandardItem(list.at(i).at(j)));
            }
        }
    } else {
        for (int i = 1; i < list.length(); i++) {
            for (int j = 0; j < list.at(i).length(); j++) {
                m_model->setItem(i - 1, j + 1, new QStandardItem(list.at(i).at(j)));
            }
        }
    }
}
