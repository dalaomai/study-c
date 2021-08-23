#ifndef FFMPEGWIDGET_H
#define FFMPEGWIDGET_H

#include <QWidget>
#include <QThread>
#include <QDebug>
#include <QPainter>
#include <QImage>
#include <QPaintEvent>

extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

namespace Ui
{
    class FFmpegWidget;
}

class FFmpegVideo : public QThread
{
    Q_OBJECT

public:
    explicit FFmpegVideo();
    ~FFmpegVideo();

    void setUrl(QString url);
    int open_input_file();

protected:
    void run();
signals:
    void sendQImage(QImage);

private:
    AVFormatContext *fmtCtx = NULL;
    AVCodec *videoCodec = NULL;
    AVCodecContext *videoCodecCtx = NULL;
    AVPacket *pkt = NULL;
    AVFrame *yuvFrame = NULL;
    AVFrame *rgbFrame = NULL;

    struct SwsContext *img_ctx = NULL;

    unsigned char *out_buffer = nullptr;
    int videoStreamIndex = -1;
    int numBytes = -1;
    int videoW = 0;
    int videoH = 0;

    QString _url;
};

class FFmpegWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FFmpegWidget(QWidget *parent = nullptr);
    ~FFmpegWidget();

    void setUrl(QString url);

    void play();
    void stop();

protected:
    void paintEvent(QPaintEvent *);

private slots:
    void receiveQImage(const QImage &rImg);

private:
    FFmpegVideo *ffmpeg;
    QImage img;

    Ui::FFmpegWidget *ui;
};

#endif // FFMPEGWIDGET_H
