#include "SendMavlinkMessageState.h"

#include "MAVLinkProtocol.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"

#include "QGCLoggingCategory.h"

#include <QString>
#include <utility>

SendMavlinkMessageState::SendMavlinkMessageState(QState *parent, MessageEncoder encoder, int retryCount)
    : QGCState(QStringLiteral("SendMavlinkMessageState"), parent)
    , _encoder(std::move(encoder))
    , _retryCount(retryCount)
{
    connect(this, &QState::entered, this, &SendMavlinkMessageState::_sendMessage);
}

void SendMavlinkMessageState::_sendMessage()
{
    if (++_runCount > _retryCount + 1) {
        qCDebug(QGCStateMachineLog) << "Exceeded maximum retries sending MAVLink message";
        emit error();
        return;
    }

    if (!_encoder) {
        qCDebug(QGCStateMachineLog) << "No MAVLink message encoder configured";
        emit error();
        return;
    }

    Vehicle *vehicle = MultiVehicleManager::instance()->activeVehicle();
    if (!vehicle) {
        qCWarning(QGCStateMachineLog) << "No active vehicle available";
        emit error();
        return;
    }

    SharedLinkInterfacePtr sharedLink = vehicle->vehicleLinkManager()->primaryLink().lock();
    if (!sharedLink) {
        qCWarning(QGCStateMachineLog) << "No active link available to send MAVLink message";
        emit error();
        return;
    }

    mavlink_message_t message{};

    const uint8_t systemId = MAVLinkProtocol::instance()->getSystemId();
    const uint8_t componentId = MAVLinkProtocol::getComponentId();
    const uint8_t channel = sharedLink->mavlinkChannel();

    _encoder(systemId, channel, &message);
    if (!vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), message)) {
        qCWarning(QGCStateMachineLog) << "Failed to send MAVLink message";
        emit error();
        return;
    }

    emit advance();
}
