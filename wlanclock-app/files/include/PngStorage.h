#ifndef PNGSTORAGE_H
#define PNGSTORAGE_H

#include "AbstractCanvas.h"
#include <vector>

class PngStorage
{
    public:
        static PngStorage& getInstance()
        {
            // Guaranteed to be destroyed.
            // Instantiated on first use.
            static PngStorage instance;

            return instance;
        }

        virtual ~PngStorage();

        void init();
        AbstractCanvas *getCanvas(std::string filename);
        int loadPng(std::string filename);

        PngStorage(PngStorage const&)  = delete;
        void operator=(PngStorage const&) = delete;
    private:
        PngStorage();
        typedef struct
        {
            AbstractCanvas *canvas;
            std::string name;
        } PngElement;
        int loadPng(PngElement &element);

        std::vector<PngElement> mPngStorage;
};

#endif /* PNGSTORAGE_H */
