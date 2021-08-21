extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

void to_yuv420p()
{
    char filePath[] = "/root/study_c/.temp/sample_960x540.mkv";
    char outFilePath[] = "/root/study_c/.temp/result.yuv";
    FILE *outFp = fopen(outFilePath, "w+b");
    int videoStreamIndex = -1;
    int ret = 0;

    AVFormatContext *fmtCtx = NULL;
    AVPacket *pkt = NULL;
    AVCodecContext *codecCtx = NULL;
    AVCodecParameters *avCodecPara = NULL;
    AVCodec *codec = NULL;
    unsigned char *out_buffer;
    AVFrame *yuvFrame = av_frame_alloc();
    AVFrame *yuv420pFrame = av_frame_alloc();
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

        int i, n = 0;
        int w = codecCtx->width;
        int h = codecCtx->height;
        int send_ret = 0;

        struct SwsContext *img_ctx = sws_getContext(w, h, codecCtx->pix_fmt, w, h, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, w, h, 1);
        out_buffer = (unsigned char *)av_malloc(numBytes * sizeof(unsigned char));

        pkt = av_packet_alloc();
        av_new_packet(pkt, codecCtx->width * codecCtx->height);
        av_image_fill_arrays(yuv420pFrame->data, yuv420pFrame->linesize, out_buffer, AV_PIX_FMT_YUV420P, w, h, 1);

        while (av_read_frame(fmtCtx, pkt) >= 0)
        {
            if (pkt->stream_index == videoStreamIndex)
            {
                n++;
                if (avcodec_send_packet(codecCtx, pkt) == 0)
                {
                    while (avcodec_receive_frame(codecCtx, yuvFrame) == 0)
                    {
                        sws_scale(img_ctx, (const uint8_t *const *)yuvFrame->data, yuvFrame->linesize, 0, h, yuv420pFrame->data, yuv420pFrame->linesize);
                        fwrite(yuv420pFrame->data[0], 1, w * h, outFp);
                        fwrite(yuv420pFrame->data[1], 1, w * h / 4, outFp);
                        fwrite(yuv420pFrame->data[2], 1, w * h / 4, outFp);
                        i++;
                    }
                }
            }
            av_packet_unref(pkt);
        }
        printf("there are %d frames and receive %d frames", n, i);

    } while (0);

    fclose(outFp);

    av_packet_free(&pkt);
    avcodec_close(codecCtx);
    // avcodec_parameters_free(&avCodecPara);
    avformat_close_input(&fmtCtx);
    avformat_free_context(fmtCtx);
    // av_free(codec);
}

void to_yuv420sp()
{
    char filePath[] = "/root/study_c/.temp/sample_960x540.mkv";
    char outFilePath[] = "/root/study_c/.temp/result.yuv";
    FILE *outFp = fopen(outFilePath, "w+b");
    int videoStreamIndex = -1;
    int ret = 0;

    AVFormatContext *fmtCtx = NULL;
    AVPacket *pkt = NULL;
    AVCodecContext *codecCtx = NULL;
    AVCodecParameters *avCodecPara = NULL;
    AVCodec *codec = NULL;
    unsigned char *out_buffer;
    AVFrame *yuvFrame = av_frame_alloc();
    AVFrame *nv12Frame = av_frame_alloc();

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

        int i, n = 0;
        int w = codecCtx->width;
        int h = codecCtx->height;
        int send_ret = 0;

        struct SwsContext *img_ctx = sws_getContext(
            w, h, codecCtx->pix_fmt, w, h, AV_PIX_FMT_NV12, SWS_BICUBIC, NULL, NULL, NULL);

        int numBytes = av_image_get_buffer_size(AV_PIX_FMT_NV12, w, h, 1);
        out_buffer = (unsigned char *)av_malloc(numBytes * sizeof(unsigned char));

        pkt = av_packet_alloc();
        av_new_packet(pkt, codecCtx->width * codecCtx->height);
        av_image_fill_arrays(nv12Frame->data, nv12Frame->linesize, out_buffer, AV_PIX_FMT_NV12, w, h, 1);

        while (av_read_frame(fmtCtx, pkt) >= 0)
        {
            if (pkt->stream_index == videoStreamIndex)
            {
                n++;
                if (avcodec_send_packet(codecCtx, pkt) == 0)
                {
                    while (avcodec_receive_frame(codecCtx, yuvFrame) == 0)
                    {
                        sws_scale(
                            img_ctx,
                            (const uint8_t *const *)yuvFrame->data,
                            yuvFrame->linesize, 0, h, nv12Frame->data,
                            nv12Frame->linesize);
                        fwrite(nv12Frame->data[0], 1, w * h, outFp);
                        fwrite(nv12Frame->data[1], 1, w * h / 2, outFp);
                        i++;
                    }
                }
            }
            av_packet_unref(pkt);
        }
        printf("there are %d frames and receive %d frames", n, i);

    } while (0);

    fclose(outFp);

    av_packet_free(&pkt);
    avcodec_close(codecCtx);
    // avcodec_parameters_free(&avCodecPara);
    avformat_close_input(&fmtCtx);
    avformat_free_context(fmtCtx);
    // av_free(codec);
}

int main()
{
    to_yuv420p();
}
