#ifndef RTSPRECORD_H
#define RTSPRECORD_H

#include <queue>

extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
#include "libavutil/audio_fifo.h"
}

class RTSPRecord
{
public:
    RTSPRecord(const char *inFileName, const char *outFileName);
    ~RTSPRecord();
    int start();

private:
    const char *inFileName = "rtsp://admin:maizhiling456@192.168.1.5:554/stream2";
    // const char *inFileName = "/root/study_c/.temp/sample_960x540.mkv";
    const char *outFileName = "/root/study_c/.temp/record.mp4";
    AVFormatContext *inFmtCtx, *outFmtCtx = NULL;

    AVStream *inVStream, *outVStream = NULL;
    AVStream *inAStream, *outAStream = NULL;
    int inVStreamIndex, inAStreamIndex = -1;
    int outVStreamIndex, outAStreamIndex = -1;

    AVCodec *vDecoder, *vEncoder = NULL;
    AVCodec *aDecoder, *aEncoder = NULL;
    AVCodecContext *vEncoderCtx, *vDecoderCtx = NULL;
    AVCodecContext *aEncoderCtx, *aDecoderCtx = NULL;

    AVPacket *inPkt, *vOutPkt, *tempPkt = NULL;
    // AVPacket tempPkt;
    AVPacket *aOutPkt = NULL;

    AVFrame *videoInFrame, *videoMiddleFrame, *audioInFrame, *audioMiddleFrame = NULL;

    struct SwsContext *imgSwsCtx = NULL;
    struct SwrContext *audioSwrCtx = NULL;

    int vH, vW = 0;

    AVAudioFifo *audioFifo = NULL;
    std::queue<AVPacket *> videoPktStack, audioPktStack;

    AVPixelFormat videoPixFormat = AV_PIX_FMT_YUV420P;
    AVCodecID videoEncoderID = AV_CODEC_ID_H264;
    AVCodecID audioEncoderID = AV_CODEC_ID_AAC;

    int openInput();
    int openOutput();
    int prepare_video_decoder();
    int prepare_video_encoder();
    int prepare_audio_decoder();
    int prepare_audio_resampler();
    int prepare_audio_encoder();
    int readMiddleFrame();
    int write_to_file();

    int nextPts = -1;
};
#endif