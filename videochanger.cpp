#include <iostream>
#include "vidInfo.h"
#include "imgInfo.h"

#define vid_path "videofile.yuv"
#define bmp_path "bmpfile.bmp"
#define newvid_path "changed_video.yuv"

int main() {
    
    std::cout << "------This programm inserts .bmp image into .yuv video----- \n" << std::endl;

    try {
        VideoInfo vidYUV(vid_path);

        unsigned char* img_buf = new unsigned char[vidYUV.frame_size];

        BMPinfo bmp(bmp_path, img_buf, vidYUV.width, vidYUV.height);

        vidYUV.create_file(vid_path, newvid_path, img_buf, bmp.width, bmp.height);

        delete[] img_buf;
    }
    catch (int &a) {
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
    catch (...) {
        std::cout << "---!!Unexpected ERROR occured!!---" << std::endl;
    }
}
