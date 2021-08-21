extern "C"
{
#include "libavformat/avformat.h"
}

void memory_dump(void *ptr, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        if (i % 8 == 0 && i != 0)
            printf(" ");
        if (i % 16 == 0 && i != 0)
            printf("\n");
        printf("%02x ", *((uint8_t *)ptr + i));
    }
    printf("\n");
}

int main()
{
    char filePath[] = "/root/study_c/.temp/sample_960x540.mkv";
    int videoStreamIndex = -1;
    int ret = 0;

    AVFormatContext *fmtCtx = NULL;
    AVPacket *pkt = NULL;
    AVCodecContext *codecCtx = NULL;
    AVCodecParameters *avCodecPara = NULL;
    AVCodec *codec = NULL;

    do
    {
        fmtCtx = avformat_alloc_context();

        if ((ret = avformat_open_input(&fmtCtx, filePath, NULL, NULL)) != 0)
        {
            printf("open file failue");
            break;
        }

        if ((ret = avformat_find_stream_info(fmtCtx, NULL)) < 0)
        {
            printf("find video stream failue");
            break;
        }

        for (unsigned int i = 0; i < fmtCtx->nb_streams; i++)
        {
            if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                videoStreamIndex = i;
                break;
            }
        }

        if (videoStreamIndex == -1)
        {
            printf("not find video stream");
            break;
        }

        av_dump_format(fmtCtx, 0, filePath, 0);

        avCodecPara = fmtCtx->streams[videoStreamIndex]->codecpar;
        codec = avcodec_find_decoder(avCodecPara->codec_id);
        if (codec == NULL)
        {
            printf("cannot finde decoder\n");
            break;
        }

        codecCtx = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codecCtx, avCodecPara);
        if (codecCtx == NULL)
        {
            printf("Cannot alloc codec context");
            break;
        }

        if ((ret = avcodec_open2(codecCtx, codec, NULL)) < 0)
        {
            printf("open decoder failue");
            break;
        }

        int i = 0;
        pkt = av_packet_alloc();
        av_new_packet(pkt, codecCtx->width * codecCtx->height);

        while (av_read_frame(fmtCtx, pkt) >= 0)
        {
            if (pkt->stream_index == videoStreamIndex)
            {
                i++;
            }
            av_packet_unref(pkt);
        }
        printf("there are %d frames total", i);

    } while (0);

    av_packet_free(&pkt);
    avcodec_close(codecCtx);
    // avcodec_parameters_free(&avCodecPara);
    avformat_close_input(&fmtCtx);
    avformat_free_context(fmtCtx);
    // av_free(codec);
}