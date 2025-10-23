#pragma once

#include <QState>
#include <QString>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(QGCStateMachineLog)

class QGCStateMachine;

class QGCState : public QState
{
    Q_OBJECT

public:
    QGCState(const QString& stateName, QState* parentState);

    QGCStateMachine* machine();

signals:
    void advance();     ///< Signal to indicate state is complete and machine should advance to next state
    void error();       ///< Signal to indicate an error has occurred
};
