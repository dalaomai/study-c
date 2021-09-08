extern "C"
{
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/common.h"
}

av_always_inline char *av_err2str_inline(int errnum)
{
    static char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}

int main()
{
    AVFormatContext *inputfmtctx = avformat_alloc_context();
    AVFormatContext *outputFmtCtx = NULL;
    int streamIndex = 0;
    AVStream *inputStream, *outputStream = NULL;
    AVCodec *inputCodec, *outputCodec = NULL;
    AVCodecContext *inputCodecCtx, *outputCodecCtx = NULL;
    const char *inputFilename = "/root/study_c/.temp/sample-15s.mp3";
    const char *ouputFilename = "/root/study_c/.temp/acc.mp4";
    AVPacket *inputPacket = av_packet_alloc();
    AVPacket *outputPacket = av_packet_alloc();
    AVFrame *inputFrame = av_frame_alloc();
    AVFrame *outputFrame = av_frame_alloc();
    SwrContext *resampleContext = NULL;
    AVAudioFifo *fifo = NULL;

    avformat_open_input(&inputfmtctx, inputFilename, NULL, NULL);
    avformat_find_stream_info(inputfmtctx, NULL);

    av_dump_format(inputfmtctx, 0, inputFilename, 0);

    inputStream = inputfmtctx->streams[streamIndex];
    inputCodec = avcodec_find_decoder(inputStream->codecpar->codec_id);
    inputCodecCtx = avcodec_alloc_context3(inputCodec);
    avcodec_parameters_to_context(inputCodecCtx, inputStream->codecpar);
    // inputStream->time_base.den = inputCodecCtx->sample_rate;
    inputCodecCtx->pkt_timebase.den = inputCodecCtx->sample_rate;
    inputCodecCtx->pkt_timebase.num = 1;
    avcodec_open2(inputCodecCtx, inputCodec, NULL);

    avformat_alloc_output_context2(&outputFmtCtx, NULL, NULL, ouputFilename);
    outputCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    outputCodecCtx = avcodec_alloc_context3(outputCodec);
    outputCodecCtx->channels = inputCodecCtx->channels;
    outputCodecCtx->channel_layout = av_get_default_channel_layout(outputCodecCtx->channels);
    outputCodecCtx->sample_rate = inputCodecCtx->sample_rate;
    outputCodecCtx->sample_fmt = outputCodec->sample_fmts[0];
    // outputCodecCtx->bit_rate = 320000;

    if (outputFmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        outputCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    avcodec_open2(outputCodecCtx, outputCodec, NULL);

    outputStream = avformat_new_stream(outputFmtCtx, outputCodec);
    avcodec_parameters_from_context(outputStream->codecpar, outputCodecCtx);
    // outputStream->time_base.den = outputCodecCtx->sample_rate;
    // outputStream->time_base.num = 1;

    av_dump_format(outputFmtCtx, 0, ouputFilename, 1);

    resampleContext = swr_alloc_set_opts(
        NULL,
        outputCodecCtx->channel_layout, outputCodecCtx->sample_fmt, outputCodecCtx->sample_rate,
        inputCodecCtx->channel_layout, inputCodecCtx->sample_fmt, inputCodecCtx->sample_rate,
        0, NULL);
    swr_init(resampleContext);

    fifo = av_audio_fifo_alloc(outputCodecCtx->sample_fmt, outputCodecCtx->channels, 1);
    uint8_t **channle_smaple_buffer = (uint8_t **)calloc(outputCodecCtx->channels, sizeof(uint8_t *));
    av_samples_alloc(channle_smaple_buffer, NULL, outputCodecCtx->channels, outputCodecCtx->frame_size, outputCodecCtx->sample_fmt, 0);

    inputFrame->channel_layout = inputCodecCtx->channel_layout;
    inputFrame->sample_rate = inputCodecCtx->sample_rate;
    inputFrame->format = inputCodecCtx->sample_fmt;

    outputFrame->channel_layout = outputCodecCtx->channel_layout;
    outputFrame->sample_rate = outputCodecCtx->sample_rate;
    outputFrame->format = outputCodecCtx->sample_fmt;
    outputFrame->nb_samples = outputCodecCtx->frame_size;
    // av_frame_get_buffer(outputFrame, 0);

    avio_open(&(outputFmtCtx->pb), ouputFilename, AVIO_FLAG_READ_WRITE);
    avformat_write_header(outputFmtCtx, NULL);
    int pts = 0;
    int ret = -1;
    int tempRet = -1;
    int pktCount = 0;
    int outputPktCount = 0;
    int inFCount = 0;
    int outFCount = 0;
    while (1)
    {
        ret = av_read_frame(inputfmtctx, inputPacket);
        if (ret < 0)
        {
            fprintf(stderr, "av_read_frame:%s\n", av_err2str_inline(ret));
            if (ret = AVERROR_EOF)
            {
                inputPacket = NULL;
            }
            else
            {
                return -1;
            }
        }
        pktCount += 1;
        if (inputPacket != NULL)
        {
            av_packet_rescale_ts(inputPacket, inputStream->time_base, inputCodecCtx->pkt_timebase);
        }
        tempRet = avcodec_send_packet(inputCodecCtx, inputPacket);
        if (tempRet < 0)
        {
            fprintf(stderr, "avcodec_send_packet:%s\n", av_err2str_inline(tempRet));
        }

        while (1)
        {
            ret = avcodec_receive_frame(inputCodecCtx, inputFrame);
            if (ret < 0 && ret != -11)
            {
                fprintf(stderr, "%d:avcodec_receive_frame:%s\n", pts, av_err2str_inline(ret));
            }
            if (ret >= 0)
            {
                inFCount += 1;
                tempRet = swr_convert(resampleContext, channle_smaple_buffer, outputCodecCtx->frame_size, (const uint8_t **)inputFrame->data, inputCodecCtx->frame_size);
                if (tempRet < 0)
                {
                    fprintf(stderr, "swr_convert:%s\n", av_err2str_inline(tempRet));
                }
                tempRet = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + outputCodecCtx->frame_size);
                if (tempRet < 0)
                {
                    fprintf(stderr, "av_audio_fifo_realloc:%s\n", av_err2str_inline(tempRet));
                }
                tempRet = av_audio_fifo_write(fifo, (void **)channle_smaple_buffer, outputCodecCtx->frame_size);
                if (tempRet < 0)
                {
                    fprintf(stderr, "av_audio_fifo_write:%s\n", av_err2str_inline(tempRet));
                }
            }
            av_frame_unref(inputFrame);

            while (1)
            {
                const int frame_size = FFMIN(av_audio_fifo_size(fifo),
                                             outputCodecCtx->frame_size);
                if (frame_size == 0)
                {
                    if (inputPacket == NULL && ret < 0)
                    {
                        outputFrame = NULL;
                    }
                    else
                    {
                        break;
                    }
                }
                if (outputFrame != NULL)
                {
                    outputFrame->nb_samples = frame_size;
                    av_frame_get_buffer(outputFrame, 0);
                    if (av_audio_fifo_read(fifo, (void **)outputFrame->data, frame_size) < 0)
                    {
                        break;
                    }
                    outFCount += 1;
                    outputFrame->pts = pts;
                    pts += outputFrame->nb_samples;
                }

                tempRet = avcodec_send_frame(outputCodecCtx, outputFrame);
                if (tempRet < 0)
                {
                    fprintf(stderr, "avcodec_send_frame:%s\n", av_err2str_inline(tempRet));
                }

                while (1)
                {
                    tempRet = avcodec_receive_packet(outputCodecCtx, outputPacket);
                    if (tempRet < 0)
                    {
                        if (tempRet != -11)
                        {
                            fprintf(stderr, "avcodec_receive_packet:%s\n", av_err2str_inline(tempRet));
                        }
                        break;
                    }
                    av_interleaved_write_frame(outputFmtCtx, outputPacket);
                    av_packet_unref(outputPacket);
                    outputPktCount += 1;
                }

                if (outputFrame == NULL)
                {
                    break;
                }
            }

            if (ret < 0)
            {
                break;
            }
        }
        if (inputPacket == NULL)
        {
            break;
        }
        av_packet_unref(inputPacket);
    }
    printf("%d %d %d %d %d", pts, pktCount, outputPktCount, inFCount, outFCount);
    av_write_trailer(outputFmtCtx);

    return 1;
}