#include "server.h"

Server::Server()
{
    if(this->listen(QHostAddress::Any, 2323))
    {
        qDebug() << "Start";
    }
    else
    {
        qDebug() << "error";
    }
    nextBlockSize = 0;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    Sockets.push_back(socket);
    qDebug() << "client connected" << socketDescriptor;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_14);
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";
//        QString str;
//        in >>str;
//        qDebug() << str;
//        SendToClient(str.append("--------------------------"));
        for (;;) {
            if(nextBlockSize == 0)
            {
                qDebug() << "nextBlockSize == 0";
                if(socket->bytesAvailable() < 2)
                {
                    qDebug() << "Data < 2, break";
                    break;
                }
                in >> nextBlockSize;
                qDebug() << "nextBlockSize = " << nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize)
            {
                qDebug() << "Data not full";
                break;
            }

            QVector<QString> str;
            in >> str;
            //QStringList qlist = str.split('');
            for(int i = 0; i < str.size(); i++){
                QString line = str[i];
                //line.chop(1);
                repeatFinder.calc(line);
                lengthSorter.calc(line);
                qDebug() << line;
            }
            QString qs1 = "";
            QString qs2 = "";
            QMap<QChar, int> d1 = repeatFinder.get_qdata();
            QMap<int, QVector<QString>> d2 = lengthSorter.get_qdata();

            for( QMap<QChar, int>::iterator i = d1.begin(); i != d1.end(); i++){
                qs1.append(i.key());
                qs1.append(" - " + QString::number(i.value()) + "\n");
            }

            for(QMap<int, QVector<QString>>::iterator i = d2.begin(); i != d2.end(); i++){
                int c = 0;
                for(QString j: i.value()){
                    if(c == 0)qs2.append(QString::number(i.key()));
                    else qs2.append(" ");
                    qs2.append(" - " + j + "\n");
                    c++;
                }
            }

            qDebug() << "Data:" << str;
            qDebug() << "1: " << d1;
            qDebug() << "2: " << d2;
            nextBlockSize = 0;
            SendToClient(d1, d2);
            break;
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}


void Server::SendToClient(QMap<QChar, int> d1, QMap<int, QVector<QString>> d2)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    out << quint64(0) << d1 << d2;
    out.device()->seek(0);
    out << quint64(Data.size() - sizeof(quint64));
    //socket->write(Data);
    for(int i = 0; i < Sockets.size(); ++i){
        Sockets[i]->write(Data);
    }
}
