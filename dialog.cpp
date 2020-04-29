#include "dialog.h"
#include "ui_dialog.h"
#include <QtNetwork>
#include <QMessageBox>
#include <QPixmap>

// 编码表
static const unsigned char base64_encode_table[64] =
{
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/'
};

// 解码表，128个ascii编码对应
static const unsigned char base64_decode_table[128] =
{
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0,0xF0,0xFF,0xFF,0xF1,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xE0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3E,0xFF,0xF2,0xFF,0x3F,
    0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
    0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
    0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF
};

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // 初始化
    tcpServer = new QTcpServer();
    tcpSocket = nullptr;
    data = "";
    ui->img->clear();

    ui->logShow->append("*** SMTP服务器准备好");
    ui->logShow->append("$$$$$$$$$$$$$$$$$$$$$$");

    // 调用listen函数监听同时绑定IP和端口号
    if(tcpServer->listen(QHostAddress::LocalHost,2019))
    {
        // 将服务器的连接信号连接到接收连接的槽
        this->connect(tcpServer,SIGNAL(newConnection()),this,SLOT(ready_acpt()));
    }
    else
    {
        QMessageBox::critical(this, tr("网络"),
                              tr("错误: %1.").arg(tcpServer->errorString()));
    }
}

// 建立连接
void Dialog::ready_acpt()
{
    ui->logShow->append("*** 收到连接请求");
    ui->logShow->append("*** 建立连接");
    if(tcpServer->hasPendingConnections())  // 查询是否有新连接
    {
        // 获取客户端套接字
        tcpSocket = tcpServer->nextPendingConnection();
        if(tcpSocket != nullptr)  // 客户端存在
        {
            // 连接成功，回复本服务器状态
            QString msg = "220 Simple Mail Server Ready for Mail Server";
            ui->logShow->append("S:" + msg);
            // 使用write函数向服务器发送数据
            QString sendmsg = msg + "\r\n";
            tcpSocket->write(sendmsg.toLocal8Bit());
            // 接收消息
            this->connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(recv_msg()));
            // 连接结束
            this->connect(tcpSocket,SIGNAL(disconnected()),this,SLOT(cnt_end()));
        }
    }
}

// 接收消息
void Dialog::recv_msg()
{
    // 获得消息
    QByteArray buffer = tcpSocket->readAll();
    QString recvmsg = QString::fromLocal8Bit(buffer);
    if(recvmsg.left(4) == "HELO" || recvmsg.left(4) == "EHLO")
    {
        ui->logShow->append("C:" + recvmsg.trimmed());  // 去掉首尾空白符
        QString msg = "250 OK 127.0.0.1";
        ui->logShow->append("S:" + msg);
        // 使用write函数向服务器发送数据
        QString sendmsg = msg + "\r\n";
        tcpSocket->write(sendmsg.toLocal8Bit());
    }
    else if(recvmsg.left(10) == "MAIL FROM:")
    {
        ui->logShow->append("C:" + recvmsg.trimmed());
        QString msg = "250 Sender OK";
        ui->logShow->append("S:" + msg);
        // 使用write函数向服务器发送数据
        QString sendmsg = msg + "\r\n";
        tcpSocket->write(sendmsg.toLocal8Bit());
    }
    else if(recvmsg.left(8) == "RCPT TO:")
    {
        ui->logShow->append("C:" + recvmsg.trimmed());
        QString msg = "250 Receiver OK";
        ui->logShow->append("S:" + msg);
        // 使用write函数向服务器发送数据
        QString sendmsg = msg + "\r\n";
        tcpSocket->write(sendmsg.toLocal8Bit());
    }
    else if(recvmsg.left(4) == "DATA")
    {
        ui->logShow->append("C:" + recvmsg.trimmed());
        QString msg = "354 Go ahead. End with <CRLF>.<CRLF>";
        ui->logShow->append("S:" + msg);
        // 使用write函数向服务器发送数据
        QString sendmsg = msg + "\r\n";
        tcpSocket->write(sendmsg.toLocal8Bit());
    }
    else if(recvmsg.left(4) == "QUIT")
    {
        ui->logShow->append("C:" + recvmsg.trimmed());
        QString msg = "221 Quit, Goodbye !";
        ui->logShow->append("S:" + msg);
        // 使用write函数向服务器发送数据
        QString sendmsg = msg + "\r\n";
        tcpSocket->write(sendmsg.toLocal8Bit());
    }
    else if(recvmsg.left(4) == "RSET"){
        ui->logShow->append("C:" + recvmsg.trimmed());
    }
    else  // 邮件DATA内容
    {
        data = data + recvmsg;
        if(data.right(5) == "\r\n.\r\n")
        {
            showMail(data);
            QString msg = "250 Message accepted for delivery";
            ui->logShow->append("S:" + msg);
            // 使用write函数向服务器发送数据
            QString sendmsg = msg + "\r\n";
            tcpSocket->write(sendmsg.toLocal8Bit());
        }
    }
}

// 关闭连接
void Dialog::cnt_end()
{
    if(tcpSocket != nullptr)
    {
        ui->logShow->append("关闭连接");
        tcpSocket->close();
        tcpSocket->deleteLater();
        //tcpServer->close();
    }
}

// base64编码，3个字节转换成4个可打印字符
QByteArray Dialog::base64_encode(QString dt)
{
    if(dt.isEmpty())
    {
        return dt.toLocal8Bit();
    }

    QString res;
    QByteArray tmp = dt.toLocal8Bit();
    int ind = 0;
    for(int i = tmp.length();i > 0;i -= 3)
    {
        if(i >= 3)
        {
            int b0 = tmp.at(ind) & 0xFF;
            int b1 = tmp.at(ind+1) & 0xFF;
            int b2 = tmp.at(ind+2) & 0xFF;
            res.append(base64_encode_table[b0>>2]);
            res.append(base64_encode_table[((b0<<4)|(b1>>4)) & 0x3F]);
            res.append(base64_encode_table[((b1<<2)|(b2>>6)) & 0x3F]);
            res.append(base64_encode_table[b2 & 0x3F]);
            ind += 3;
        }
        else
        {
            int b0 = tmp.at(ind) & 0xFF;
            int b1;
            if(i == 2)
            {
                b1 = tmp.at(ind+1) & 0xFF;
            }
            else
            {
                b1 = 0;
            }
            res.append(base64_encode_table[b0>>2]);
            res.append(base64_encode_table[((b0<<4)|(b1>>4)) & 0x3F]);
            if(i == 1)
            {
                res.append('=');
            }
            else
            {
                res.append(base64_encode_table[(b1<<2) & 0x3F]);
            }
            res.append('=');
        }
    }
    return res.toLocal8Bit();
}

// base64解码
QByteArray Dialog::base64_decode(QString dt)
{
    // QByteArray test = QByteArray::fromBase64(data.toUtf8());
    // QString res = test.toStdString().c_str();
    // return res;

    if(dt.isEmpty())
    {
        return dt.toLocal8Bit();
    }
    // ui->mailShow->append(data);

    int len = dt.length();  // 字符串长度

    QByteArray res;
    QDataStream out(&res, QIODevice::WriteOnly);  // 写入数据流

    int ind = 0;
    int p = 0;
    for(int i = 0;i < len; i++)
    {
        int v = dt[i].unicode();
        if(v >= 0x80)  // ascii最大0x7F
        {
            ind = -1;
            break;
        }

        int value = base64_decode_table[v];  // 找到对应编码
        if(value == 0xFF)
        {
            ind = -1;
            break;
        }

        // 跳过换行\n回车\r连字符-制表符\t空格space
        if(value == 0xF0|value == 0xF1|value == 0xF2|value == 0xE0|value == 0xF3)
            continue;

        if(ind == 0)
        {
            if(v == '=')
            {
                ind = -1;
                break;
            }

            p = value;
            ind++;
        }
        else if(ind == 1)
        {
            if(v == '=')
            {
                ind = -1;
                break;
            }

            out<<static_cast<unsigned char>((p<<2)|(value>>4));
            p = value;
            ind++;
        }
        else if(ind == 2)
        {
            if(v == '=')
            {
                ind = 0;
                break;
            }

            out<<static_cast<unsigned char>((p<<4)|(value>>2));
            p = value;
            ind++;
        }
        else
        {
            ind = 0;
            if(v == '=')
                break;

            out<<static_cast<unsigned char>((p<<6)|value);
        }
    }
    if(ind == 0)  // 有效字符个数必须是4的倍数
        return res;
    else if(ind == -1)
        return "base64_decode wrong1";
    else
        return "base64_decode worng2";
}

QByteArray Dialog::quoted_decode(QString dt)
{
    QByteArray dt_tmp = dt.toLocal8Bit();
    int i = 0;
    char tmp = 0;
    QByteArray res;

    while(i < dt.length())
    {
        if(dt.at(i) == '='&&dt.at(i+1) == '\r'&&dt.at(i+2) == '\n')
        {
            i += 3;
        }
        else if(dt.at(i) == '=')
        {
            char c1 = dt_tmp.at(i+1);
            char c2 = dt_tmp.at(i+2);
            tmp = ((c1>'9')?(c1-'A'+10):(c1-'0'))*16 + ((c2>'9')?(c2-'A'+10):(c2-'0'));
            res.append(tmp);
            i += 3;
        }
        else
        {
            res.append(dt.at(i));
            i++;
        }
    }

    return res;
}

void Dialog::showMail(QString mail_data)
{
    // Base64的基本思想：将3个字节转换成4个可打印字符
    // 剩1个字节：后面补4个比特的“0”，再分成2个6位组，映射为2个ASCII字符，而后再填充两个“=”
    // 剩2个字节：后面补2个比特的“0”，再分成3个6位组，映射为3个ASCII字符，而后再填充1个“=”
    // 添加回车换行：变换后，每76个字符后增加一回车换行

    ui->mailShow->append("$$$$$$$$$$$$$$$$$$$$$$");
    // ui->mailShow->append(mail_data);

    QStringList data_list = mail_data.split("\r\n");
    QString mail_head,mail_body;
    // QString bdy = "";
    int i,fl = 0,ec_type = 0;  // fl分邮件头与邮件体；ec_type标志编码方式,1表示base64,2表示复杂
    // 处理邮件头
    for(i = 0;fl == 0;i++)
    {
        QString tmp = data_list.at(i);  // 一行数据
        if(tmp.isEmpty())  // 邮件头和邮件体之间用空行分隔
        {
            fl = 1;  // 标志空行，分离每一大段
        }
        else if(tmp == "Content-Transfer-Encoding: base64")
        {
            ec_type = 1;
            mail_head.append(tmp + '\n');
        }
        else if(tmp.left(23) == "Content-Type: multipart")
        {
            ec_type = 2;
            mail_head.append(tmp + '\n');
            /*
            i++;
            tmp = data_list.at(i);

            if(tmp.trimmed().left(8) == "boundary")
            {
                QStringList tp = tmp.split("\"");
                bdy = "--" + tp.at(1);
            }

            mail_head.append(tmp + '\n');
            */
        }
        // 分了3段，=?和?=是开始和结束标记，中间的?是间隔符
        // 第一段：utf-8或者gbk，表示原来页面的编码方式
        // 第二段：B是表示邮件的编码方式为base64
        // 第三段：标题内容
        else if(tmp.left(11) == "Subject: =?")
        {
            QStringList tp = tmp.split('?');
            if(tp.at(2) == "B")
            {
                mail_head.append("Subject: " + base64_decode(tp.at(3)) + '\n');
            }
            else
            {
                mail_head.append(tmp + '\n');
            }
        }
        else
        {
            mail_head.append(tmp + '\n');
        }
    }
    ui->mailShow->append(mail_head);
    // 处理邮件体
    if(ec_type == 0)  //简单7bit编码，或quoted-printable编码（暂未处理）
    {
        for(;i < data_list.size() - 2;i++)
        {
            QString tmp = data_list.at(i);
            ui->mailShow->append(tmp);
        }
    }
    else if(ec_type == 1)  //简单base64编码邮件
    {
        for(;i < data_list.size() - 2;i++)
        {
            QString tmp = data_list.at(i);  // 一行数据
            if(tmp.isEmpty())
            {
                ui->mailShow->append(tmp);
            }
            else
            {
                mail_body.append(tmp + "\r\n");
            }
        }
        QString re_body = base64_decode(mail_body);
        ui->mailShow->append(re_body);
    }
    else  //复杂邮件
    {
        ui->mailShow->append(data_list.at(i));
        int flag = 0,type_fl = 0,has_e = 0;  //标志base64/文件格式/是否有附件
        QString f_name,f_type;  // 记录储存文件的名称和格式
        for(i += 1;i < data_list.size() - 2;i++)
        {
            QString tmp = data_list.at(i);
            if(tmp == "Content-Transfer-Encoding: base64")
            {
                flag = 1;
                ui->mailShow->append(tmp);
            }
            else if(tmp == "Content-Transfer-Encoding: quoted-printable")
            {
                flag = 2;
                ui->mailShow->append(tmp);
            }
            else if(tmp.left(19) == "Content-Type: image")
            {
                type_fl = 1;
                // QStringList ft = tmp.split('/');
                // f_type = ft.at(1).left(ft.length()-1);
                ui->mailShow->append(tmp);
            }
            else if(tmp.left(23) == "Content-Type: text/html")
            {
                type_fl = 2;
                ui->mailShow->append(tmp);
            }
            else if(tmp.left(6) == "\tname=")  // 有附件
            {
                has_e = 1;
                QStringList t = tmp.split('\"');
                if(t.at(1).startsWith("=?"))
                {
                    QStringList tp = tmp.split('?');
                    if(tp.at(2) == "B")
                    {
                        QByteArray fn = base64_decode(tp.at(3));
                        ui->mailShow->append("\tname=\"" + fn + "\"");
                        f_name = fn.toStdString().c_str();
                    }
                    else
                    {
                        ui->mailShow->append(tmp);
                    }
                }
                else
                {
                    f_name = t.at(1);
                    ui->mailShow->append(tmp);
                }
            }
            else if(tmp.left(9) == "\tfilename")
            {
                QStringList tp;
                if(tmp.contains("utf-8"))
                {
                    tp = tmp.split("\'\'");
                    ui->mailShow->append("\tfilename=" + tp.at(1));
                }
                else
                {
                    ui->mailShow->append(tmp);
                }

            }
            else if(tmp.isEmpty())  // 每部分邮件体之间用空行分隔
            {
                ui->mailShow->append(tmp);
                i++;
                tmp = data_list.at(i);
                QString part_body;
                while(tmp.left(12) != "--Apple-Mail")
                {
                    part_body.append(tmp + "\r\n");
                    i++;
                    tmp = data_list.at(i);
                }
                if(flag == 1)  // 是base64编码的部分
                {
                    // ui->mailShow->append("原：" + part_body);
                    QByteArray re_body = base64_decode(part_body);

                    if(type_fl == 1)  // 图片
                    {
                        ui->mailShow->append("图片见左图。");
                        ui->img->clear();

                        QBuffer buf(&re_body);
                        buf.open(QIODevice::WriteOnly);

                        QImage image;  // 用QImage进行加载，然后转乘QPixmap用户绘制。QPixmap绘制效果是最好的。
                        if(!image.loadFromData(re_body))
                        {
                            ui->mailShow->append("image loaded wrong. ");
                        }
                        ui->img->setPixmap(QPixmap::fromImage(image));
                        image.save("/Users/renlei/Qt-workspace/img/" + f_name);
                        type_fl = 0;
                    }
                    else if(type_fl == 2)  // html
                    {
                        ui->mailShow->append(re_body);

                        type_fl = 0;
                    }
                    else
                    {
                        ui->mailShow->append(re_body);
                    }
                    flag = 0;
                }
                else if(flag == 2)  // quoted
                {
                    QByteArray re_body = quoted_decode(part_body);

                    if(type_fl == 1)  // 图片
                    {
                        ui->mailShow->append("图片见左图。");
                        ui->img->clear();

                        QBuffer buf(&re_body);
                        buf.open(QIODevice::WriteOnly);

                        QImage image;  // 用QImage进行加载，然后转乘QPixmap用户绘制。QPixmap绘制效果是最好的。
                        if(!image.loadFromData(re_body))
                        {
                            ui->mailShow->append("image loaded wrong.");
                        }
                        ui->img->setPixmap(QPixmap::fromImage(image));
                        image.save("/Users/renlei/Qt-workspace/img/" + f_name);
                        type_fl = 0;
                    }
                    else if(type_fl == 2)  // html
                    {
                        ui->mailShow->append(re_body);

                        type_fl = 0;
                    }
                    else
                    {
                        ui->mailShow->append(re_body);
                    }
                    flag = 0;
                }
                else  // 不是base64和quoted
                {
                    ui->mailShow->append(part_body);
                    type_fl = 0;
                }
                ui->mailShow->append(tmp);
            }
            else
            {
                ui->mailShow->append(tmp);
            }
        }
    }
}

Dialog::~Dialog()
{
    delete ui;
}

