#ifndef AMONET_CONTROLLER_H
#define AMONET_CONTROLLER_H

#include <windows.h>
#include <QObject>
#include <QTimer>

#include "Static_Refrence.h"

// include from AMONet //
#include <AMONet.h>
#include <AMONetError.h>
// include from AMONet //

#define High_Active 0
#define Low_Active  1



class AMONet_Controller : public QObject
{
    Q_OBJECT
public:
    explicit AMONet_Controller(QObject *parent = nullptr);
    ~AMONet_Controller();

private:
    U16    m_RingNo;       // AMONet Driver Ring NO.
    U16    m_DeviceIP;     // Device IP
    U16    m_MaxAxis;      // Maximun Axis

    QVector<F64>    m_vStartVel;     // Set start speed
    QVector<F64>    m_vMaxVel;       // Set maximum velocity
    QVector<F64>    m_vTacc;         // Specified acceleration time in unit of second ( 0 ~ 速度Max 所花時間 )
    QVector<F64>    m_vTdec;         // Specified deceleration time in unit of second ( 速度Max ~ 0 所花時間)


    I32    m_NewPosition;  // Set New Position


    BOOL    m_AlmLogic;     // Alarm logic
    BOOL    m_LtcLogic;     // LCT logic
    BOOL    m_SDLogic;      // SD Logic

    // HANDLE  hEvent;     // MNET communication status // 要使用看門口模式再使用

    BOOL    m_bInit;              // PCI-1202U Card is Initialize or not.
    BOOL    m_bStart;             // Judge Start Ring setting ON or not.
    BOOL    m_bAxisInit;          // AxisInitialize Status
    BOOL    m_bServoStatus;       // Servo state
    BOOL    m_bERC;               // ERC state
    BOOL    m_bRALM;              // servo Driver Alarm state
    BOOL    m_IsRelative;         // is Relative or absolutely

public:
    BOOL CardInitialize();
    BOOL SetStartRing();
    BOOL AxisInitialize(U16 maxAxis = 3);
    BOOL ServoSwitch();
    BOOL AlarmSwitch();
    BOOL ErcSwitch();
    BOOL ResetPositionAndCmd();
    BOOL DriveClose();

    BOOL MotionStop();
    BOOL setSpeed( QVector<F64> startVel, QVector<F64> maxVel,
                   QVector<F64> Tacc, QVector<F64> Tdec);
    BOOL WaitMotionStop();

    QVector<U16> m_vMotionStatus;
    QVector<I32> m_vGetCommand;
    QVector<F64> m_vGetSpeed;


private:
    BOOL ErrorHandler(I16 statusRet);


signals:

};

#endif // AMONET_CONTROLLER_H
