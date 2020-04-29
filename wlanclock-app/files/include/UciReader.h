#ifndef UCIREADER_H
#define UCIREADER_H

#include <cstdint>

class UciReader
{
    public:
        static UciReader& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static UciReader instance;

            return instance;
        }

        virtual ~UciReader(){};

        std::string getKey(const char *path);
        int setKey(const char *path, const char *value);

        UciReader(UciReader const&)  = delete;
        void operator=(UciReader const&) = delete;
    private:
        UciReader(){};
};

#endif /* UCIREADER_H */
