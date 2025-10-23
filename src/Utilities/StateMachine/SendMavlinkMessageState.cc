#include "SendMavlinkMessageState.h"

#include "MAVLinkProtocol.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"

#include "QGCLoggingCategory.h"

#include <QString>
#include <utility>

SendMavlinkMessageState::SendMavlinkMessageState(QState *parent, MessageEncoder encoder)
    : QGCState(QStringLiteral("SendMavlinkMessageState"), parent)
    , _encoder(std::move(encoder))
{
    connect(this, &QState::entered, this, &SendMavlinkMessageState::_sendMessage);
}

void SendMavlinkMessageState::_sendMessage()
{
    if (!_encoder) {
        setError(QStringLiteral("No MAVLink message encoder configured"));
        return;
    }

    Vehicle *vehicle = MultiVehicleManager::instance()->activeVehicle();
    if (!vehicle) {
        setError(QStringLiteral("No active vehicle available"));
        return;
    }

    SharedLinkInterfacePtr sharedLink = vehicle->vehicleLinkManager()->primaryLink().lock();
    if (!sharedLink) {
        setError(QStringLiteral("No active link available to send MAVLink message"));
        return;
    }

    mavlink_message_t message{};

    const uint8_t systemId = MAVLinkProtocol::instance()->getSystemId();
    const uint8_t componentId = MAVLinkProtocol::getComponentId();
    const uint8_t channel = sharedLink->mavlinkChannel();

    _encoder(systemId, channel, &message);
    if (!vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), message)) {
        setError(QStringLiteral("Failed to send MAVLink message"));
        return;
    }

    emit advance();
}
