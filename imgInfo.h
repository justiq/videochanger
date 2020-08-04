// Класс BMPinfo

#include <iostream>
#include <vector>
#include <thread>

class BMPinfo {
private:
    struct RGB_pixel { //структура с информацией о пикселе
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    };

    unsigned int filesize;

    bool read_BMP(const char* filename, unsigned char* YUV420, int &vid_width, int &vid_height) {

        unsigned char* file_info = new unsigned char[54];
        FILE* image_file;

        if ((image_file = fopen(filename, "rb")) == NULL) {

            std::cout << "Can't open BMP image" << std::endl;
            throw 1;
        }

        fread(file_info, sizeof(unsigned char), 54, image_file); //чтение заголовка файла
        filesize = *(uint32_t*)&file_info[2];  //полный размер файла (бит)
        width = *(int*)&file_info[18];       //длина строки пикселей
        height = *(int*)&file_info[22];      //кол-во пиксельных строк

        delete[] file_info; //удаление данных заголовка файла

        if (width > vid_width || height > vid_height) {
            
            fclose(image_file);
            return false;
        }

        int datacolour_size = filesize - 54; //размер блока данных цвета + добавочные байты
        unsigned char* datacolour = new unsigned char[datacolour_size]; //массив с данными 

        fread(datacolour, sizeof(unsigned char), datacolour_size, image_file); //чтение данных пикселей BGR-24
        fclose(image_file);

        RGB_pixel RGB;
        RGB = { 0, 0, 0 };

        std::vector<std::thread> threads;
        std::vector<int> YUV;
        int row_padded = (height - 1) * (width % 4);

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {

                RGB.blue = datacolour[(j + (height - 1 - i) * width) * 3 + row_padded];
                RGB.green = datacolour[(j + (height - 1 - i) * width) * 3 + 1 + row_padded];
                RGB.red = datacolour[(j + (height - 1 - i) * width) * 3 + 2 + row_padded];
                //threads.push_back(std::thread(convert_RGB_to_YUV444, &YUV, std::ref(RGB)));
                convert_RGB_to_YUV444(&YUV, RGB);
            }
            row_padded -= (width % 4);
        }

        delete[] datacolour;    //удаление динамического массива с данными цвета

        //std::thread cnvrt(YUV_to_YUV420, YUV, std::ref(YUV420));
        YUV_to_YUV420(YUV, YUV420);
        YUV.clear();

        return true;
    }

    void convert_RGB_to_YUV444(std::vector<int>* YUV, struct RGB_pixel& RGB) { //конвертация строки YUV444

        int Y, U, V;

        Y = 0.257 * (int)RGB.red + 0.504 * (int)RGB.green + 0.098 * (int)RGB.blue + 16;
        U = -0.148 * (int)RGB.red - 0.291 * (int)RGB.green + 0.439 * (int)RGB.blue + 128;
        V = 0.439 * (int)RGB.red - 0.368 * (int)RGB.green - 0.071 * (int)RGB.blue + 128;

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

        YUV->push_back(Y);
        YUV->push_back(U);
        YUV->push_back(V);
    }

    void YUV_to_YUV420(std::vector<int>& YUV, unsigned char* YUV420) { //конвертация в YUV420

        int i, j;
        int sum;
        int si0, si1, sj0, sj1;
        int sizeU = height * width, sizeV, plus_size = 0;
        sizeV = 5 * height * width;
        sizeV /= 4;

        for (j = 0; j < height; j++) {
            for (i = 0; i < width; i++) {
                YUV420[i + j * width] = YUV[(i + j * width) * 3];
            }
        }

        for (j = 0; j < height; j += 2) {
            sj0 = j;
            sj1 = (j + 1 < height) ? j + 1 : j;

            for (i = 0; i < width; i += 2) {
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

    }

public:
    BMPinfo(const char* filename, unsigned char* YUV420, int &vW, int &vH) {

        if (!read_BMP(filename, YUV420, vW, vH)) {
            std::cout << "Image size doesn't fit!" << std::endl;
            //return;                                   //написать throw       DONT FORGET!
        }
    }

    int width;
    int height;
};
