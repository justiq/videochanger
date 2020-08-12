//Class BMPinfo

#pragma once

class BMPinfo {
private:

    unsigned int filesize;

    bool read_BMP(const char*, unsigned char*, int&, int&);

    void convert_RGB_to_YUV(unsigned char*, unsigned char*, const int&, const int&, const int&, int);

public:
    BMPinfo(const char*, unsigned char*, int&, int&);

    int width;
    int height; 
};
