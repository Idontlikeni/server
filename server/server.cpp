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
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./Requests.db");
    if(db.open()){
        qDebug("open");
    }else{
        qDebug("no open");
    }

    query = new QSqlQuery(db);
    query->exec("CREATE TABLE requets ( Id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, time DATETIME NOT NULL, ip TEXT NOT NULL, bytes INTEGER NOT NULL);");
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    Sockets.push_back(socket);
    int i = (int)socketDescriptor;
    qDebug() << "client connected" << socketDescriptor << " " << socket->peerAddress().toString() << " " << socket->peerPort();
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    int id = socket->socketDescriptor();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_14);


    //qDebug() << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") << socket->peerAddress().toString();
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";
//        QString str;
//        in >>str;
//        qDebug() << str;
//        SendToClient(str.append("--------------------------"));
        repeatFinder.clear();
        lengthSorter.clear();
        QVector<QString> str;
        in >> str;
        QSqlQuery query2(db);
        QString adress = socket->peerAddress().toString().split(":")[3] + ":" + QString::number(socket->peerPort());
        query2.exec("INSERT INTO requets (time, ip, bytes) VALUES ('" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "', '" + adress + "', " + QString::number(sizeof(QString) * str.size()) + ");");
        qDebug() << "asasas";
        if(str[0] == "0"){
            qDebug() << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") << socket->peerAddress().toString() << sizeof(QString) * str.size();
            for(int i = 1; i < str.size(); i++){
                QString line = str[i];
                repeatFinder.calc(line);
                lengthSorter.calc(line);
                //qDebug() << line;
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
            SendToClient(d1, d2, {}, id);
        }else{
            QVector<QString> qv;
            QSqlQuery query2;
            query2.exec("SELECT time, ip, bytes FROM requets");
            while (query2.next()) {
                qv.push_back("Time: " + query2.value(0).toString() + " | IP: " + query2.value(1).toString() + " | Bytes: " + query2.value(2).toString());
                //qDebug() << name << salary;
            }
            SendToClient({}, {}, qv, id);
        }

    }
    else
    {
        qDebug() << "DataStream error";
    }
}


void Server::SendToClient(QMap<QChar, int> d1, QMap<int, QVector<QString>> d2, QVector<QString> qv, int id)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_14);
    out << d1 << d2 << qv;
    //socket->write(Data);
    for(int i = 0; i < Sockets.size(); ++i){
        if(id == Sockets[i]->socketDescriptor()){
            Sockets[i]->write(Data);
            break;
        }
    }
}
