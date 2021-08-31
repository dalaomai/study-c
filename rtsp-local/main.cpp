#include "rtsprecord.h"

void writeYuv420pToFile(const char *fileName, AVFrame *frame, int w, int h)
{
    FILE *outFp = fopen(fileName, "w+b");
    fwrite(frame->data[0], 1, w * h, outFp);
    fwrite(frame->data[1], 1, w * h / 4, outFp);
    fwrite(frame->data[2], 1, w * h / 4, outFp);
    fclose(outFp);
}

int main()
{
    // RTSPRecord recorder = RTSPRecord{
    //     "rtsp://admin:maizhiling456@192.168.1.5:554/stream2",
    //     "/root/study_c/.temp/record.mp4"};
    RTSPRecord recorder = RTSPRecord{
        "http://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/gear2/prog_index.m3u8",
        "/root/study_c/.temp/record.mp4"};
    recorder.start();
}