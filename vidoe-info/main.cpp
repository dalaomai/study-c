extern "C"
{
#include "libavformat/avformat.h"
}

int main()
{
    AVFormatContext *fmt_ctx = avformat_alloc_context();
    char fileName[] = "/root/study_c/.temp/sample_960x540.mkv";
    int ret = 0;
    do
    {
        av_register_all();
        if ((ret = avformat_open_input(&fmt_ctx, fileName, NULL, NULL)) < 0)
        {
            break;
        }

        if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0)
        {
            printf("Cannot find stream info");
            break;
        }

        av_dump_format(fmt_ctx, 0, fileName, 0);

    } while (0);

    avformat_close_input(&fmt_ctx);
}