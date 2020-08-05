// Класс BMPinfo

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

class BMPinfo {
private:

    unsigned int filesize;
    std::mutex mtx;

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

        std::vector<std::thread> threads;

        unsigned char* YUV = new unsigned char[datacolour_size];

        int row_padded = (height - 1) * (width % 4);

        for (int i = 0; i < height; i++) {
               
            threads.emplace_back(std::thread(&BMPinfo::convert_RGB_to_YUV444, this, 
                                YUV, datacolour, row_padded, i));
            row_padded -= (width % 4);
        }

        for (int x = 0; x < height; x++) {
            threads[x].join();
        }


        threads.emplace_back(std::thread(&BMPinfo::YUV_to_YUV420, this, YUV, YUV420));
        threads[height].join();

        delete[] YUV;
        threads.clear();
        delete[] datacolour;    //удаление динамического массива с данными цвета
        return true;
    }

    void convert_RGB_to_YUV444(unsigned char* YUV, unsigned char *datacolour, const int &row_padded, int i) { //конвертация строки YUV444

        
        int Y, U, V;
        int r, g, b;
        for (int j = 0; j < width; j++) {

            b = datacolour[(j + (height - 1 - i) * width) * 3 + row_padded];
            g = datacolour[(j + (height - 1 - i) * width) * 3 + 1 + row_padded];
            r = datacolour[(j + (height - 1 - i) * width) * 3 + 2 + row_padded];
            
            
            Y = 0.257 * (int)r + 0.504 * (int)g + 0.098 * (int)b + 16;
            U = -0.148 * (int)r - 0.291 * (int)g + 0.439 * (int)b + 128;
            V = 0.439 * (int)r - 0.368 * (int)g - 0.071 * (int)b + 128;

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

    void YUV_to_YUV420(unsigned char* YUV, unsigned char* YUV420) { //конвертация в YUV420

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
            throw 2;
        }
    }

    int width;
    int height;
};
