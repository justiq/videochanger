//Класс VideoInfo  

#include <iostream>
#include <vector>
#include <thread>

//типовые значения размеров кадра видео
#define _1080p 1920, 1080
#define _720p 1280, 720
#define VGA 640, 480
#define WVGA 832, 480
#define CIF 352, 288
#define QCIF 176, 144
#define SMALL 192, 256

class VideoInfo { //Класс с информацией о формате видео
private:
    long file_size; // Раземр файла
    int frame_count; // Количество кадров видео

    void openfile(const char* filename) { //метод открывающий файл видео

        FILE* video_file;

        if ((video_file = fopen(filename, "rb")) == NULL) {
            std::cout << "Can't open YUV video" << std::endl;
            throw -1;
        }

        //чтение информации о размера файла
        fseek(video_file, 0, SEEK_END);
        file_size = ftell(video_file);

        fclose(video_file);

        int choice = inputInterface(); //выбор качества видео
        bool isgood = false; //булева переменная, необходимая дляпроверки того, подходит ли выбранное качество размеру видео

        while (!isgood) {
            switch (choice) { //Проверка и установка выбранного качества видео
            case 1:
                isgood = resolution(_1080p);
                break;
            case 2:
                isgood = resolution(_720p);
                break;
            case 3:
                isgood = resolution(VGA);
                break;
            case 4:
                isgood = resolution(WVGA);
                break;
            case 5:
                isgood = resolution(CIF);
                break;
            case 6:
                isgood = resolution(QCIF);
                break;
            case 7:
                isgood = resolution(SMALL);
                break;
            case 8:
                isgood = resolution(width, height);
                break;
            }
            
            if (!isgood) {
                choice = inputInterface();
            }
        }
    }

    int inputInterface() { //выбор качества видео
        int choice;

        std::cout << "Chose one of the resolutions below or print your own: \n" <<
            "1. 1080p (1920*1080) \n" <<
            "2. 720p (1280*720) \n" <<
            "3. VGA (640*480) \n" <<
            "4. WVGA (832*480) \n" <<
            "5. CIF/SIF(625) (352*288) \n" <<
            "6. QCIF (176*144) \n" <<
            "7. SMALL (192*256) \n" <<
            "8. Custom size \n" <<
            "0. Clear console \n \n" <<
            "> ";

        while (!(std::cin >> choice)) { //прверка правильности введенных данных
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "Bad data inputed! Try again: ";
        }

        while (choice <= 0 || choice > 8) { //присутствует ли данная опция среди предложенных
            if (choice == 0) {
                system("cls");
                std::cout << "Chose one of the resolutions below or print your own: \n" <<
                    "1. 1080p (1920*1080) \n" <<
                    "2. 720p (1280*720) \n" <<
                    "3. VGA (640*480) \n" <<
                    "4. WVGA (832*480) \n" <<
                    "5. CIF/SIF(625) (352*288) \n" <<
                    "6. QCIF (176*144) \n" <<
                    "7. SMALL (192*256) \n" <<
                    "8. Custom size \n" <<
                    "0. Clear console \n \n" <<
                    "> ";
            }
            else
                std::cout << "Option doesn't exist! Try again: ";

            while (!(std::cin >> choice)) { //прверка правильности введенных данных
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                std::cout << "\nBad data inputed! Try again: ";
            }
        }
        if (choice == 8) { //Ручное изменение переменных width и height 
            custom_size();
        }
        return choice;
    }

    void custom_size() { //Ручное изменение переменных width и height 
        std::cout << "Your Width: ";

        while (!(std::cin >> width)) { //прверка правильности введенных данных
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "\nIncorect Input of Width. \n" <<
                "Your Width: ";
        }

        std::cout << "Your Height: ";

        while (!(std::cin >> height)) { //прверка правильности введенных данных
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "\nIncorect Input of Width. \n" <<
                "Your Height: ";
        }
    }

    bool resolution(int wdt, int hght) { //метод проверяющий правильность внесенных данных
        frame_size = 3 * wdt * hght; //Значение переменной размера кадра
        frame_size /= 2;

        if ((file_size % frame_size) == 0) { //Проверка соответствия размера видео его качеству 
            width = wdt;
            height = hght;
            frame_count = file_size / frame_size; //вычисление колличества кадров
        }
        else {
            std::cout << "Wrong format! Please choose another one!"<< std::endl; 
            return false;
        }
        return true;
    }

    void frame_by_frame(unsigned char *vid_info, unsigned char* img_buf, int& img_width, int& img_height, 
                        int &img_heightd2, int &img_widthd2, int &vidY_size, int &vidYU_size, int &vid_widthd2,
                        int &imgY_size, int &imgYU_size, int i) {

        int j = 0, k = 0;

        for (j = 0; j < img_height; j++) { //циклы заменяющие Y видео на Y какртинки
            for (k = 0; k < img_width; k++) {
                vid_info[i + k + (j * width)] = img_buf[k + (j * img_width)];
            }
        }

        for (j = 0; j < img_heightd2; j++) { //циклы заменяющие UV видео на UV какртинки
            for (k = 0; k < img_widthd2; k++) {
                vid_info[i + vidY_size + k + (j * vid_widthd2)] = img_buf[imgY_size + k + (j * img_widthd2)];
                vid_info[i + vidYU_size + k + (j * vid_widthd2)] = img_buf[imgYU_size + k + (j * img_widthd2)];
            }
        }
    }

public:
    VideoInfo(const char* filename) { //конструктор класса
        openfile(filename);
    }
        int width;      //ШИРИНА кадра видео
        int height;     //ВЫСОТА кадра видео
        int frame_size; //переменная размера одного кадра
    
        void create_file(const char *vid_filename, const char* newV_filename, unsigned char *img_buf, int& img_width, int& img_height) {

            int i = 0;

            int vidY_size = width * height; //Размер Y области кадра видео
            int vidYU_size = 5 * width * height; //Размер YU области кадра видео
            vidYU_size /= 4;

            int vid_widthd2 = width / 2; //половина ширины видео

            int imgY_size = img_width * img_height; //Размер Y области картинки
            int imgYU_size = 5 * img_width * img_height; //Размер YU области картинки
            imgYU_size /= 4;
            
            int img_heightd2 = img_height / 2; //половина высоты картинки
            int img_widthd2 = img_width / 2; //половина ширины картинки

            FILE* video_file;
            if ((video_file = fopen(vid_filename, "rb")) == NULL) {
                std::cout << "Can't open YUV video" << std::endl;
                throw - 1;
            }

            unsigned char* vid_info = new unsigned char[file_size]; // Информация из видео
            fread(vid_info, sizeof(unsigned char), file_size, video_file); //чтение информации о видео
            fclose(video_file);

            std::vector<std::thread> threads; //вектор потоков

            for (i = 0; i < file_size; i += frame_size) { //цикл, проходящий все кадры видео

                threads.emplace_back(std::thread(&VideoInfo::frame_by_frame, this, vid_info, img_buf, 
                                    std::ref(img_width), std::ref(img_height), std::ref(img_heightd2), 
                                    std::ref(img_widthd2), std::ref(vidY_size), std::ref(vidYU_size), 
                                    std::ref(vid_widthd2), std::ref(imgY_size), std::ref(imgYU_size), i)); 
            }

            for (i = 0; i < frame_count; i ++) {
                threads[i].join();
            }

            // создание нового файла
            FILE* new_file; 
            if ((new_file = fopen(newV_filename, "wb")) == NULL) { //открытие нового файла
                std::cout << "Can't open yuv image" << std::endl;
                throw 0;
            }

            fwrite(vid_info, sizeof(unsigned char), file_size, new_file); //запись в файл измененной информации
            fclose(new_file);
        }
};