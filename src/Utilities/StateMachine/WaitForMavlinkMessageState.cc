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
        setError(QStringLiteral("No active vehicle available"));
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

    disconnect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &WaitForMavlinkMessageState::_messageReceived);
    if (_timeoutTimer.isActive()) {
        _timeoutTimer.stop();
    }
    emit advance();
}

void WaitForMavlinkMessageState::_onTimeout()
{
    if (_vehicle) {
        disconnect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &WaitForMavlinkMessageState::_messageReceived);
        _vehicle = nullptr;
    }
    emit timeout();
}
