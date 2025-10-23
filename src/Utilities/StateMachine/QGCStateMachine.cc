#include "QGCStateMachine.h"
#include "QGCApplication.h"
#include "AudioOutput.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "AudioOutput.h"

#include <QFinalState>

QGCStateMachine::QGCStateMachine(const QString& machineName, QObject* parent)
    : QStateMachine (parent)
    , _vehicle      (MultiVehicleManager::instance()->activeVehicle())
{
    setObjectName(machineName);

    connect(this, &QGCStateMachine::started, this, [this] () {
        qCDebug(QGCStateMachineLog) << "State machine started:" << objectName();
    });
    connect(this, &QGCStateMachine::stopped, this, [this] () {
        qCDebug(QGCStateMachineLog) << "State machine finished:" << objectName();
    });

    connect(this, &QGCStateMachine::stopped, this, [this] () { this->deleteLater(); });
}

void QGCStateMachine::setError(const QString& errorString)
{
    qCWarning(QGCStateMachineLog) << "errorString" << errorString << " - " << Q_FUNC_INFO;
    _errorString = errorString;
    displayError();
    stop();
}

void QGCStateMachine::displayError()
{
    qCWarning(QGCStateMachineLog) << _errorString << " - " << Q_FUNC_INFO;
    qgcApp()->showAppMessage(_errorString);
    _errorString.clear();
}
