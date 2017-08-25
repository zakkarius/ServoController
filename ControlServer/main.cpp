#include <cstdio>

#include "../phidget_helper.h"
#include "../libmodbus_helper.h"

int main()
{
    ServerLibModBus server("127.0.0.1");
    server.setDebug(true);

    PhidgetController ph;

    while (!server.finished())
    {
        ServoPacket rc = server.receive<ServoPacket>();

        printf("CONNECTION.................... %d", errno);

        if (errno == ECONNRESET)
            break;

        ph.move(rc.channelNumber, rc.targetPosition);
    }

    return 0;
}
