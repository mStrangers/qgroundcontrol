#pragma once

#include "QGCState.h"
#include "QGCMAVLink.h"

#include <cstdint>
#include <functional>

class Vehicle;

class SendMavlinkMessageState : public QGCState
{
    Q_OBJECT

public:
    using MessageEncoder = std::function<void (uint8_t systemId, uint8_t channel, mavlink_message_t *message)>;

    SendMavlinkMessageState(QState *parent, MessageEncoder encoder);

private slots:
    void _sendMessage();

private:
    MessageEncoder _encoder;
};
