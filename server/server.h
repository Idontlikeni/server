#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QDataStream>
#include <algorithms.h>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QTime>
#include <QDate>
class Server: public QTcpServer{
    Q_OBJECT

public:
    Server();
    QTcpSocket* socket;

private:
    QVector <QTcpSocket*> Sockets;
    QByteArray Data;
    void SendToClient(QMap<QChar, int> d1, QMap<int, QVector<QString>> d2, QVector<QString> qv, int id);
    quint16 nextBlockSize;
    SRepeatFinder repeatFinder;
    LengthSorter lengthSorter;
    QSqlDatabase db;
    QSqlQuery *query;
    QDateTime datetime;
    QVector<QString> requets;
public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};

#endif // SERVER_H
