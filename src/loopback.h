#ifndef LOOPBACK_H
#define LOOPBACK_H

#include <QObject>
#include <QUdpSocket>

class LoopBack : public QObject
{
    Q_OBJECT
public:
    LoopBack(QObject *parent = 0);
    virtual ~LoopBack();


public slots:
    void processPendingDatagrams();
    void timerEvent(QTimerEvent *event);

private:
    QUdpSocket m_inSocket, m_outSocket;

    const static int m_wekInputPort =  6448;
    const static int m_wekOutputPort = 12000;
};

#endif // LOOPBACK_H
