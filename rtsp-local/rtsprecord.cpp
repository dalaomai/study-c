#include "rtsprecord.h"

av_always_inline char *av_err2str_inline(int errnum)
{
    static char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}

int check(int ret)
{
    if (ret < 0 && ret != -11)
    {
        fprintf(stderr, "err:%s\n", av_err2str_inline(ret));
    }
    return ret;
}

StreamCodecContext::StreamCodecContext() {}

StreamCodecContext::~StreamCodecContext()
{
    if (dec_ctx != NULL)
    {
        avcodec_free_context(dec_ctx);
    }
    if (enc_ctx != NULL)
    {
        avcodec_free_context(enc_ctx);
    }
    if (dec_frame != NULL)
    {
        av_frame_free(dec_frame);
    }
    if (enc_frame != NULL)
    {
        av_frame_free(enc_frame);
    }
}

int StreamCodecContext::init_decodec(AVFormatContext *in_fmt_ctx, AVStream *in_stream)
{
    StreamCodecContext::in_stream = in_stream;
    AVCodec *decodec = avcodec_find_decoder(in_stream->codecpar->codec_id);
    dec_ctx = avcodec_alloc_context3(decodec);
    avcodec_parameters_to_context(dec_ctx, in_stream->codecpar);

    if (dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        dec_ctx->framerate = av_guess_frame_rate(in_fmt_ctx, in_stream, NULL);
    }

    avcodec_open2(dec_ctx, decodec, NULL);
    return 0;
}

int StreamCodecContext::init_encodec(AVFormatContext *out_fmt_ctx, AVStream *out_stream)
{
    StreamCodecContext::out_stream = out_stream;
}

VideoStreamCodecContext::VideoStreamCodecContext(bool allow_copy)
{
    VideoStreamCodecContext::allow_copy = allow_copy;
}

VideoStreamCodecContext::~VideoStreamCodecContext() {}

int VideoStreamCodecContext::init_decodec(AVFormatContext *in_fmt_ctx, AVStream *in_stream)
{
    StreamCodecContext::init_decodec(in_fmt_ctx, in_stream);

    input_pkt_duration = 1 / (av_q2d(in_stream->avg_frame_rate) * av_q2d(in_stream->time_base));

    if (allow_copy && dec_ctx->codec_id == videoEncoderID)
    {
        is_copy = true;
    }

    // outVStream->time_base = vDecoderCtx->time_base;
    return 1;
}

int VideoStreamCodecContext::init_encodec(AVFormatContext *out_fmt_ctx, AVStream *out_stream)
{
    StreamCodecContext::init_encodec(out_fmt_ctx, out_stream);
    if (is_copy)
    {
        avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
        out_stream->codecpar->codec_tag = MKTAG('a', 'v', 'c', '1');
        return 1;
    }
    AVCodec *encodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    enc_ctx = avcodec_alloc_context3(encodec);
    // avcodec_parameters_to_context(enc_ctx, in_stream->codecpar);

    // enc_ctx->color_range = dec_ctx->color_range;
    enc_ctx->height = dec_ctx->height;
    enc_ctx->width = dec_ctx->width;
    enc_ctx->sample_aspect_ratio = dec_ctx->sample_aspect_ratio;
    // enc_ctx->codec_tag = MKTAG('a', 'v', 'c', '1');
    if (encodec->pix_fmts)
    {
        enc_ctx->pix_fmt = encodec->pix_fmts[0];
    }
    else
    {
        enc_ctx->pix_fmt = dec_ctx->pix_fmt;
    }
    enc_ctx->time_base = av_inv_q(dec_ctx->framerate);

    enc_ctx->gop_size = 120; /* emit one intra frame every twelve frames at most */
    enc_ctx->max_b_frames = 16;
    enc_ctx->rc_buffer_size = 0;

    if (out_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    av_dict_set(&enc_options, "allow_skip_frames", "1", 0);
    check(avcodec_open2(enc_ctx, encodec, &enc_options));

    avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);

    img_sws_ctx = sws_getContext(
        dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
        enc_ctx->width, enc_ctx->height, enc_ctx->pix_fmt, SWS_BICUBIC,
        NULL, NULL, NULL);

    dec_frame = av_frame_alloc();
    enc_frame = av_frame_alloc();

    dec_frame->width = dec_ctx->width;
    dec_frame->height = dec_ctx->height;
    dec_frame->format = video_pix_format;
    av_image_alloc(enc_frame->data, enc_frame->linesize,
                   dec_ctx->width, dec_ctx->height, video_pix_format, 1);

    enc_frame->width = enc_ctx->width;
    enc_frame->height = enc_ctx->height;
    enc_frame->format = video_pix_format;
}

int VideoStreamCodecContext::decode_pkt(AVPacket *pkt, PacketQueue *pkt_queue)
{
    int ret = -1;
    if (pkt->duration == 0)
    {
        pkt->duration = input_pkt_duration;
    }
    if (is_copy)
    {
        pkt_queue->push(pkt);
        return 0;
    }

    av_packet_rescale_ts(pkt, in_stream->time_base, av_inv_q(dec_ctx->framerate));
    if (pkt->flags & AV_PKT_FLAG_KEY)
    {
        key_pkt_pts = pkt->pts;
    }
    else if (pkt->pts < key_pkt_pts)
    {
        av_packet_unref(pkt);
        return 0;
    }
    ret = check(avcodec_send_packet(dec_ctx, pkt));

    while (1)
    {
        ret = check(avcodec_receive_frame(dec_ctx, dec_frame));
        if (ret < 0)
        {
            if (pkt == NULL)
            {
                enc_frame = NULL;
            }
            else
            {
                break;
            }
        }
        else
        {
            check(sws_scale(img_sws_ctx, dec_frame->data, dec_frame->linesize, 0, dec_ctx->height, enc_frame->data, enc_frame->linesize));
            enc_frame->pkt_duration = dec_frame->pkt_duration;
            enc_frame->pts = dec_frame->pts;
            // enc_frame->pkt_dts = dec_frame->pkt_dts;
            // enc_frame->pkt_pts = dec_frame->pkt_pts;
            // printf("pts:%d dts:%d %d\n", enc_frame->pts, enc_frame->pkt_dts, enc_frame->pkt_duration);
            // enc_frame->pkt_pos = inPkt->pos;
        }

        int enc_frame_pts = 0;
        if (enc_frame != NULL)
        {
            enc_frame_pts = enc_frame->pts;
        }
        if (last_enc_frame_pts < 0)
        {
            last_enc_frame_pts = enc_frame->pts - enc_frame->pkt_duration;
        }

        do
        {
            if (enc_frame_pts == last_enc_frame_pts)
            {
                break;
            }
            if (enc_frame != NULL)
            {
                last_enc_frame_pts = last_enc_frame_pts + enc_frame->pkt_duration;
                enc_frame->pts = last_enc_frame_pts;
            }
            check(avcodec_send_frame(enc_ctx, enc_frame));
            // av_frame_unref(enc_frame);
            while (1)
            {
                out_pkt = av_packet_alloc();
                // av_new_packet(out_pkt,enc_ctx->height*enc_ctx->width);

                ret = check(avcodec_receive_packet(enc_ctx, out_pkt));
                if (ret >= 0)
                {
                    out_pkt->stream_index = out_stream->index;
                    av_packet_rescale_ts(out_pkt, enc_ctx->time_base, out_stream->time_base);
                    pkt_queue->push(out_pkt);
                }
                else
                {
                    av_packet_free(&out_pkt);
                    break;
                }
            }
        } while (enc_frame != NULL && enc_frame->pts < enc_frame_pts);
    }

    if (pkt != NULL)
    {
        av_packet_free(&pkt);
    }

    return 0;
}

AudioStreamCodecContext::AudioStreamCodecContext() {}

AudioStreamCodecContext::~AudioStreamCodecContext() {}

int AudioStreamCodecContext::init_decodec(AVFormatContext *in_fmt_ctx, AVStream *in_stream)
{
    int ret = StreamCodecContext::init_decodec(in_fmt_ctx, in_stream);

    // out_stream->time_base = dec_ctx->time_base;

    return ret;
}

int AudioStreamCodecContext::init_encodec(AVFormatContext *out_fmt_ctx, AVStream *out_stream)
{
    StreamCodecContext::init_encodec(out_fmt_ctx, out_stream);

    AVCodec *encoder = avcodec_find_encoder(audioEncoderID);

    enc_ctx = avcodec_alloc_context3(encoder);
    enc_ctx->channels = in_stream->codecpar->channels;
    enc_ctx->channel_layout = av_get_default_channel_layout(enc_ctx->channels);
    enc_ctx->sample_rate = dec_ctx->sample_rate;
    enc_ctx->sample_fmt = encoder->sample_fmts[0];
    // enc_ctx->bit_rate = dec_ctx->bit_rate;

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
    if (out_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    check(avcodec_open2(enc_ctx, encoder, NULL));

    avcodec_parameters_from_context(out_stream->codecpar, enc_ctx);

    int_swr();

    dec_frame = av_frame_alloc();
    enc_frame = av_frame_alloc();

    dec_frame->channel_layout = av_get_default_channel_layout(dec_ctx->channels);
    dec_frame->sample_rate = dec_ctx->sample_rate;
    dec_frame->format = dec_ctx->sample_fmt;

    enc_frame->channel_layout = enc_ctx->channel_layout;
    enc_frame->sample_rate = enc_ctx->sample_rate;
    enc_frame->format = enc_ctx->sample_fmt;
    // enc_frame->nb_samples = enc_ctx->frame_size;
    return 1;
}

int AudioStreamCodecContext::int_swr()
{
    swr_ctx = swr_alloc_set_opts(
        NULL,
        av_get_default_channel_layout(enc_ctx->channels),
        enc_ctx->sample_fmt,
        enc_ctx->sample_rate,
        av_get_default_channel_layout(dec_ctx->channels),
        dec_ctx->sample_fmt,
        dec_ctx->sample_rate,
        0, NULL);

    check(swr_init(swr_ctx));

    smaple_fifo = av_audio_fifo_alloc(enc_ctx->sample_fmt, enc_ctx->channels, 1);
}

int AudioStreamCodecContext::decode_pkt(AVPacket *pkt, PacketQueue *pkt_queue)
{
    int ret = -1;

    av_packet_rescale_ts(pkt, in_stream->time_base, dec_ctx->time_base);
    check(avcodec_send_packet(dec_ctx, pkt));

    while (check(avcodec_receive_frame(dec_ctx, dec_frame)) >= 0)
    {
        uint8_t **converted_input_samples = NULL;
        // converted_input_samples = (uint8_t **)calloc(enc_ctx->channels, sizeof(*converted_input_samples));

        // check(av_samples_alloc(
        //     converted_input_samples, NULL, enc_ctx->channels, enc_ctx->frame_size, enc_ctx->sample_fmt, 0));
        // swr_convert(swr_ctx, converted_input_samples, enc_frame->nb_samples, (const uint8_t **)dec_frame->data, dec_frame->nb_samples);

        // check(av_audio_fifo_realloc(smaple_fifo, av_audio_fifo_size(smaple_fifo) + enc_ctx->frame_size));
        // av_audio_fifo_write(smaple_fifo, (void **)converted_input_samples, enc_ctx->frame_size);

        dec_frame->channel_layout = av_get_default_channel_layout(dec_ctx->channels);
        check(swr_convert_frame(swr_ctx, enc_frame, dec_frame));
        enc_frame->pts = dec_frame->pts;
        enc_frame->pkt_dts = dec_frame->pkt_dts;

        check(avcodec_send_frame(enc_ctx, enc_frame));

        while (1)
        {
            out_pkt = av_packet_alloc();

            ret = check(avcodec_receive_packet(enc_ctx, out_pkt));

            if (ret < 0)
            {
                av_packet_free(&out_pkt);
                break;
            }
            else
            {
                av_packet_rescale_ts(out_pkt, enc_ctx->time_base, out_stream->time_base);
                out_pkt->stream_index = out_stream->index;
                pkt_queue->push(out_pkt);
            }
        }

        av_frame_unref(dec_frame);
    }
}

RTSPRecord::RTSPRecord(const char *inFileName, const char *outFileName)
{
    RTSPRecord::inFileName = inFileName;
    RTSPRecord::outFileName = outFileName;
    inFmtCtx = avformat_alloc_context();

    v_input_pkt_queue = new PacketQueue();
    a_input_pkt_queue = new PacketQueue();
    video_pkt_queue = new PacketQueue();
    audio_pkt_queue = new PacketQueue();

    write_pkt_cond = new std::condition_variable();
    v_pkt_process_cond = new std::condition_variable();
    a_pkt_process_cond = new std::condition_variable();
}

RTSPRecord::~RTSPRecord()
{
    delete v_input_pkt_queue;
    delete a_input_pkt_queue;
    delete video_pkt_queue;
    delete audio_pkt_queue;
    delete write_pkt_cond;
    delete v_pkt_process_cond;
    delete a_pkt_process_cond;
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

    video_stream_codec_context = new VideoStreamCodecContext(allow_copy);
    video_stream_codec_context->init_decodec(inFmtCtx, inVStream);
    video_stream_codec_context->init_encodec(outFmtCtx, outVStream);

    audio_stream_codec_context = new AudioStreamCodecContext();
    audio_stream_codec_context->init_decodec(inFmtCtx, inAStream);
    audio_stream_codec_context->init_encodec(outFmtCtx, outAStream);

    check(avformat_write_header(outFmtCtx, NULL));
    av_dump_format(outFmtCtx, 0, outFileName, 1);

    std::thread read_pkt_t(read_pkt_process_, (void *)this);
    std::thread process_video_pkt(video_pkt_process_, (void *)this);
    std::thread process_audio_pkt(audio_pkt_process_, (void *)this);
    std::thread process_wirte(write_file_process_, (void *)this);

    sleep(20);

    stop = true;

    read_pkt_t.join();
    v_pkt_process_cond->notify_all();
    a_pkt_process_cond->notify_all();
    process_video_pkt.join();
    process_audio_pkt.join();
    write_pkt_cond->notify_all();
    process_wirte.join();

    av_write_trailer(outFmtCtx);

    return 1;
}

int RTSPRecord::openInput()
{
    int ret = -1;
    av_dict_set(&open_options, "rtsp_transport", "tcp", 0);
    ret = check(avformat_open_input(&inFmtCtx, inFileName, NULL, &open_options));
    if (ret < 0)
    {
        fprintf(stderr, "cannot open input stream\n");
        return ret;
    }

    ret = check(avformat_find_stream_info(inFmtCtx, NULL));
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
    // outVStream->time_base = inVStream->time_base;
    outVStreamIndex = outVStream->index;

    // outAStream->time_base = inAStream->time_base;
    outAStreamIndex = outAStream->index;

    // avcodec_parameters_copy(outAStream->codecpar, inAStream->codecpar);

    return 1;
}

void RTSPRecord::read_pkt_process()
{
    while (!stop)
    {
        AVPacket *pkt = av_packet_alloc();
        int ret = check(av_read_frame(inFmtCtx, pkt));

        if (ret >= 0)
        {
            if (pkt->stream_index == inVStreamIndex)
            {
                v_input_pkt_queue->push(pkt);
                v_pkt_process_cond->notify_all();
            }
            else if (pkt->stream_index == inAStreamIndex)
            {
                a_input_pkt_queue->push(pkt);
                a_pkt_process_cond->notify_all();
            }
            // cond->notify_all();
        }
        else
        {
            av_packet_free(&pkt);
        }
    }
    v_input_pkt_queue->push(NULL);
    a_input_pkt_queue->push(NULL);
}

void RTSPRecord::video_pkt_process()
{
    AVPacket *pkt;
    while (!v_input_pkt_queue->empty())
    {
        pkt = v_input_pkt_queue->front();
        v_input_pkt_queue->pop();
        video_stream_codec_context->decode_pkt(pkt, video_pkt_queue);
    }
}

void RTSPRecord::audio_pkt_process()
{
    AVPacket *pkt;
    while (!a_input_pkt_queue->empty())
    {
        pkt = a_input_pkt_queue->front();
        a_input_pkt_queue->pop();
        audio_stream_codec_context->decode_pkt(pkt, audio_pkt_queue);
    }
}

void RTSPRecord::write_file_process()
{

    while (!this->video_pkt_queue->empty() || !this->audio_pkt_queue->empty())
    {
        write_to_file();
    }
}

int RTSPRecord::write_to_file()
{
    AVPacket *tempPkt;
    float ts = 0;

    int last_scrap_v_pts = -1, last_scrap_a_pts = -1;

    // 进行对齐

    while (key_v_pts < 0 && !video_pkt_queue->empty())
    {
        tempPkt = video_pkt_queue->front();
        // ts = tempPkt->pts * av_q2d(outVStream->time_base);

        if (tempPkt->flags & AV_PKT_FLAG_KEY)
        {
            key_v_pts = tempPkt->pts;
            key_a_pts = av_rescale_q(key_v_pts, outVStream->time_base, outAStream->time_base);
        }
        else
        {
            last_scrap_v_pts = tempPkt->pts;
            last_scrap_a_pts = av_rescale_q(last_scrap_v_pts, outVStream->time_base, outAStream->time_base);
            av_packet_free(&tempPkt);
            video_pkt_queue->pop();
        }
    }

    while (key_v_pts < 0 && !audio_pkt_queue->empty())
    {
        tempPkt = audio_pkt_queue->front();
        if (tempPkt->pts < last_scrap_a_pts)
        {
            av_packet_free(&tempPkt);
            audio_pkt_queue->pop();
        }
        else
        {
            break;
        }
    }

    while (key_v_pts >= 0 && !video_pkt_queue->empty())
    {
        tempPkt = video_pkt_queue->front();
        tempPkt->pts -= key_v_pts;
        tempPkt->dts -= key_v_pts;

        ts = tempPkt->pts * av_q2d(outVStream->time_base);
        printf("video stream %d pts %d time %.2f flags %d\n", tempPkt->stream_index, tempPkt->pts, ts, tempPkt->flags);

        check(av_interleaved_write_frame(outFmtCtx, tempPkt));
        av_packet_free(&tempPkt);
        video_pkt_queue->pop();
    }

    while (key_v_pts >= 0 && !audio_pkt_queue->empty())
    {
        tempPkt = audio_pkt_queue->front();
        if (tempPkt->pts >= key_a_pts)
        {
            tempPkt->pts -= key_a_pts;
            tempPkt->dts -= key_a_pts;

            ts = tempPkt->pts * av_q2d(outAStream->time_base);
            printf("audio write pts %d time %.2f stream %d \n", tempPkt->pts, ts, tempPkt->stream_index);
            av_interleaved_write_frame(outFmtCtx, tempPkt);
        }
        av_packet_free(&tempPkt);

        audio_pkt_queue->pop();
    }
    return 0;
}

void RTSPRecord::read_pkt_process_(void *param)
{
    RTSPRecord *p = (RTSPRecord *)param;
    p->read_pkt_process();
}

void RTSPRecord::video_pkt_process_(void *param)
{
    RTSPRecord *p = (RTSPRecord *)param;
    while (!p->stop)
    {
        std::mutex temp_mutex;
        std::unique_lock<std::mutex> temp_lock(temp_mutex);
        p->v_pkt_process_cond->wait(temp_lock, [p]
                                    { return !p->v_input_pkt_queue->empty() || p->stop; });
        temp_lock.unlock();
        p->video_pkt_process();
        p->write_pkt_cond->notify_all();
    }
}

void RTSPRecord::audio_pkt_process_(void *param)
{
    RTSPRecord *p = (RTSPRecord *)param;

    while (!p->stop)
    {
        std::mutex temp_mutex;
        std::unique_lock<std::mutex> temp_lock(temp_mutex);
        p->a_pkt_process_cond->wait(temp_lock, [p]
                                    { return !p->a_input_pkt_queue->empty() || p->stop; });
        temp_lock.unlock();
        p->audio_pkt_process();
        p->write_pkt_cond->notify_all();
    }
}

void RTSPRecord::write_file_process_(void *param)
{
    RTSPRecord *p = (RTSPRecord *)param;

    while (!p->stop)
    {
        std::mutex temp_mutex;
        std::unique_lock<std::mutex> temp_lock(temp_mutex);
        p->write_pkt_cond->wait(temp_lock, [p]
                                { return !p->video_pkt_queue->empty() || !p->audio_pkt_queue->empty() || p->stop; });
        temp_lock.unlock();
        p->write_file_process();
    }
}