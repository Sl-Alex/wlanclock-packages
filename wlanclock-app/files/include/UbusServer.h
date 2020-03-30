#ifndef UBUSSERVER_H
#define UBUSSERVER_H

class UbusServer
{
    public:
        static UbusServer& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static UbusServer instance;

            return instance;
        }

        virtual ~UbusServer() {};

        int start(void);
        int stop (void);
        bool isRunning(void);

        UbusServer(UbusServer const&)  = delete;
        void operator=(UbusServer const&) = delete;
    private:
        UbusServer();
};

#endif /* UBUS_SERVER_H */
