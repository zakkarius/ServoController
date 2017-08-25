#ifndef PHIDGET_HELPER_H
#define PHIDGET_HELPER_H

#include <cstdio>
#include <phidget22.h>

PhidgetReturnCode ErrorHandler(PhidgetReturnCode errorCode, const char message[] = "", bool detailed = false)
{
    if (errorCode != EPHIDGET_OK)\
    {
        if (!detailed)
            printf("Failed to %s\n", message);
        else
        {
            const char* errs = "";
            Phidget_getErrorDescription(errorCode, &errs);
            printf("Failed to %s: %s\n", message, errs);
        }
    }

    return errorCode;
}

static void CCONV
onAttachHandler(PhidgetHandle phid, void *)
{
    PhidgetReturnCode res;
    int hubPort;
    int channel;
    int serial;

    ErrorHandler(Phidget_getDeviceSerialNumber(phid, &serial), "get device serial number");
    ErrorHandler(Phidget_getChannel(phid, &channel), "get channel number");
    res = ErrorHandler(Phidget_getHubPort(phid, &hubPort), "get hub port");
    if (res != EPHIDGET_OK)
    {
        fprintf(stderr, "failed to get hub port\n");
        hubPort = -1;
    }

    if (hubPort == -1)
        printf("channel %d on device %d attached\n", channel, serial);
    else
        printf("channel %d on device %d hub port %d attached\n", channel, serial, hubPort);
}

static void CCONV
onDetachHandler(PhidgetHandle phid, void *)
{
    PhidgetReturnCode res;
    int hubPort;
    int channel;
    int serial;

    ErrorHandler(Phidget_getDeviceSerialNumber(phid, &serial), "get device serial number");
    ErrorHandler(Phidget_getChannel(phid, &channel), "get channel number");
    res = ErrorHandler(Phidget_getHubPort(phid, &hubPort), "get hub port");
    if (res != EPHIDGET_OK)
        hubPort = -1;

    if (hubPort != -1)
        printf("channel %d on device %d detached\n", channel, serial);
    else
        printf("channel %d on device %d hub port %d detached\n", channel, hubPort, serial);
}

static void CCONV
errorHandler(PhidgetHandle, void *, Phidget_ErrorEventCode errorCode, const char* errorString)
{
    fprintf(stderr, "Error: %s (%d)\n", errorString, errorCode);
}

static void CCONV
onPositionChangeHandler(PhidgetRCServoHandle, void *, double position)
{
	printf("Position Changed: %.3g\n", position);
}

static void CCONV
onVelocityChangeHandler(PhidgetRCServoHandle, void *, double velocity)
{
	printf("Velocity Changed: %.3g\n", velocity);
}

static void CCONV
onTargetPositionReachedHandler(PhidgetRCServoHandle, void *, double position)
{
	printf("Target Position Reached: %.3g\n", position);
}

static PhidgetReturnCode CCONV
initChannel(PhidgetHandle ch, int channelNumber)
{
    ErrorHandler(Phidget_setChannel((PhidgetHandle)ch, channelNumber), "set channel number");

    ErrorHandler(Phidget_setOnAttachHandler(ch, onAttachHandler, NULL), "assign on attach handler");
    ErrorHandler(Phidget_setOnDetachHandler(ch, onDetachHandler, NULL), "assign on detach handler");
    ErrorHandler(Phidget_setOnErrorHandler(ch, errorHandler, NULL), "assign on error handler");

    return EPHIDGET_OK;
}

static PhidgetReturnCode CCONV
initialize(PhidgetRCServoHandle ch, int channelNumber)
{
    const char* errs = "";

    ErrorHandler(initChannel((PhidgetHandle)ch, channelNumber), "initialize channel", true);

    ErrorHandler(PhidgetRCServo_setOnPositionChangeHandler(ch, onPositionChangeHandler, NULL), "set position change handler", true);
    ErrorHandler(PhidgetRCServo_setOnVelocityChangeHandler(ch, onVelocityChangeHandler, NULL), "set velocity change handler", true);
    ErrorHandler(PhidgetRCServo_setOnTargetPositionReachedHandler(ch, onTargetPositionReachedHandler, NULL), "set target position reached handler", true);

    PhidgetReturnCode res = ErrorHandler(Phidget_openWaitForAttachment((PhidgetHandle)ch, 5000), "open channel");
    if (res != EPHIDGET_OK)
    {
        if (res == EPHIDGET_TIMEOUT)
            printf("Channel did not attach after 5 seconds: please check that the device is attached\n");
        else
        {
            Phidget_getErrorDescription(res, &errs);
            fprintf(stderr, "failed to open channel:%s\n", errs);
        }
    }

    printf("Setting target position to 0\n");
    ErrorHandler(PhidgetRCServo_setTargetPosition(ch, 0), "set target position");

    printf("Setting engaged\n");
    ErrorHandler(PhidgetRCServo_setEngaged(ch, 1), "set engaged");

    return EPHIDGET_OK;
}

static PhidgetReturnCode CCONV
deinitialize(PhidgetRCServoHandle ch)
{
    Phidget_close((PhidgetHandle)ch);
    PhidgetRCServo_delete(&ch);

    return EPHIDGET_OK;
}

size_t powerOf2(size_t n)
{
    return 0x1 << n;
}

template<typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

class PhidgetController
{
public:
    enum CHANNEL_NUMBER
    {
        CHANNEL0 = 0,
        CHANNEL1,
        CHANNEL2,
        CHANNEL3,
        CHANNEL4,
        CHANNEL5,
        CHANNEL6,
        CHANNEL7,
        CHANNELS_COUNT
    };

    enum CHANNEL_FLAGS
    {
        NO_FLAG       = 0,
        CHANNEL0_FLAG = 0x1,
        CHANNEL1_FLAG = 0x2,
        CHANNEL2_FLAG = 0x4,
        CHANNEL3_FLAG = 0x8,
        CHANNEL4_FLAG = 0x10,
        CHANNEL5_FLAG = 0x20,
        CHANNEL6_FLAG = 0x40,
        CHANNEL7_FLAG = 0x80,
        ALL_FLAG      = 0xFF
    };

public:
    PhidgetController()
    {
        PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);

        for (size_t i = CHANNEL0; i < CHANNELS_COUNT; ++i)
        {
            ErrorHandler(PhidgetRCServo_create(&channels[i]), "create rc servo channel");
            initialize(channels[i], i);
        }
    }

    PhidgetController(size_t channelsFlags)
    {
        PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);

        if (channelsFlags > ALL_FLAG)
        {
            printf("INVALID Channel Flags  %d\n", channelsFlags);
            channelsFlags = ALL_FLAG;
        }

        for (size_t i = CHANNEL0; i < CHANNELS_COUNT; ++i)
        {
            if (channelFlag((CHANNEL_NUMBER)i) & channelsFlags)
            {
                ErrorHandler(PhidgetRCServo_create(&channels[i]), "create rc servo channel");
                initialize(channels[i], i);
            }
        }
    }

    ~PhidgetController()
    {
        for (size_t i = 0; i < CHANNELS_COUNT; ++i)
            deinitialize(channels[i]);
    }

    PhidgetReturnCode move(CHANNEL_NUMBER ch, double pt)
    {
        double moveToPosition = POSITION(pt);
        printf("Channel %d: Setting Target Position to %f for 5 seconds...\n", ch, moveToPosition);
        return PhidgetRCServo_setTargetPosition(channels[ch], moveToPosition);
    }

    double current(CHANNEL_NUMBER ch) const
    {
        return POSITION_IN_MM(getPosition(ch));
    }


private:
    PhidgetRCServoHandle channels[CHANNELS_COUNT] = { NULL };

    static CHANNEL_FLAGS channelFlag(CHANNEL_NUMBER ch)
    {
        return (CHANNEL_FLAGS)powerOf2(ch);
    }

    static const CHANNEL_FLAGS ChannelFlags[CHANNELS_COUNT];

    static constexpr double MIN_IN_MM   = 0.0;
    static constexpr double MAX_IN_MM   = 100.0;
    static const size_t MIN_ACTUAL      = 43;
    static const size_t MAX_ACTUAL      = 138;
    static const size_t LENGTH_ACTUAL   = MAX_ACTUAL - MIN_ACTUAL;
    static constexpr double ONE_DIGIT_LENGTH = MAX_IN_MM / LENGTH_ACTUAL;

    static double POSITION(double valueInMM)
    {
        double result = MIN_ACTUAL + valueInMM / ONE_DIGIT_LENGTH;

        if (result < MIN_ACTUAL)
            return MIN_ACTUAL;

        if (result > MAX_ACTUAL)
            return MAX_ACTUAL;

        return result;
    }

    static double POSITION_IN_MM(double point)
    {
        if (point <= MIN_ACTUAL)
            return MIN_IN_MM;

        if (point >= MAX_ACTUAL)
            return MAX_IN_MM;

        return (point - MIN_ACTUAL) * ONE_DIGIT_LENGTH;
    }

    double getPosition(CHANNEL_NUMBER ch) const
    {
        double result;
        PhidgetRCServo_getTargetPosition(channels[ch], &result);
        return result;
    }
};

const PhidgetController::CHANNEL_FLAGS PhidgetController::ChannelFlags[PhidgetController::CHANNELS_COUNT] =
{
    PhidgetController::CHANNEL0_FLAG,
    PhidgetController::CHANNEL1_FLAG,
    PhidgetController::CHANNEL2_FLAG,
    PhidgetController::CHANNEL3_FLAG,
    PhidgetController::CHANNEL4_FLAG,
    PhidgetController::CHANNEL5_FLAG,
    PhidgetController::CHANNEL6_FLAG,
    PhidgetController::CHANNEL7_FLAG
};

struct ServoPacket
{
public:
    PhidgetController::CHANNEL_NUMBER channelNumber;
    size_t targetPosition;

    ServoPacket(PhidgetController::CHANNEL_NUMBER channel, size_t position) : channelNumber(channel), targetPosition(position) {}
};

#endif // PHIDGET_HELPER_H
