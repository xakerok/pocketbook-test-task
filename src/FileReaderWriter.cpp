
#include "FileReaderWriter.h"

#include <fstream>
#include <QImage>
#include <QDebug>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

Bitmap::Bitmap()
    : isopen{false}, fileheader{}, info{}, bitcount{}, width{}, height{},
      width_in_bytes{} {}

bool Bitmap::readfile(const char* filename)
{
    std::ifstream fin(filename, std::ios::binary);
    if (!fin) { std::cout << "open failed " << filename << '\n'; return false; }

    fin.read(fileheader.type, 2);
    if (strncmp(fileheader.type, "BM", 2) != 0) return false;
    readint(fin, fileheader.size);
    readint(fin, fileheader.reserved);
    readint(fin, fileheader.offset);

    readint(fin, info.struct_size);
    readint(fin, info.width);
    readint(fin, info.height);
    readint(fin, info.planes);
    readint(fin, info.bitcount);
    readint(fin, info.compression);
    readint(fin, info.image_size);
    readint(fin, info.xpermeter);
    readint(fin, info.ypermeter);
    readint(fin, info.colors_used);
    readint(fin, info.colors_important);

    width = info.width;
    height = info.height;
    bitcount = info.bitcount;

    if (info.struct_size != 40)
    {
        printf("wrong structure size %d\n", info.struct_size);
        return false;
    }

    std::vector<uint16_t> bitcheck {1,4,8,24,32};
    if(std::find(bitcheck.begin(), bitcheck.end(), bitcount) == bitcheck.end())
    {
        printf("cannot handle this bitcount %d\n", bitcount);
        return false;
    }

    int palette_size = (bitcount > 8) ? 0 : (1 << bitcount);
    palette.resize(palette_size);
    for (auto &e : palette)
    {
        //BGRA -> ABGR
        uint8_t buf[4] {};
        fin.read((char*)buf, 4);
        e = buf[2] | (buf[1] << 8) | (buf[0] << 16) | (buf[3] << 24);
    }

    if(fin.tellg() != fileheader.offset)
    { printf("error reading image\n"); return false; }

    width_in_bytes = ((width * info.bitcount + 31) / 32) * 4;
    image.resize(width_in_bytes * height);
    fin.read((char*)image.data(), image.size());
    isopen = true;
    return true;
}

bool Bitmap::writefile(const char* filename)
{
    if (!isopen) return false;
    std::ofstream fout(filename, std::ios::binary);
    if (!fout) { std::cout << "open failed " << filename << '\n'; return false; }

    fout.write((char*)fileheader.type, 2);
    writeint(fout, fileheader.size);
    writeint(fout, fileheader.reserved);
    writeint(fout, fileheader.offset);
    writeint(fout, info.struct_size);
    writeint(fout, info.width);
    writeint(fout, info.height);
    writeint(fout, info.planes);
    writeint(fout, info.bitcount);
    writeint(fout, info.compression);
    writeint(fout, info.image_size);
    writeint(fout, info.xpermeter);
    writeint(fout, info.ypermeter);
    writeint(fout, info.colors_used);
    writeint(fout, info.colors_important);

    for (auto &e : palette)
    {
        //ABGR -> BGRA
        uint8_t buf[4]{};
        buf[0] = (e >> 16) & 0xff;
        buf[1] = (e >>  8) & 0xff;
        buf[2] = (e >>  0) & 0xff;
        buf[3] = (e >> 24) & 0xff;
        fout.write((char*)buf, 4);
    }

    fout.write((char*)image.data(), image.size());

    return true;
}

FileReaderWriter::FileReaderWriter()
{

}

void FileReaderWriter::readFile(const std::string &fileName)
{

    Bitmap img;

    img.readfile(fileName.c_str());

    img.writefile("D:\\3_raw.bmp");


//    QImage saveImg(bitmap.width, bitmap.height, QImage::Format_Indexed8);
//    saveImg.loadFromData(bitmap.data.data(), bitmap.data.size());

//    saveImg.save("D:\\3.bmp", "BMP");
}

