#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include <QTcpServer>  //监听套接字
#include <QTcpSocket>  //通信套接字//对方的(客户端的)套接字(通信套接字)

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private:
    Ui::Dialog *ui;

    QTcpServer *tcpServer;  //服务器套接字
    QTcpSocket *tcpSocket;  //连接后服务器返回的套接字
    QString data;

private slots:
    void ready_acpt();
    void recv_msg();
    void cnt_end();

public:
    QByteArray base64_encode(QString dt);
    QByteArray base64_decode(QString dt);
    QByteArray quoted_decode(QString dt);
    void showMail(QString mail_data);
};
#endif // DIALOG_H
