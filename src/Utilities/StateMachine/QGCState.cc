#include "QGCStateMachine.h"
#include "QGCLoggingCategory.h"

QGC_LOGGING_CATEGORY(QGCStateMachineLog, "Utilities.StateMachine")

QGCState::QGCState(const QString& stateName, QState* parentState) 
    : QState(QState::ExclusiveStates, parentState)
{
    setObjectName(stateName);

    connect(this, &QState::entered, this, [this] () {
        qCDebug(QGCStateMachineLog) << "Entered state" << objectName() << " - " << Q_FUNC_INFO;
    });
    connect(this, &QState::exited, this, [this] () {
        qCDebug(QGCStateMachineLog) << "Exited state" << objectName() << " - " << Q_FUNC_INFO;
    });
}

QGCStateMachine* QGCState::machine() 
{ 
    return qobject_cast<QGCStateMachine*>(QState::machine()); 
}
