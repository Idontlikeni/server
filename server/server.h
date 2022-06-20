#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QDataStream>
#include <algorithms.h>

class Server: public QTcpServer{
    Q_OBJECT

public:
    Server();
    QTcpSocket* socket;

private:
    QVector <QTcpSocket*> Sockets;
    QByteArray Data;
    void SendToClient(QMap<QChar, int> d1, QMap<int, QVector<QString>> d2);
    quint64 nextBlockSize;
    SRepeatFinder repeatFinder;
    LengthSorter lengthSorter;
public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
