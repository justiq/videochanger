// Class BMPinfo

#include <iostream>
#include <vector>
#include <thread>
#include "imgInfo.h"


    bool BMPinfo::read_BMP(const char* filename, unsigned char* YUV420, int& vid_width, int& vid_height) {

        unsigned char* file_info = new unsigned char[54]; //dynamic array w/ img header
        FILE* image_file;

        if ((image_file = fopen(filename, "rb")) == NULL) {
            std::cout << "Can't open BMP image" << std::endl;
            throw 1;
        }

        fread(file_info, sizeof(unsigned char), 54, image_file); //header reading
        filesize = *(uint32_t*)&file_info[2];  //full size of img
        width = *(int*)&file_info[18];       //img width
        height = *(int*)&file_info[22];      //img height

        delete[] file_info;

        if (width > vid_width || height > vid_height) { // img fits video frame?
            fclose(image_file);
            return false;
        }

        int datacolour_size = filesize - 54; //RGB data
        unsigned char* datacolour = new unsigned char[datacolour_size]; //data array 

        fread(datacolour, sizeof(unsigned char), datacolour_size, image_file); //reading data BGR-24
        fclose(image_file);

        std::vector<std::thread> threads; //threads vector
        unsigned char* YUV = new unsigned char[datacolour_size]; //dynamic array YUV444
        int row_padded = (height - 1) * (width % 4);            //BMP file padding

        for (int i = 0; i < height; i++) {
            threads.emplace_back(std::thread(&BMPinfo::convert_RGB_to_YUV444, this,
                YUV, datacolour, row_padded, i));   //thread creation
            row_padded -= (width % 4);              //padding decrease
        }

        for (int x = 0; x < height; x++) { // waiting for threads ending
            threads[x].join();
        }

        threads.clear();
        delete[] datacolour;

        YUV_to_YUV420(YUV, YUV420);

        delete[] YUV;

        return true;
    }

    void BMPinfo::convert_RGB_to_YUV444(unsigned char* YUV, unsigned char* datacolour, const int& row_padded, int i) { //convertion to YUV444


        int Y, U, V;
        int r, g, b;
        for (int j = 0; j < width; j++) {

            
            b = datacolour[(j + (height - 1 - i) * width) * 3 + row_padded];
            g = datacolour[(j + (height - 1 - i) * width) * 3 + 1 + row_padded];
            r = datacolour[(j + (height - 1 - i) * width) * 3 + 2 + row_padded];

            //RGB to YUV
            Y = 0.257 * r + 0.504 * g + 0.098 * b + 16;
            U = -0.148 * r - 0.291 * g + 0.439 * b + 128;
            V = 0.439 * r - 0.368 * g - 0.071 * b + 128;

            if (Y < 0)
                Y = 0;
            else if (Y > 255)
                Y = 255;
            if (U < 0)
                U = 0;
            else if (U > 255)
                U = 255;
            if (V < 0)
                V = 0;
            else if (V > 255)
                V = 255;

            YUV[(j + i * width) * 3] = Y;
            YUV[(j + i * width) * 3 + 1] = U;
            YUV[(j + i * width) * 3 + 2] = V;
        }
    }

    void BMPinfo::YUV_to_YUV420(unsigned char* YUV, unsigned char* YUV420) { //convertion to YUV420

        int j;
        int sizeU = height * width, sizeV;
        sizeV = 5 * height * width;
        sizeV /= 4;

        std::vector<std::thread> threads; //threads vector

        for (j = 0; j < height; j++) {
            threads.emplace_back(std::thread(&BMPinfo::Y_in_YUV420, this, YUV, YUV420, j));
        }

        for (j = 0; j < height; j += 2) {
            threads.emplace_back(std::thread(&BMPinfo::UV_in_YUV420, this, YUV, YUV420, j,
                std::ref(sizeU), std::ref(sizeV)));
        }

        for (int x = 0; x < (int)threads.size(); x++) {
            threads[x].join();
        }
        threads.clear();
    }

    void BMPinfo::Y_in_YUV420(unsigned char* YUV, unsigned char* YUV420, int j) { //writing Y from 444 to 420
        for (int i = 0; i < width; i++) {
            YUV420[i + j * width] = YUV[(i + j * width) * 3];
        }
    }

    void BMPinfo::UV_in_YUV420(unsigned char* YUV, unsigned char* YUV420, int j, int& sizeU, int& sizeV) { //writing UV from 444 to 420
        int si0, si1, sj0, sj1;
        int sum, plus_size = j * width;
        plus_size /= 4;

        sj0 = j;
        sj1 = (j + 1 < height) ? j + 1 : j;

        for (int i = 0; i < width; i += 2) {
            si0 = i;
            si1 = (i + 1 < width) ? i + 1 : i;

            sum = YUV[(si0 + sj0 * width) * 3 + 1];
            sum += YUV[(si1 + sj0 * width) * 3 + 1];
            sum += YUV[(si0 + sj1 * width) * 3 + 1];
            sum += YUV[(si1 + sj1 * width) * 3 + 1];

            sum = sum / 4;

            YUV420[sizeU + plus_size] = (unsigned char)sum;

            sum = YUV[(si0 + sj0 * width) * 3 + 2];
            sum += YUV[(si1 + sj0 * width) * 3 + 2];
            sum += YUV[(si0 + sj1 * width) * 3 + 2];
            sum += YUV[(si1 + sj1 * width) * 3 + 2];

            sum = sum / 4;

            YUV420[sizeV + plus_size] = (unsigned char)sum;
            plus_size++;
        }
    }


    BMPinfo::BMPinfo(const char* filename, unsigned char* YUV420, int& vW, int& vH) { //class constructor

        if (!read_BMP(filename, YUV420, vW, vH)) {
            std::cout << "Image size doesn't fit!" << std::endl;
            throw 2;
        }
    }

