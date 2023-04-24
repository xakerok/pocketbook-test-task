#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Bitmap
{
    template <typename T>
    void readint(std::ifstream &f, T &val)
    {  // endian-independant read
        // static_assert(sizeof(T) <= 4); //requires c++17
        uint8_t buf[4];
        f.read((char *)buf, sizeof(val));
        val = 0;
        for (int i = sizeof(val) - 1; i >= 0; i--) val += (buf[i] << (8 * i));
    }

    template <typename T>
    void writeint(std::ofstream &f, T val)
    {  // endian-independant write
        // static_assert(sizeof(T) <= 4); //requires c++17
        uint8_t buf[4];
        for (int i = sizeof(val) - 1; i >= 0; i--) buf[i] = (val >> 8 * i) & 0xff;
        f.write((char *)buf, sizeof(val));
    }

    // based on BITMAPFILEHEADER, struct size will be off
    struct cfileheader
    {
        char type[2];
        uint32_t size;
        uint32_t reserved;
        uint32_t offset;
    };

    // based on BITMAPINFOHEADER, struct size will be off
    struct cinfoheader
    {
        uint32_t struct_size;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bitcount;
        uint32_t compression;
        uint32_t image_size;
        uint32_t xpermeter;
        uint32_t ypermeter;
        uint32_t colors_used;
        uint32_t colors_important;
    };

    Bitmap(const Bitmap &) = delete;  // lets make these inaccessible
    Bitmap &operator=(Bitmap const &) = delete;

public:
    Bitmap();
    bool readfile(const char *filename);
    bool writefile(const char *filename);
    bool isopen;
    int width, height;
    int width_in_bytes;
    int bitcount;
    cfileheader fileheader;
    cinfoheader info;
    std::vector<uint32_t> palette;
    std::vector<uint8_t> image;
    uint32_t getpixel(int x, int y)
    {
        switch (bitcount)
        {
            case 1:
            {
                const int rowindex = (height - y - 1) * width_in_bytes;
                const uint8_t bit  = (image[rowindex + x / 8] >> (7 - (x % 8))) & 1;
                return palette[bit];
            }
            case 4:
            {
                const int start = (height - y - 1) * width_in_bytes;
                uint8_t pal     = image[start + x / 2];
                if (!(x % 2))
                    pal >>= 4;
                return palette[pal & 0xf];
            }
            case 8:
            {
                // find the index of pixel at x/y
                // the pixel should have a value between 0 to 256
                // get the color from palette
                const int i       = (height - y - 1) * width_in_bytes + x;
                const uint8_t pal = image[i];
                return palette[pal];
            }
            case 24:
            {
                const int i = (height - y - 1) * width_in_bytes + x * 3;
                return image[i + 2] + (image[i + 1] << 8) + (image[i + 0] << 16);
            }
            case 32:
            {
                const int i = (height - y - 1) * width_in_bytes + x * 4;
                return image[i + 2] + (image[i + 1] << 8) + (image[i + 0] << 16);
            }
        }
        return 0;
    }
};
