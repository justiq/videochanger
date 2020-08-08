//class VideoInfo  
#pragma once


class VideoInfo { 
private:
    long file_size; 
    int frame_count; 

    void openfile(const char*); 

    int inputInterface(); 
       
    void custom_size();
       
    bool resolution(int, int); 
        
    void frame_by_frame(unsigned char*, unsigned char*, int&, int&, int&, int&, int&, int&, int&, int&, int&, int);

public:
    VideoInfo(const char*);

        int width;      
        int height;    
        int frame_size; 
    
        void create_file(const char*, const char*, unsigned char*, int&, int&); 

};
