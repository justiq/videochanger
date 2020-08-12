//class VideoInfo  

#include <iostream>
#include <vector>
#include <thread>
#include "vidInfo.h"

//typical resolutions
#define _1080p 1920, 1080
#define _720p 1280, 720
#define VGA 640, 480
#define WVGA 832, 480
#define CIF 352, 288
#define QCIF 176, 144
#define SMALL 192, 256

void VideoInfo::openfile(const char* filename) { //video file openning method

    FILE* video_file;

    if ((video_file = fopen(filename, "rb")) == NULL) {
        std::cout << "Can't open YUV video" << std::endl;
        throw - 1;
    }

    //information about file size
    fseek(video_file, 0, SEEK_END);
    file_size = ftell(video_file);

    fclose(video_file);

    int choice = inputInterface(); //video resolution choice
    bool isgood = false; //is resolution fits

    while (!isgood) {
        switch (choice) {
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

int VideoInfo::inputInterface() { //video resolution choice
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

    while (!(std::cin >> choice)) { // is data good?
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Bad data inputed! Try again: ";
    }

    while (choice <= 0 || choice > 8) { // does option exists?
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

        while (!(std::cin >> choice)) { // is data good?
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "\nBad data inputed! Try again: ";
        }
    }
    if (choice == 8) { //manual set of width & height 
        custom_size();
    }
    return choice;
}

void VideoInfo::custom_size() { //manual set of width & height 
    std::cout << "Your Width: ";

    while (!(std::cin >> width)) { //is data good?
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "\nIncorect Input of Width. \n" <<
            "Your Width: ";
    }

    std::cout << "Your Height: ";

    while (!(std::cin >> height)) { //is data good?
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "\nIncorect Input of Width. \n" <<
            "Your Height: ";
    }
}

bool VideoInfo::resolution(int wdt, int hght) { //resolution check method
    frame_size = 3 * wdt * hght;
    frame_size /= 2;

    if ((file_size % frame_size) == 0) {
        width = wdt;
        height = hght;
        frame_count = file_size / frame_size;
    }
    else {
        std::cout << "Wrong format! Please choose another one!" << std::endl;
        return false;
    }
    return true;
}

void VideoInfo::frame_by_frame(unsigned char* vid_info, unsigned char* img_buf, int& img_width, int& img_height,
    int& img_heightd2, int& img_widthd2, int& vidY_size, int& vidYU_size, int& vid_widthd2,
    int& imgY_size, int& imgYU_size) {

    int j = 0, k = 0;

    for (j = 0; j < img_height; j++) { //Changes Y on video
        for (k = 0; k < img_width; k++) {
            vid_info[k + (j * width)] = img_buf[k + (j * img_width)];
        }
    }

    for (j = 0; j < img_heightd2; j++) { //Changes UV on video
        for (k = 0; k < img_widthd2; k++) {
            vid_info[vidY_size + k + (j * vid_widthd2)] = img_buf[imgY_size + k + (j * img_widthd2)];
            vid_info[vidYU_size + k + (j * vid_widthd2)] = img_buf[imgYU_size + k + (j * img_widthd2)];
        }
    }
}


VideoInfo::VideoInfo(const char* filename) { //class constuctor
    openfile(filename);
}

void VideoInfo::create_file(const char* vid_filename, const char* newV_filename, unsigned char* img_buf, int& img_width, int& img_height) {

    int i = 0;

    int vidY_size = width * height; //Size of Y in a video frame
    int vidYU_size = 5 * width * height; //Size of UV in a video frame
    vidYU_size /= 4;

    int vid_widthd2 = width / 2; //half of a video frame width

    int imgY_size = img_width * img_height; //Size of Y in an image
    int imgYU_size = 5 * img_width * img_height; //Size of UV in an image
    imgYU_size /= 4;

    int img_heightd2 = img_height / 2; //half of an image height
    int img_widthd2 = img_width / 2; //half of an image width

    FILE* video_file;
    if ((video_file = fopen(vid_filename, "rb")) == NULL) {
        std::cout << "Can't open YUV video" << std::endl;
        throw - 1;
    }

    unsigned char* vid_info = new unsigned char[frame_size]; //video infoemation

    // New file creation
    FILE* new_file;
    if ((new_file = fopen(newV_filename, "wb")) == NULL) {
        std::cout << "Can't open yuv image" << std::endl;
        throw 0;
    }

    std::vector<std::thread> threads; //threads vector

    for (i = 0; i < frame_count; i++) { //frame by frame cycle 
        fread(vid_info, sizeof(unsigned char), frame_size, video_file); //reading video info

        threads.emplace_back(std::thread(&VideoInfo::frame_by_frame, this, vid_info, img_buf,
            std::ref(img_width), std::ref(img_height), std::ref(img_heightd2),
            std::ref(img_widthd2), std::ref(vidY_size), std::ref(vidYU_size),
            std::ref(vid_widthd2), std::ref(imgY_size), std::ref(imgYU_size)));
        threads[i].join();

        fwrite(vid_info, sizeof(unsigned char), frame_size, new_file);
    }

    fclose(video_file);
    fclose(new_file);
    threads.clear();
    delete[] vid_info;
}
