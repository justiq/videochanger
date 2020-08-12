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

    int sizeU = height * width, sizeV; //where starts U & V in YUV420
    sizeV = 5 * height * width;
    sizeV /= 4;

    std::vector<std::thread> threads; //threads vector
    int row_padded = (height - 1) * (width % 4);            //BMP file padding

    int num_threads = std::thread::hardware_concurrency(); //Possible threads to be concurret
    
    int i = 0, k = 0;
    while (i < height) {
        while ((k < num_threads) && (i < height)) {
            threads.emplace_back(std::thread(&BMPinfo::convert_RGB_to_YUV, this,
                YUV420, datacolour, row_padded, std::ref(sizeU), std::ref(sizeV), i));   //thread creation
            row_padded -= (width % 4);              //padding decrease
            i++;
            k++;
        }
        k = 0;
        for (auto& t : threads) {
            t.join();
        }
        threads.clear();
    }
    delete[] datacolour;
    return true;
}

void BMPinfo::convert_RGB_to_YUV(unsigned char* YUV, unsigned char* datacolour, const int& row_padded, 
    const int& sizeU, const int& sizeV, int i) { //convertion to YUV444

    int Y;
    int r, g, b;
    for (int j = 0; j < width; j++) {

        b = datacolour[(j + (height - 1 - i) * width) * 3 + row_padded];
        g = datacolour[(j + (height - 1 - i) * width) * 3 + 1 + row_padded];
        r = datacolour[(j + (height - 1 - i) * width) * 3 + 2 + row_padded];

        //RGB to YUV
        Y = 0.257 * r + 0.504 * g + 0.098 * b + 16;

        if (Y < 0)
            Y = 0;
        else if (Y > 255)
            Y = 255;
        
        YUV[j + i * width] = Y;
    }

    if (i % 2 == 0) {

        int U, V;
        int si0, si1, sj0, sj1;
        int plus_size = i * width;
        plus_size /= 4;
    
        si0 = i;
        si1 = (i + 1 < height) ? i + 1 : i;

        for (int j = 0; j < width; j += 2) {
            sj0 = j;
            sj1 = (j + 1 < width) ? j + 1 : j;

            b = datacolour[(sj0 + (height - 1 - si0) * width) * 3 + row_padded];
            g = datacolour[(sj0 + (height - 1 - si0) * width) * 3 + 1 + row_padded];
            r = datacolour[(sj0 + (height - 1 - si0) * width) * 3 + 2 + row_padded];
            U = -0.148 * r - 0.291 * g + 0.439 * b + 128;
            V = 0.439 * r - 0.368 * g - 0.071 * b + 128;

            b = datacolour[(sj1 + (height - 1 - si0) * width) * 3 + row_padded];
            g = datacolour[(sj1 + (height - 1 - si0) * width) * 3 + 1 + row_padded];
            r = datacolour[(sj1 + (height - 1 - si0) * width) * 3 + 2 + row_padded];
            U += -0.148 * r - 0.291 * g + 0.439 * b + 128;
            V += 0.439 * r - 0.368 * g - 0.071 * b + 128;

            b = datacolour[(sj0 + (height - 1 - si1) * width) * 3 + row_padded - (width % 4)];
            g = datacolour[(sj0 + (height - 1 - si1) * width) * 3 + 1 + row_padded - (width % 4)];
            r = datacolour[(sj0 + (height - 1 - si1) * width) * 3 + 2 + row_padded - (width % 4)];
            U += -0.148 * r - 0.291 * g + 0.439 * b + 128;
            V += 0.439 * r - 0.368 * g - 0.071 * b + 128;

            b = datacolour[(sj1 + (height - 1 - si1) * width) * 3 + row_padded - (width % 4)];
            g = datacolour[(sj1 + (height - 1 - si1) * width) * 3 + 1 + row_padded - (width % 4)];
            r = datacolour[(sj1 + (height - 1 - si1) * width) * 3 + 2 + row_padded - (width % 4)];
            U += -0.148 * r - 0.291 * g + 0.439 * b + 128;
            V += 0.439 * r - 0.368 * g - 0.071 * b + 128;

            U = U / 4;
            V = V / 4;

            //RGB to YUV
            if (U < 0)
                U = 0;
            else if (U > 255)
                U = 255;
            if (V < 0)
                V = 0;
            else if (V > 255)
                V = 255;

            YUV[sizeU + plus_size] = (unsigned char)U;
            YUV[sizeV + plus_size] = (unsigned char)V;
            plus_size++;
        }
    } 
}

BMPinfo::BMPinfo(const char* filename, unsigned char* YUV420, int& vW, int& vH) { //class constructor

    if (!read_BMP(filename, YUV420, vW, vH)) {
        std::cout << "Image size doesn't fit!" << std::endl;
        throw 2;
    }
}

