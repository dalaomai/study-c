extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/ffversion.h"
}

// #include "libavcodec/avcodec.h"
// #include "libavfilter/avfilter.h"
// #include "libavformat/avformat.h"
// #include "libavutil/avutil.h"
// #include "libavutil/ffversion.h"
// #include "libswresample/swresample.h"
// #include "libswscale/swscale.h"
// #include "libpostproc/postprocess.h"

// -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale
// -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale -lm -lpthread
int main()
{
    unsigned codecVer = avcodec_version();
    int ver_major, ver_minor, ver_micro;
    ver_major = (codecVer >> 16) & 0xff;
    ver_minor = (codecVer >> 8) & 0xff;
    ver_micro = (codecVer)&0xff;
    printf("FFmpeg :%s,avcodec:%d=%d.%d.%d", FFMPEG_VERSION, codecVer, ver_major, ver_minor, ver_micro);
}