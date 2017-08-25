#ifndef LIBMODBUS_HELPER_H
#define LIBMODBUS_HELPER_H

#include <modbus.h>
#include <string>

class LibModBus
{
public:
    static const size_t DEFAULT_PORT = 1502;

    LibModBus(std::string tcpUrl, size_t port = DEFAULT_PORT)
    {
        context = modbus_new_tcp(tcpUrl.c_str(), port);
    }

    ~LibModBus()
    {
        modbus_close(context);
        modbus_free(context);
    }

    int setDebug(bool on)
    {
        return modbus_set_debug(context, on);
    }

protected:
    modbus_t* context;
};

class ServerLibModBus : public LibModBus
{
public:
    static const int DATA_SIZE = 260;
    static const size_t DATA_RECEIVE_FINISHED_MARK = static_cast<size_t>(-1);

    bool finished() { return query == current; }

    ServerLibModBus(std::string tcpUrl, size_t port = DEFAULT_PORT) :
        LibModBus(tcpUrl, port),
        query((uint8_t *)malloc(DATA_SIZE)),
        current(NULL),
        MODBUS_HEADER_LENGTH(modbus_get_header_length(context)),
        DATA_SIZE_POSITION(MODBUS_HEADER_LENGTH - 2),
        DATA_START(DATA_SIZE_POSITION + 1)
    {
        int s = modbus_tcp_listen(context, 1);
        modbus_tcp_accept(context, &s);
    }

    ~ServerLibModBus()
    {
        free(query);
    }

    void receive_loop()
    {
        current = query;
        size_t rc = 0;

        while (rc != DATA_RECEIVE_FINISHED_MARK)
        {
            current += rc;
            rc = modbus_receive(context, current);
        }
    }

    template<typename T>
    T& data()
    {
        return *reinterpret_cast<T*>(&query[DATA_START]);
    }

    template<typename T>
    T& receive()
    {
        receive_loop();
        return data<T>();
    }

    uint8_t* rawData()
    {
        return &query[DATA_START];
    }

    size_t dataSize() const
    {
        return query[DATA_SIZE_POSITION];
    }

private:
    uint8_t* query;
    uint8_t* current;

    const int MODBUS_HEADER_LENGTH;
    const int DATA_SIZE_POSITION;
    const int DATA_START;
};

class ClientLibModBus : public LibModBus
{
public:
    ClientLibModBus(std::string tcpUrl, size_t port = DEFAULT_PORT) : LibModBus(tcpUrl, port)
    {
        modbus_connect(context);
    }

    template<typename T>
    int send(T data) const
    {
        return modbus_send_raw_request(context, (uint8_t *)&data, sizeof(data));
    }

    template<typename T, size_t N>
    int send(T(& data)[N]) const
    {
        return modbus_send_raw_request(context, (uint8_t *)data, N);
    }
};

#endif // LIBMODBUS_HELPER_H
