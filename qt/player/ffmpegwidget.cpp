#include "ffmpegwidget.h"
#include "ui_ffmpegwidget.h"

FFmpegVideo::FFmpegVideo()
{
    fmtCtx = avformat_alloc_context();
    pkt = av_packet_alloc();
    yuvFrame = av_frame_alloc();
    rgbFrame = av_frame_alloc();
}

FFmpegVideo::~FFmpegVideo()
{
    if (!pkt)
        av_packet_free(&pkt);
    if (!yuvFrame)
        av_frame_free(&yuvFrame);
    if (!rgbFrame)
        av_frame_free(&rgbFrame);
    if (!videoCodecCtx)
        avcodec_free_context(&videoCodecCtx);
    if (!videoCodecCtx)
        avcodec_close(videoCodecCtx);
    if (!fmtCtx)
        avformat_close_input(&fmtCtx);
}

void FFmpegVideo::setUrl(QString url)
{
    _url = url;
}

int FFmpegVideo::open_input_file()
{
    if (_url.isEmpty())
    {
        return -1;
    }

    if (
        avformat_open_input(
            &fmtCtx, _url.toLocal8Bit().data(),
            NULL, NULL) < 0)
    {
        printf("open file falue");
        return -1;
    }

    if (avformat_find_stream_info(fmtCtx, NULL) < 0)
    {
        printf("not find stream");
        return -1;
    }

    int streamCount = fmtCtx->nb_streams;
    for (int i = 0; i < streamCount; i++)
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
        return -1;
    }

    AVCodecParameters *videoCodecPara = fmtCtx->streams[videoStreamIndex]->codecpar;
    if (!(videoCodec = avcodec_find_decoder(videoCodecPara->codec_id)))
    {
        printf("not find valid decode codec");
        return -1;
    }

    if (!(videoCodecCtx = avcodec_alloc_context3(videoCodec)))
    {
        printf("not alloc decode context");
        return -1;
    }

    if (avcodec_parameters_to_context(videoCodecCtx, videoCodecPara) < 0)
    {
        printf("cannot initialize codec para");
        return -1;
    }

    if (avcodec_open2(videoCodecCtx, videoCodec, NULL) < 0)
    {
        printf("cannot open codec");
        return -1;
    }

    videoW = videoCodecCtx->width;
    videoH = videoCodecCtx->height;
    img_ctx = sws_getContext(
        videoW, videoH, videoCodecCtx->pix_fmt,
        videoW, videoH, AV_PIX_FMT_RGB32, SWS_BICUBIC,
        NULL, NULL, NULL);

    numBytes = av_image_get_buffer_size(
        AV_PIX_FMT_RGB32, videoW, videoH, 1);
    out_buffer = (unsigned char *)av_malloc(numBytes * sizeof(unsigned char));

    int fillResp = av_image_fill_arrays(
        rgbFrame->data, rgbFrame->linesize, out_buffer,
        AV_PIX_FMT_RGB32, videoW, videoH, 1);

    if (fillResp < 0)
    {
        qDebug() << "fill arrays failue";
        return 1;
    }

    return true;
}

void FFmpegVideo::run()
{
    if (!open_input_file())
    {
        qDebug() << "open file failue";
        return;
    }

    while (av_read_frame(fmtCtx, pkt) >= 0)
    {
        if (pkt->stream_index == videoStreamIndex)
        {
            if (avcodec_send_packet(videoCodecCtx, pkt) >= 0)
            {
                int ret;
                while (avcodec_receive_frame(videoCodecCtx, yuvFrame) >= 0)
                {
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    {
                        return;
                    }
                    else if (ret <= 0)
                    {
                        fprintf(stderr, "error decocding");
                        exit(1);
                    }

                    sws_scale(img_ctx, yuvFrame->data, yuvFrame->linesize, 0, videoH, rgbFrame->data, rgbFrame->linesize);

                    QImage img(out_buffer, videoW, videoH, QImage::Format_RGB32);
                    emit sendQImage(img);
                    QThread::msleep(30);
                }
            }
        }
        av_packet_unref(pkt);
    }
    qDebug() << "play done";
}

FFmpegWidget::FFmpegWidget(QWidget *parent) : QWidget(parent),
                                              ui(new Ui::FFmpegWidget)
{
    ui->setupUi(this);

    ffmpeg = new FFmpegVideo;
    connect(ffmpeg, SIGNAL(sendQImage(QImage)), this, SLOT(receiveQImage(QImage)));
    connect(ffmpeg, &FFmpegVideo::finished, ffmpeg, &FFmpegVideo::deleteLater);
}

FFmpegWidget::~FFmpegWidget()
{
    if (ffmpeg->isRunning())
    {
        stop();
    }
    delete ui;
}

void FFmpegWidget::setUrl(QString url)
{
    ffmpeg->setUrl(url);
}

void FFmpegWidget::play()
{
    stop();
    ffmpeg->start();
}

void FFmpegWidget::stop()
{
    if (ffmpeg->isRunning())
    {
        ffmpeg->requestInterruption();
        ffmpeg->quit();
        ffmpeg->wait(100);
    }
    img.fill(Qt::black);
}

void FFmpegWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, img);
}

void FFmpegWidget::receiveQImage(const QImage &rImg)
{
    img = rImg.scaled(this->size());
    update();
}