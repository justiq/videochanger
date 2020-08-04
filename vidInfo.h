//����� VideoInfo  

#include <iostream>

//������� �������� �������� ����� �����
#define _1080p 1920, 1080
#define _720p 1280, 720
#define VGA 640, 480
#define WVGA 832, 480
#define CIF 352, 288
#define QCIF 176, 144
#define SMALL 192, 256

class VideoInfo { //����� � ����������� � ������� �����
private:
    long file_size; // ������ �����
    int frame_count; // ���������� ������ �����

    void openfile(const char* filename) { //����� ����������� ���� �����

        FILE* video_file;

        if ((video_file = fopen(filename, "rb")) == NULL) {
            std::cout << "Can't open YUV video" << std::endl;
            throw -1;
        }

        fseek(video_file, 0, SEEK_END);
        file_size = ftell(video_file);

        fclose(video_file);

        int choise = inputInterface();
        bool isgood = false;

        while (!isgood) {
            switch (choise) {
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
                choise = inputInterface();
            }
        }
    }

    int inputInterface() {
        int choise;

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

        while (!(std::cin >> choise)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "Bad data inputed! Try again: ";
        }

        while (choise <= 0 || choise > 8) {
            if (choise == 0) {
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
                std::cout << "No such function! Try again: ";

            while (!(std::cin >> choise)) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                std::cout << "\nBad data inputed! Try again: ";
            }
        }
        if (choise == 8) {
            custom_size();
        }
        return choise;
    }

    void custom_size() {
        std::cout << "Your Width: ";

        while (!(std::cin >> width)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "\nIncorect Input of Width. \n" <<
                "Your Width: ";
        }

        std::cout << "Your Height: ";

        while (!(std::cin >> height)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "\nIncorect Input of Width. \n" <<
                "Your Height: ";
        }
    }

    bool resolution(int wdt, int hght) { //����� ����������� ������������ ��������� ������
        frame_size = 3 * wdt * hght;
        frame_size /= 2;
        if ((file_size % frame_size) == 0) {
            width = wdt;
            height = hght;
            frame_count = file_size / frame_size; //���������� ����������� ������
        }
        else {
            std::cout << "Wrong format! Please choose another one!"<< std::endl; 
            return false;
        }
        return true;
    }

public:
    VideoInfo(const char* filename) { //����������� ������
        openfile(filename);
    }
        int width;      //������ ����� �����
        int height;     //������ ����� �����
        int frame_size; //���������� ������� ������ �����
    
        void create_file(const char *vid_filename, const char* newV_filename, unsigned char *img_buf, int& img_width, int& img_height) {

            int i = 0, j = 0, k = 0;

            int vidY_size = width * height; //������ Y ������� ����� �����
            int vidYU_size = 5 * width * height; //������ YU ������� ����� �����
            vidYU_size /= 4;

            int vid_widthd2 = width / 2; //�������� ������ �����

            int imgY_size = img_width * img_height; //������ Y ������� ��������
            int imgYU_size = 5 * img_width * img_height; //������ YU ������� ��������
            imgYU_size /= 4;
            
            int img_heightd2 = img_height / 2; //�������� ������ ��������
            int img_widthd2 = img_width / 2; //�������� ������ ��������

            FILE* video_file;
            if ((video_file = fopen(vid_filename, "rb")) == NULL) {
                std::cout << "Can't open YUV video" << std::endl;
                throw - 1;
            }

            unsigned char* vid_info = new unsigned char[file_size]; // ���������� �� �����
            fread(vid_info, sizeof(unsigned char), file_size, video_file); //������ ���������� � �����
            fclose(video_file);

            for (i = 0; i < file_size; i += frame_size) { //����, ���������� ��� ����� �����

                for (j = 0; j < img_height; j++) { //����� ���������� Y ����� �� Y ���������
                    for (k = 0; k < img_width; k++) {
                        vid_info[i + k + (j * width)] = img_buf[k + (j * img_width)];
                    }
                }
                
                for (j = 0; j < img_heightd2; j++) { //����� ���������� UV ����� �� UV ���������
                    for (k = 0; k < img_widthd2; k++) {
                        vid_info[i + vidY_size + k + (j * vid_widthd2)] = img_buf[imgY_size + k + (j * img_widthd2)];
                        vid_info[i + vidYU_size + k + (j * vid_widthd2)] = img_buf[imgYU_size + k + (j * img_widthd2)];
                    }
                }
            }

            FILE* new_file;
            if ((new_file = fopen(newV_filename, "wb")) == NULL) { //�������� ������ �����
                std::cout << "Can't open yuv image" << std::endl;
                throw 0;
            }

            fwrite(vid_info, sizeof(unsigned char), file_size, new_file); //������ � ���� ���������� ����������
            fclose(new_file);
        }
};