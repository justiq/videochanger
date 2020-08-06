// Класс BMPinfo

#include <iostream>
#include <vector>
#include <thread>

class BMPinfo {
private:

    unsigned int filesize;

    bool read_BMP(const char* filename, unsigned char* YUV420, int &vid_width, int &vid_height) {

        unsigned char* file_info = new unsigned char[54]; //массив с заголовком изображения
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

        if (width > vid_width || height > vid_height) { // проверка соответствия размера изображения размеру кадра видео
            fclose(image_file);
            return false;
        }

        int datacolour_size = filesize - 54; //размер блока данных цвета + добавочные байты
        unsigned char* datacolour = new unsigned char[datacolour_size]; //массив с данными 

        fread(datacolour, sizeof(unsigned char), datacolour_size, image_file); //чтение данных пикселей BGR-24
        fclose(image_file);

        std::vector<std::thread> threads; //вектор потоков
        unsigned char* YUV = new unsigned char[datacolour_size]; //динамический массив с цветами формата YUV444
        int row_padded = (height - 1) * (width % 4);            //вычисление подложки BMP файла

        for (int i = 0; i < height; i++) {
            threads.emplace_back(std::thread(&BMPinfo::convert_RGB_to_YUV444, this,   
                                YUV, datacolour, row_padded, i));   //создание потока
            row_padded -= (width % 4);                              //уменьшение размера подложки
        }

        for (int x = 0; x < height; x++) { //ожидание завершения потоков 
            threads[x].join();
        }

        threads.clear();
        delete[] datacolour;    //удаление динамического массива с данными цвета

        YUV_to_YUV420(YUV, YUV420);
        
        delete[] YUV;

        return true;
    }

    void convert_RGB_to_YUV444(unsigned char* YUV, unsigned char *datacolour, const int &row_padded, int i) { //конвертация строки YUV444

        
        int Y, U, V;
        int r, g, b;
        for (int j = 0; j < width; j++) {
             
            //значения краснего, синего и зеленого цветов
            b = datacolour[(j + (height - 1 - i) * width) * 3 + row_padded];
            g = datacolour[(j + (height - 1 - i) * width) * 3 + 1 + row_padded];
            r = datacolour[(j + (height - 1 - i) * width) * 3 + 2 + row_padded];
            
            //конвертаниця RGB в YUV
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

    void YUV_to_YUV420(unsigned char* YUV, unsigned char* YUV420) { //конвертация в YUV420

        int j;
        int sizeU = height * width, sizeV;
        sizeV = 5 * height * width;
        sizeV /= 4;

        std::vector<std::thread> threads; //вектор потоков

        for (j = 0; j < height; j++) {
            threads.emplace_back(std::thread(&BMPinfo::Y_in_YUV420, this, YUV, YUV420, j));
        }

        for (j = 0; j < height; j += 2) {
            threads.emplace_back(std::thread(&BMPinfo::UV_in_YUV420, this, YUV, YUV420, j, 
                        std::ref(sizeU), std::ref(sizeV)));
        }

        for (int x = 0; x < threads.size(); x++) {
            threads[x].join();
        }
        threads.clear();
    }

    void Y_in_YUV420(unsigned char* YUV, unsigned char* YUV420, int j) { //перезапись части Y из 444 в 420
        for (int i = 0; i < width; i++) {
            YUV420[i + j * width] = YUV[(i + j * width) * 3];
        }
    }

    void UV_in_YUV420(unsigned char* YUV, unsigned char* YUV420, int j, int& sizeU, int& sizeV) { //перезапись части UV из 444 в 420 
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

public:
    BMPinfo(const char* filename, unsigned char* YUV420, int &vW, int &vH) { //конструктор класса

        if (!read_BMP(filename, YUV420, vW, vH)) {
            std::cout << "Image size doesn't fit!" << std::endl;
            throw 2;
        }
    }

    int width; //ширина изображения
    int height; //высота изображения
};
