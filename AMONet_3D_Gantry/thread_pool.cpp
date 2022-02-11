#include "thread_pool.h"
#include <QThread>
#include <QDebug>

AMONet_Status::AMONet_Status(QObject *parent) : QObject(parent), QRunnable()
{
    setAutoDelete(true);
    getStatus = true;

}


void AMONet_Status::run()
{
    while (getStatus)
    {
        for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
        {
            _mnet_m4_motion_done(m_RingNo, m_DeviceIP, AxisNumber, m_vMotionStatus[AxisNumber]);
            _mnet_m4_get_command(m_RingNo, m_DeviceIP, AxisNumber, m_vGetCommand[AxisNumber]);
            _mnet_m4_get_current_speed(m_RingNo, m_DeviceIP, AxisNumber, m_vGetSpeed[AxisNumber]);
            _mnet_m4_get_position(m_RingNo, m_DeviceIP, AxisNumber, m_vGetPosition[AxisNumber]);

        }
        emit sendStatus( m_vMotionStatus, m_vGetCommand, m_vGetSpeed, m_vGetPosition);
        QThread::usleep(1);
    }
}

void AMONet_Status::receiveStatus(U16 m_RingNo, U16 m_DeviceIP, U16 m_MaxAxis)
{
    this->m_RingNo = m_RingNo;
    this->m_DeviceIP = m_DeviceIP;
    this->m_MaxAxis = m_MaxAxis;

    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        m_vMotionStatus.append(0); // motion status
        m_vGetCommand.append(0);   // command status
        m_vGetSpeed.append(0);     // speed status
        m_vGetPosition.append(0);  // position status
    }
}

void AMONet_Status::close()
{
    getStatus = false;
}
