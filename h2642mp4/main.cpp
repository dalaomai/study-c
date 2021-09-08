extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
}

int main()
{
    int frame_index = 0;
    int inStreamIndex = -1, outStreamIndex = -1;
    const char *inFileName = "/root/study_c/.temp/result.h264";
    const char *outFileName = "/root/study_c/.temp/result.mp4";

    AVFormatContext *inFmtCtx = NULL, *outFmtCtx = NULL;
    AVCodecParameters *inCodecPara = NULL, *outCodecPara = NULL;
    AVStream *inStream = NULL, *outStream = NULL;
    AVCodec *outCodec = NULL;
    AVCodecContext *outCodecCtx = NULL;

    AVPacket *pkt = av_packet_alloc();

    do
    {
        if (avformat_open_input(&inFmtCtx, inFileName, NULL, NULL) < 0)
        {
            printf("cannot open input file");
            break;
        }

        if (avformat_find_stream_info(inFmtCtx, NULL) < 0)
        {
            printf("cannot find stream in input file");
            break;
        }

        for (size_t i = 0; i < inFmtCtx->nb_streams; i++)
        {
            if (inFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                inStreamIndex = i;
                break;
            }
        }
        if (inStreamIndex == -1)
        {
            printf("cannot find video stream in input file");
            break;
        }

        inStream = inFmtCtx->streams[inStreamIndex];
        inCodecPara = inStream->codecpar;

        // 输出视频信息
        av_dump_format(inFmtCtx, 0, inFileName, 0);

        if (avformat_alloc_output_context2(&outFmtCtx, NULL, NULL, outFileName) < 0)
        {
            printf("cannot alloc output file");
            break;
        }

        if (avio_open(&outFmtCtx->pb, outFileName, AVIO_FLAG_READ_WRITE) < 0)
        {
            printf("cannot open output file");
            break;
        }

        if (!(outStream = avformat_new_stream(outFmtCtx, NULL)))
        {
            printf("cannot allocat stream");
            break;
        }

        outStream->time_base.den = 25;
        outStream->time_base.num = 1;
        outStreamIndex = outStream->index;

        outCodec = avcodec_find_encoder(inCodecPara->codec_id);
        if (outCodec == NULL)
        {
            printf("cannot find encoder");
            break;
        }

        outCodecCtx = avcodec_alloc_context3(outCodec);
        outCodecPara = outFmtCtx->streams[outStream->index]->codecpar;
        if (avcodec_parameters_copy(outCodecPara, inCodecPara) < 0)
        {
            printf("cannot copy codec para");
            break;
        }
        if (avcodec_parameters_to_context(outCodecCtx, outCodecPara) < 0)
        {
            printf("cannot alloc codec ctx from para");
            break;
        }
        outCodecCtx->time_base.den = 25;
        outCodecCtx->time_base.num = 1;
        // outCodecCtx->pix_fmt = outCodec->pix_fmts[0];

        if (avcodec_open2(outCodecCtx, outCodec, NULL) < 0)
        {
            printf("cannot open output codec");
            break;
        }

        av_dump_format(outFmtCtx, 0, outFileName, 1);

        if (avformat_write_header(outFmtCtx, NULL) < 0)
        {
            printf("cannot write header to output file");
            break;
        }
        while (av_read_frame(inFmtCtx, pkt) >= 0)
        {
            if (pkt->stream_index == inStreamIndex)
            {
                printf("frame_index:%d\n", frame_index);
                if (pkt->pts == AV_NOPTS_VALUE)
                {
                    // printf("frame_index:%d not found pts\n", frame_index);
                    AVRational time_base1 = inStream->time_base;
                    // int64_t calc_duration = (double)AV_TIME_BASE / av_q2d(inStream->r_frame_rate);
                    // pkt->pts = (double)(frame_index * calc_duration) / (double)(av_q2d(time_base1) * AV_TIME_BASE);
                    // pkt->dts = pkt->pts;
                    // pkt->duration = (double)calc_duration / (double)(av_q2d(time_base1) * AV_TIME_BASE);
                    pkt->duration = 1 / (av_q2d(time_base1) * av_q2d(inStream->r_frame_rate));
                    pkt->pts = frame_index * pkt->duration;
                    pkt->dts = pkt->pts;
                    frame_index++;
                }

                pkt->pts = av_rescale_q_rnd(pkt->pts, inStream->time_base, outStream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                pkt->dts = av_rescale_q_rnd(pkt->dts, inStream->time_base, outStream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                pkt->duration = av_rescale_q(pkt->duration, inStream->time_base, outStream->time_base);
                if (av_interleaved_write_frame(outFmtCtx, pkt) < 0)
                {
                    printf("error muxing packet");
                    break;
                }
                av_packet_unref(pkt);
            }
        }
        av_write_trailer(outFmtCtx);

    } while (0);
}
