//Class BMPinfo

#pragma once

class BMPinfo {
private:

    unsigned int filesize;

    bool read_BMP(const char*, unsigned char*, int&, int&);

    void convert_RGB_to_YUV444(unsigned char*, unsigned char*, const int&, int);


    void YUV_to_YUV420(unsigned char*, unsigned char*); 

    void Y_in_YUV420(unsigned char*, unsigned char*, int); 

    void UV_in_YUV420(unsigned char*, unsigned char*, int, int&, int&);  

public:
    BMPinfo(const char*, unsigned char*, int&, int&);

    int width;
    int height; 
};
