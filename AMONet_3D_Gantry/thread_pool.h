#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <QObject>
#include <QRunnable>
#include <QVector>
#include <windows.h>

// include from AMONet //
#include <AMONet.h>
#include <AMONetError.h>
// include from AMONet //

class AMONet_Status : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit AMONet_Status(QObject *parent = nullptr);

    void run() override;
    void receiveStatus(U16 m_RingNo, U16 m_DeviceIP, U16 m_MaxAxis);
    void close();

private:
    bool getStatus;
    U16 m_MaxAxis;
    U16 m_RingNo;
    U16 m_DeviceIP;

    QVector<U16> m_vMotionStatus;
    QVector<I32> m_vGetCommand;
    QVector<I32> m_vGetPosition;
    QVector<F64> m_vGetSpeed;

signals:
    void sendStatus( QVector<U16> m_vMotionStatus, QVector<I32> m_vGetCommand, QVector<F64> m_vGetSpeed, QVector<I32> m_vGetPosition);


};

#endif // THREAD_POOL_H
