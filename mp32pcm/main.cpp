#include <stdio.h>

extern "C"
{
#include "libavformat/avformat.h"
}

int main()
{
    const char *inFileName = "/root/study_c/.temp/file_example_MP3_5MG.mp3";
    const char *outFileName = "/root/study_c/.temp/result.pcm";

    FILE *outFile = fopen(outFileName, "w+b");
    if (!outFile)
    {
        printf("cannot open output file");
        return -1;
    }

    AVFormatContext *fmtCtx = avformat_alloc_context();
    AVCodecContext *codecCtx = NULL;
    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    int aStreamIndex = -1;

    do
    {
        if (avformat_open_input(&fmtCtx, inFileName, NULL, NULL) < 0)
        {
            printf("cannot open input file");
            break;
        }
        av_dump_format(fmtCtx, 0, inFileName, 0);

        if (avformat_find_stream_info(fmtCtx, NULL) < 0)
        {
            printf("cannot find stream");
            break;
        }

        for (int i = 0; i < fmtCtx->nb_streams; i++)
        {
            if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                aStreamIndex = i;
                break;
            }
        }
        if (aStreamIndex < 0)
        {
            printf("not find audio stream");
            break;
        }

        AVCodecParameters *aCodecPara = fmtCtx->streams[aStreamIndex]->codecpar;
        AVCodec *codec = avcodec_find_decoder(aCodecPara->codec_id);

        if (!codec)
        {
            printf("not find codec");
            break;
        }

        codecCtx = avcodec_alloc_context3(codec);
        if (avcodec_parameters_to_context(codecCtx, aCodecPara) < 0)
        {
            printf("alloc codec err");
            break;
        }
        codecCtx->pkt_timebase = fmtCtx->streams[aStreamIndex]->time_base;

        if (avcodec_open2(codecCtx, codec, NULL) < 0)
        {
            printf("open codec err");
            break;
        }

        while (av_read_frame(fmtCtx, pkt) >= 0)
        {
            if (pkt->stream_index == aStreamIndex)
            {
                if (avcodec_send_packet(codecCtx, pkt) >= 0)
                {
                    while (avcodec_receive_frame(codecCtx, frame) >= 0)
                    {
                        /*
                        双通道数据，planar模式和packed模式
                        
                        */
                        if (av_sample_fmt_is_planar(codecCtx->sample_fmt))
                        {
                            int numBytes = av_get_bytes_per_sample(codecCtx->sample_fmt);
                            for (int i = 0; i < frame->nb_samples; i++)
                            {
                                for (int ch = 0; ch < codecCtx->channels; ch++)
                                {
                                    fwrite((char *)frame->data[ch] + numBytes * i, 1, numBytes, outFile);
                                }
                            }
                        }
                    }
                }
            }
            av_packet_unref(pkt);
        }

    } while (0);
}