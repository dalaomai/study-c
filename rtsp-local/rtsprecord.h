#ifndef RTSPRECORD_H
#define RTSPRECORD_H

#include <queue>
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <unistd.h>

extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
#include "libavutil/audio_fifo.h"
}

typedef std::queue<AVPacket *> PacketQueue;
class StreamCodecContext
{
public:
    StreamCodecContext();
    ~StreamCodecContext();
    AVStream *in_stream, *out_stream;

    AVDictionary *enc_options = NULL;

    AVCodecContext *dec_ctx;
    AVCodecContext *enc_ctx;

    AVPacket *out_pkt;

    AVFrame *dec_frame;
    AVFrame *enc_frame;

    int init_decodec(AVFormatContext *in_fmt_ctx, AVStream *in_stream);
    int init_encodec(AVFormatContext *out_fmt_ctx, AVStream *out_stream);

protected:
    AVPixelFormat video_pix_format = AV_PIX_FMT_YUV420P;
    AVCodecID videoEncoderID = AV_CODEC_ID_H264;
    AVCodecID audioEncoderID = AV_CODEC_ID_AAC;
    bool allow_copy, is_copy = false;
};

class VideoStreamCodecContext : public StreamCodecContext
{
public:
    struct SwsContext *img_sws_ctx = NULL;

    VideoStreamCodecContext(bool allow_copy = true);
    ~VideoStreamCodecContext();
    int init_decodec(AVFormatContext *in_fmt_ctx, AVStream *in_stream);
    int init_encodec(AVFormatContext *out_fmt_ctx, AVStream *out_stream);
    int decode_pkt(AVPacket *pkt, PacketQueue *pkt_queue);

private:
    int key_pkt_pts = -1;
    int last_enc_frame_pts = -1;
    int input_pkt_duration = -1;
};
class AudioStreamCodecContext : public StreamCodecContext
{
public:
    struct SwrContext *swr_ctx;
    AVAudioFifo *smaple_fifo;

    AudioStreamCodecContext();
    ~AudioStreamCodecContext();

    int init_decodec(AVFormatContext *in_fmt_ctx, AVStream *in_stream);
    int init_encodec(AVFormatContext *out_fmt_ctx, AVStream *out_stream);
    int decode_pkt(AVPacket *pkt, PacketQueue *pkt_queue);

private:
    int int_swr();
};

class RTSPRecord
{
public:
    RTSPRecord(const char *inFileName, const char *outFileName);
    ~RTSPRecord();
    int start();

private:
    const char *inFileName = "";
    const char *outFileName = "/root/study_c/.temp/record.mp4";
    const int MAX_PKT_QUEUE = 100;
    AVFormatContext *inFmtCtx, *outFmtCtx = NULL;

    AVDictionary *open_options = NULL;

    AVStream *inVStream, *outVStream = NULL;
    AVStream *inAStream, *outAStream = NULL;
    int inVStreamIndex, inAStreamIndex = -1;
    int outVStreamIndex, outAStreamIndex = -1;

    PacketQueue *v_input_pkt_queue, *a_input_pkt_queue;
    PacketQueue *video_pkt_queue, *audio_pkt_queue;
    std::condition_variable *write_pkt_cond, *v_pkt_process_cond, *a_pkt_process_cond;

    VideoStreamCodecContext *video_stream_codec_context;
    AudioStreamCodecContext *audio_stream_codec_context;

    int openInput();
    int openOutput();
    void read_pkt_process();
    static void read_pkt_process_(void *param);
    void video_pkt_process();
    static void video_pkt_process_(void *param);
    void audio_pkt_process();
    static void audio_pkt_process_(void *param);
    int write_to_file();
    void write_file_process();
    static void write_file_process_(void *param);

    float output_video_ts = 0;
    int key_v_pts = -1, key_a_pts = -1;

    bool stop = false;
    bool allow_copy = true;
};
#endif