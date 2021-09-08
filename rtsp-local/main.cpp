#include "rtsprecord.h"

int main()
{
    // RTSPRecord recorder = RTSPRecord{
    //     "rtsp://admin:maizhiling456@192.168.1.5:554/stream1",
    //     "/root/study_c/.temp/record.mp4"};
    RTSPRecord recorder = RTSPRecord{
        "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8",
        "/root/study_c/.temp/record.mp4"};
    recorder.start();
}