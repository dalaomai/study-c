extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

void saveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[256];

    sprintf(szFilename, ".temp/video_frams/frame%d.ppm", iFrame);
    pFile = fopen(szFilename, "wb");
    if (pFile == NULL)
    {
        return;
    }

    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    for (int y = 0; y < height; y++)
    {
        fwrite(pFrame->data[0] + y * pFrame->linesize[0], 3, width, pFile);
    }
    fclose(pFile);
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
    AVFrame *yuvFrame = av_frame_alloc();
    AVFrame *rgbFrame = av_frame_alloc();

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

        struct SwsContext *img_ctx = sws_getContext(
            codecCtx->width, codecCtx->height, codecCtx->pix_fmt,
            codecCtx->width, codecCtx->height, AV_PIX_FMT_RGB32,
            SWS_BICUBIC, NULL, NULL, NULL);

        int numBytes = av_image_get_buffer_size(
            AV_PIX_FMT_RGB32, codecCtx->width, codecCtx->height, 1);
        unsigned char *out_buffer = (unsigned char *)av_malloc(numBytes * sizeof(unsigned char));

        int i = 0;
        pkt = av_packet_alloc();
        av_new_packet(pkt, codecCtx->width * codecCtx->height);

        av_image_fill_arrays(
            rgbFrame->data, rgbFrame->linesize,
            out_buffer, AV_PIX_FMT_RGB32,
            codecCtx->width, codecCtx->height, 1);

        while (av_read_frame(fmtCtx, pkt) >= 0)
        {
            if (pkt->stream_index == videoStreamIndex)
            {
                if (avcodec_send_packet(codecCtx, pkt) == 0)
                {
                    while (avcodec_receive_frame(codecCtx, yuvFrame) == 0)
                    {
                        sws_scale(img_ctx,
                                  (const uint8_t *const *)yuvFrame->data,
                                  yuvFrame->linesize, 0, codecCtx->height, rgbFrame->data, rgbFrame->linesize);
                        saveFrame(rgbFrame, codecCtx->width, codecCtx->height, i);
                        i++;
                    }
                }
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