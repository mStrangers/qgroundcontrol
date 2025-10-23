#include "WaitForMavlinkMessageState.h"

#include "MultiVehicleManager.h"
#include "Vehicle.h"

#include "QGCLoggingCategory.h"

#include <QString>
#include <utility>
 
WaitForMavlinkMessageState::WaitForMavlinkMessageState(QState *parent, uint32_t messageId, Predicate predicate, int timeoutMsecs)
    : QGCState(QStringLiteral("WaitForMavlinkMessageState"), parent)
    , _messageId(messageId)
    , _predicate(std::move(predicate))
    , _timeoutMsecs(timeoutMsecs > 0 ? timeoutMsecs : 0)
{
    connect(this, &QState::entered, this, &WaitForMavlinkMessageState::_onEntered);
    connect(this, &QState::exited, this, &WaitForMavlinkMessageState::_onExited);
    _timeoutTimer.setSingleShot(true);
    connect(&_timeoutTimer, &QTimer::timeout, this, &WaitForMavlinkMessageState::_onTimeout);
}

void WaitForMavlinkMessageState::_onEntered()
{
    _vehicle = MultiVehicleManager::instance()->activeVehicle();
    if (!_vehicle) {
        qCWarning(QGCStateMachineLog) << "No active vehicle available";
        return;
    }

    connect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &WaitForMavlinkMessageState::_messageReceived, Qt::UniqueConnection);

    if (_timeoutMsecs > 0) {
        _timeoutTimer.start(_timeoutMsecs);
    }
}

void WaitForMavlinkMessageState::_onExited()
{
    if (_vehicle) {
        disconnect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &WaitForMavlinkMessageState::_messageReceived);
        _vehicle = nullptr;
    }

    _timeoutTimer.stop();
}

void WaitForMavlinkMessageState::_messageReceived(const mavlink_message_t &message)
{
    if (message.msgid != _messageId) {
        return;
    }
    if (_predicate && !_predicate(message)) {
        return;
    }

    qCDebug(QGCStateMachineLog) << "WaitForMavlinkMessageState::_messageReceived received expected message id" << _messageId;

    disconnect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &WaitForMavlinkMessageState::_messageReceived);
    _timeoutTimer.stop();

    emit advance();
}

void WaitForMavlinkMessageState::_onTimeout()
{
    qCDebug(QGCStateMachineLog) << "WaitForMavlinkMessageState::_onTimeout timeout waiting for message id" << _messageId;
    
    if (_vehicle) {
        disconnect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &WaitForMavlinkMessageState::_messageReceived);
        _vehicle = nullptr;
    }
    emit timeout();
}
