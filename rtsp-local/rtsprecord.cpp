#include "rtsprecord.h"

RTSPRecord::RTSPRecord(const char *inFileName, const char *outFileName)
{
    RTSPRecord::inFileName = inFileName;
    RTSPRecord::outFileName = outFileName;

    inFmtCtx = avformat_alloc_context();
    inPkt = av_packet_alloc();
    // vOutPkt = av_packet_alloc();
    videoInFrame = av_frame_alloc();
    audioInFrame = av_frame_alloc();
    videoMiddleFrame = av_frame_alloc();
    audioMiddleFrame = av_frame_alloc();
    // aOutPkt = av_packet_alloc();
}

RTSPRecord::~RTSPRecord()
{
    return;
}

int RTSPRecord::start()
{
    int ret = -1;
    ret = openInput();
    if (ret < 0)
    {
        return ret;
    }
    ret = openOutput();
    if (ret < 0)
    {
        return ret;
    }
    ret = prepare_video_decoder();
    if (ret < 0)
    {
        return ret;
    }
    ret = prepare_video_encoder();
    if (ret < 0)
    {
        return ret;
    }
    ret = prepare_audio_decoder();
    if (ret < 0)
    {
        return ret;
    }
    ret = prepare_audio_encoder();
    if (ret < 0)
    {
        return ret;
    }
    ret = prepare_audio_resampler();
    if (ret < 0)
    {
        return ret;
    }
    ret = readMiddleFrame();
    return 1;
}

int RTSPRecord::openInput()
{
    int ret = -1;
    ret = avformat_open_input(&inFmtCtx, inFileName, NULL, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "cannot open input stream\n");
        return ret;
    }

    ret = avformat_find_stream_info(inFmtCtx, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "cannot find streams\n");
        return ret;
    }

    av_dump_format(inFmtCtx, 0, inFileName, 0);

    for (size_t i = 0; i < inFmtCtx->nb_streams; i++)
    {
        if (inFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            inAStreamIndex = i;
        }
        else if (inFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            inVStreamIndex = i;
        }
    }
    if (inVStreamIndex * inAStreamIndex < 0)
    {
        fprintf(stderr, "not found A/V stream");
        return -1;
    }

    inVStream = inFmtCtx->streams[inVStreamIndex];
    inAStream = inFmtCtx->streams[inAStreamIndex];

    vH = inVStream->codecpar->height;
    vW = inVStream->codecpar->width;

    return 1;
}

int RTSPRecord::openOutput()
{
    int ret = -1;
    ret = avformat_alloc_output_context2(&outFmtCtx, NULL, NULL, outFileName);
    if (ret < 0)
    {
        fprintf(stderr, "alloc output err\n");
        return ret;
    }

    ret = avio_open(&outFmtCtx->pb, outFileName, AVIO_FLAG_READ_WRITE);
    if (ret < 0)
    {
        fprintf(stderr, "open output err\n");
        return ret;
    }

    outVStream = avformat_new_stream(outFmtCtx, NULL);
    outAStream = avformat_new_stream(outFmtCtx, NULL);
    if (!outVStream || !outAStream)
    {
        fprintf(stderr, "new stream err\n");
        return -1;
    }
    outVStream->time_base = inVStream->time_base;
    outVStreamIndex = outVStream->index;

    outAStream->time_base = inAStream->time_base;
    outAStreamIndex = outAStream->index;

    avcodec_parameters_copy(outAStream->codecpar, inAStream->codecpar);

    return 1;
}

int RTSPRecord::prepare_video_decoder()
{
    int ret = -1;
    vDecoder = avcodec_find_decoder(inVStream->codecpar->codec_id);
    if (vDecoder == NULL)
    {
        fprintf(stderr, "not found decoder\n");
        return -1;
    }
    vDecoderCtx = avcodec_alloc_context3(vDecoder);
    if (vDecoderCtx == NULL)
    {
        fprintf(stderr, "failue to alloc decoder context\n");
        return -1;
    }

    ret = avcodec_parameters_to_context(vDecoderCtx, inVStream->codecpar);
    if (ret < 0)
    {
        fprintf(stderr, "failue to avcodec_parameters_to_context context\n");
        return ret;
    }

    ret = avcodec_open2(vDecoderCtx, vDecoder, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "failue to open vDecoder\n");
        return ret;
    }

    // av_new_packet(inPkt, vW * vH);

    imgSwsCtx = sws_getContext(
        vW, vH, vDecoderCtx->pix_fmt,
        vW, vH, videoPixFormat, SWS_BICUBIC,
        NULL, NULL, NULL);
    videoMiddleFrame->width = vW;
    videoMiddleFrame->height = vH;
    videoMiddleFrame->format = videoPixFormat;
    av_image_alloc(videoMiddleFrame->data, videoMiddleFrame->linesize,
                   vW, vH, videoPixFormat, 1);

    outVStream->time_base = vDecoderCtx->time_base;
    return 1;
}

int RTSPRecord::prepare_video_encoder()
{
    int ret = -1;

    vEncoder = avcodec_find_encoder(videoEncoderID);
    AVCodecParameters *outCodecPara = outVStream->codecpar;

    avcodec_parameters_copy(outCodecPara, inVStream->codecpar);
    outCodecPara->codec_id = videoEncoderID;
    outCodecPara->format = videoPixFormat;
    outCodecPara->height = vH;
    outCodecPara->width = vW;
    outCodecPara->codec_tag = MKTAG('a', 'v', 'c', '1');

    vEncoderCtx = avcodec_alloc_context3(vEncoder);
    ret = avcodec_parameters_to_context(vEncoderCtx, outCodecPara);
    if (ret < 0)
    {
        fprintf(stderr, "avcodec_parameters_to_context\n");
        return ret;
    }
    vEncoderCtx->time_base = inVStream->time_base;
    vEncoderCtx->time_base.num = 1;
    vEncoderCtx->time_base.den = 25;
    vEncoderCtx->bit_rate = 400000;
    vEncoderCtx->gop_size = 12;
    vEncoderCtx->qmin = 10;
    vEncoderCtx->qmax = 51;
    vEncoderCtx->qcompress = (float)0.6;

    ret = avcodec_open2(vEncoderCtx, vEncoder, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "opn vencodr err\n");
        return ret;
    }

    avcodec_parameters_from_context(outVStream->codecpar, vEncoderCtx);
    // av_new_packet(vOutPkt, vH * vW);

    return 1;
}

int RTSPRecord::prepare_audio_decoder()
{
    int ret = -1;
    aDecoder = avcodec_find_decoder(inAStream->codecpar->codec_id);
    if (!aDecoder)
    {
        fprintf(stderr, "not found audio decoder/n");
        return -1;
    }

    aDecoderCtx = avcodec_alloc_context3(aDecoder);
    if (!aDecoderCtx)
    {
        fprintf(stderr, "alloc audio decoder context err/n");
        return -1;
    }

    ret = avcodec_parameters_to_context(aDecoderCtx, inAStream->codecpar);
    if (ret < 0)
    {
        fprintf(stderr, "err avcodec_parameters_to_context audio decoder\n");
        return ret;
    }

    ret = avcodec_open2(aDecoderCtx, aDecoder, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "open audio decoder err\n");
        return ret;
    }

    outAStream->time_base = aDecoderCtx->time_base;
    return 1;
}

int RTSPRecord::prepare_audio_resampler()
{
    int ret = -1;
    audioSwrCtx = swr_alloc_set_opts(
        NULL,
        av_get_default_channel_layout(aEncoderCtx->channels),
        aEncoderCtx->sample_fmt,
        aEncoderCtx->sample_rate,
        av_get_default_channel_layout(aDecoderCtx->channels),
        aDecoderCtx->sample_fmt,
        aDecoderCtx->sample_rate,
        0, NULL);
    if (!audioSwrCtx)
    {
        fprintf(stderr, "allocat audion swr context err\n");
        return AVERROR(ENOMEM);
    }

    ret = swr_init(audioSwrCtx);
    if (ret < 0)
    {
        fprintf(stderr, "init audio swr err\n");
        return ret;
    }
    audioMiddleFrame->channel_layout = av_get_default_channel_layout(aEncoderCtx->channels);
    audioMiddleFrame->channels = aEncoderCtx->channels;
    audioMiddleFrame->sample_rate = aEncoderCtx->sample_rate;
    audioMiddleFrame->format = aEncoderCtx->sample_fmt;
    audioMiddleFrame->nb_samples = aEncoderCtx->frame_size;
    av_frame_get_buffer(audioMiddleFrame, 0);
    audioMiddleFrame->linesize[1] = audioMiddleFrame->linesize[1];

    audioInFrame->channel_layout = av_get_default_channel_layout(aEncoderCtx->channels);
    audioInFrame->format = aEncoderCtx->sample_fmt;
    audioInFrame->sample_rate = aEncoderCtx->sample_rate;

    audioFifo = av_audio_fifo_alloc(aEncoderCtx->sample_fmt, aEncoderCtx->channels, 1);
    if (!audioFifo)
    {
        fprintf(stderr, "alloc audi fifo err\n");
        return -1;
    }
    return 0;
}

int RTSPRecord::prepare_audio_encoder()
{
    int ret = -1;

    aEncoder = avcodec_find_encoder(audioEncoderID);
    if (!aEncoder)
    {
        fprintf(stderr, "not find audio encoder\n");
        return -1;
    }
    aEncoderCtx = avcodec_alloc_context3(aEncoder);
    aEncoderCtx->channels = inAStream->codecpar->channels;
    aEncoderCtx->channel_layout = av_get_default_channel_layout(aEncoderCtx->channels);
    aEncoderCtx->sample_rate = aDecoderCtx->sample_rate;
    aEncoderCtx->sample_fmt = aEncoder->sample_fmts[0];
    aEncoderCtx->bit_rate = aDecoderCtx->bit_rate;

    // AVCodecParameters *outCodecPara = outAStream->codecpar;
    // avcodec_parameters_copy(outCodecPara, inAStream->codecpar);
    // outCodecPara->codec_id = audioEncoderID;
    // outCodecPara->channels = inAStream->codecpar->channels;
    // outCodecPara->channel_layout = av_get_default_channel_layout(outCodecPara->channels);
    // outCodecPara->format = AV_SAMPLE_FMT_FLTP;
    // ret = avcodec_parameters_to_context(aEncoderCtx, outCodecPara);
    // if (ret < 0)
    // {
    //     fprintf(stderr, "err audio encoder avcodec_parameters_to_context");
    //     return ret;
    // }
    if (outFmtCtx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        aEncoderCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    ret = avcodec_open2(aEncoderCtx, aEncoder, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "err to open audio encoder\n");
        return ret;
    }
    avcodec_parameters_from_context(outAStream->codecpar, aEncoderCtx);
    return 1;
}

int RTSPRecord::readMiddleFrame()
{
    av_dump_format(outFmtCtx, 0, outFileName, 1);

    int ret = -1;
    ret = avformat_write_header(outFmtCtx, NULL);
    if (ret < 0)
    {
        fprintf(stderr, "err write header\n");
        return ret;
    }

    int videoFrameIndex = 0;
    int audioFramIndex = 0;
    while (av_read_frame(inFmtCtx, inPkt) >= 0)
    {
        if (inPkt->stream_index == inVStreamIndex)
        {
            if (avcodec_send_packet(vDecoderCtx, inPkt) == 0)
            {
                while (avcodec_receive_frame(vDecoderCtx, videoInFrame) == 0)
                {
                    int ret = sws_scale(imgSwsCtx, videoInFrame->data, videoInFrame->linesize, 0, vH, videoMiddleFrame->data, videoMiddleFrame->linesize);

                    videoMiddleFrame->pkt_duration = videoInFrame->pkt_duration;

                    videoMiddleFrame->pts = videoInFrame->pts;
                    videoMiddleFrame->pkt_dts = videoInFrame->pkt_dts;
                    videoMiddleFrame->pkt_pts = videoInFrame->pkt_pts;
                    videoMiddleFrame->pkt_pos = inPkt->pos;
                    // printf("fram %d result %d duration %d pts %d dts %d\n", videoFrameIndex, ret, videoMiddleFrame->pkt_duration, videoMiddleFrame->pkt_dts, videoMiddleFrame->pkt_pts);

                    ret = avcodec_send_frame(vEncoderCtx, videoMiddleFrame);
                    if (ret < 0)
                    {
                        fprintf(stderr, "frame %d send err\n", videoFrameIndex);
                        continue;
                    }
                    vOutPkt = av_packet_alloc();
                    ret = avcodec_receive_packet(vEncoderCtx, vOutPkt);
                    if (ret >= 0)
                    {
                        // printf("video out packet %d duration %d pts %d dts %d pos %d\n", videoFrameIndex, vOutPkt->duration, vOutPkt->dts, vOutPkt->pts, vOutPkt->pos);
                        vOutPkt->stream_index = outVStreamIndex;
                        videoPktStack.push(vOutPkt);
                        // tempPkt = &videoPktStack.top();
                        write_to_file();

                        // if (videoFrameIndex > 50)
                        // {
                        //     av_write_trailer(outFmtCtx);
                        //     return 1;
                        // }
                    }
                    // av_packet_unref(vOutPkt);

                    videoFrameIndex++;
                }
            }
        }
        else if (inPkt->stream_index == inAStreamIndex)
        {
            if (avcodec_send_packet(aDecoderCtx, inPkt) == 0)
            {
                while (avcodec_receive_frame(aDecoderCtx, audioInFrame) >= 0)
                {
                    uint8_t **converted_input_samples = NULL;
                    converted_input_samples = (uint8_t **)calloc(aEncoderCtx->channels, sizeof(*converted_input_samples));

                    av_samples_alloc(converted_input_samples, NULL, aEncoderCtx->channels, aEncoderCtx->frame_size, aEncoderCtx->sample_fmt, 0);
                    swr_convert(audioSwrCtx, converted_input_samples, audioMiddleFrame->nb_samples, (const uint8_t **)audioInFrame->data, audioInFrame->nb_samples);

                    int length = aEncoderCtx->frame_size * av_get_bytes_per_sample(aEncoderCtx->sample_fmt);

                    // memcpy(audioInFrame->data[0], converted_input_samples[0], length);
                    audioMiddleFrame->data[0] = converted_input_samples[0];
                    audioMiddleFrame->data[1] = converted_input_samples[1];
                    // audioMiddleFrame->linesize[0] = audioMiddleFrame->nb_samples;
                    audioMiddleFrame->pts = audioInFrame->pts;
                    audioMiddleFrame->pkt_dts = audioInFrame->pkt_dts;
                    audioMiddleFrame->pkt_pts = audioInFrame->pkt_pts;
                    audioMiddleFrame->pkt_pos = audioInFrame->pkt_pos;
                    // (uint8_t *)audioMiddleFrame->data = converted_input_samples;

                    ret = avcodec_send_frame(aEncoderCtx, audioMiddleFrame);
                    if (ret < 0)
                    {
                        fprintf(stderr, "audio frame %d send err\n", audioFramIndex);
                        continue;
                    }
                    aOutPkt = av_packet_alloc();
                    ret = avcodec_receive_packet(aEncoderCtx, aOutPkt);

                    if (ret >= 0)
                    {
                        aOutPkt->stream_index = outAStreamIndex;

                        // printf("audio out packet %d duration %d pts %d dts %d pos %d\n", audioFramIndex, aOutPkt->duration, aOutPkt->dts, aOutPkt->pts, aOutPkt->pos);
                        audioPktStack.push(aOutPkt);
                        write_to_file();
                    }
                    if (audioFramIndex > 500)
                    {
                        av_write_trailer(outFmtCtx);
                        return 1;
                    }
                    audioFramIndex++;
                }
                // av_packet_unref(aOutPkt);
            }
            av_packet_unref(inPkt);
        }
    }
}

int RTSPRecord::write_to_file()
{
    AVPacket *tempPkt;
    // 初始化，先同步
    if (nextPts == -1)
    {
        if (videoPktStack.empty() || audioPktStack.empty())
        {
            return 1;
        }

        int videoPts = videoPktStack.front()->pts;
        int audioPts = audioPktStack.front()->pts;

        nextPts = (videoPts > audioPts) ? videoPts : audioPts;

        // 清空队列
        while (!videoPktStack.empty() && videoPktStack.front()->pts < nextPts)
        {
            tempPkt = videoPktStack.front();
            av_packet_free(&tempPkt);
            videoPktStack.pop();
        }
        while (!audioPktStack.empty() && audioPktStack.front()->pts < nextPts)
        {
            tempPkt = audioPktStack.front();
            av_packet_free(&tempPkt);
            audioPktStack.pop();
        }
    }

    if (!(videoPktStack.empty() || audioPktStack.empty()))
    {
        nextPts += AV_TIME_BASE;
    }

    while (!videoPktStack.empty() && videoPktStack.front()->pts <= nextPts)
    {
        tempPkt = videoPktStack.front();
        printf("video write pts %d\n", tempPkt->pts);
        // av_interleaved_write_frame(outFmtCtx, tempPkt);
        av_packet_free(&tempPkt);
        videoPktStack.pop();
    }
    while (!audioPktStack.empty() && audioPktStack.front()->pts <= nextPts)
    {
        tempPkt = audioPktStack.front();
        printf("audio write pts %d\n", tempPkt->pts);
        av_interleaved_write_frame(outFmtCtx, tempPkt);
        av_packet_free(&tempPkt);
        audioPktStack.pop();
    }
    return 0;
}