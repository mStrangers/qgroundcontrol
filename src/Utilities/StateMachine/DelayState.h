#pragma once

#include "QGCState.h"

#include <QTimer>

class DelayState : public QGCState
{
    Q_OBJECT

public:
    DelayState(QState* parentState, int delayMsecs);

signals:
    void delayComplete();

private:
    QTimer _delayTimer;
};
