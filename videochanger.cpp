#include <iostream>
#include "vidInfo.h"
#include "imgInfo.h"
int16_t inputinterface();

int main() {
    
    //станартный путь до используемых файлов
    char vid_path[150] = "videofile.yuv";
    char bmp_path[150] = "bmpfile.bmp";
    char newvid_path[150] = "changed_video.yuv";

    if (inputinterface() == 2) { //ввод новых путей до файлов
        std::cin.ignore(1000, '\n');
        std::cout << "Input video path:";
        std::cin.getline(vid_path, 150, '\n');
        std::cout << "Input image path:";
        std::cin.getline(bmp_path, 150, '\n');
        std::cout << "Output video path:";
        std::cin.getline(newvid_path, 150, '\n');
    }

    std::cout << "-----------------------------------------" << std::endl;

    try {
        VideoInfo vidYUV(vid_path); //конструктор класса VideoInfo

        unsigned char* img_buf = new unsigned char[vidYUV.frame_size]; //динамический массив с информацией изображения

        BMPinfo bmp(bmp_path, img_buf, vidYUV.width, vidYUV.height); //конструктор класса BMPinfo

        vidYUV.create_file(vid_path, newvid_path, img_buf, bmp.width, bmp.height); //метод класса, создающий новый файл

        delete[] img_buf; 
    }
    catch (int &a) {  //ошибки, связанные с открытием файлов
        if (a == 1) {
            std::cout << "Please, check input .bmp image file path!" << std::endl;
        }
        if (a == -1) {
            std::cout << "Please, check input .yuv video file path!" << std::endl;
        }
        if (a == 0) {
            std::cout << "Please, check output video file path!" << std::endl;
        }
        if (a == 2) {
            std::cout << "Please, change input .bmp image file or .yuv video!" << std::endl;
        }
    }
    catch (...) { //др. ошибки
        std::cout << "---!!Unexpected ERROR occured!!---" << std::endl;
    }
}

int16_t inputinterface() { // функция с принятием решения о путях с файлами
    int16_t choice;
    std::cout << "------This programm inserts .bmp image into .yuv video----- \n" << std::endl;

    std::cout << "What do you want to do with file paths? \n\n" <<
        "1. Use default file paths (input video: videofile.yuv, input image: bmpfile.bmp, output video: changed_video.yuv) \n" <<
        "2. enter your own file path? \n\n" << ">";

    while (!(std::cin >> choice)) { //прверка правильности введенных данных
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Bad data inputed! Try again: ";
    }

    while (choice < 1 || choice > 2) { //Присутствует ли выбранная функция
        std::cout << "No such function! Try again: ";
        while (!(std::cin >> choice)) { //прверка правильности введенных данных
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "\nBad data inputed! Try again: ";
        }
    }
    return choice;
}