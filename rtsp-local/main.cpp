#include "rtsprecord.h"
#include <ctime>
#include <string>
#include <boost/format.hpp>

int main()
{
    const char *in_file = "rtsp://admin:xxxx@192.168.1.5:554/stream1";

    RTSPRecord *recorder;
    boost::format out_file_fmt("/root/study_c/.temp/temp/%1%.mp4");
    std::time_t now;
    std::string out_file;
    char time_str[64];
    int ret;
    recorder = new RTSPRecord{in_file};
    ret = recorder->open_input();
    if (ret < 0)
    {
        fprintf(stderr, "open input err");
        return -1;
    }

    for (int i = 0; i < 100; i++)
    {
        now = time(0);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
        out_file = (out_file_fmt % time_str).str();

        ret = recorder->open_output(out_file.data());
        if (ret < 0)
        {
            fprintf(stderr, "open ouput err");
            return -1;
        }
        ret = recorder->start_record(5);
        if (ret < 0)
        {
            delete recorder;
            recorder = new RTSPRecord{in_file};
            ret = recorder->open_input();
            if (ret < 0)
            {
                fprintf(stderr, "open input err");
                return -1;
            }
        }
        // sleep(20);
        // recorder->stop_record();
    }
    delete recorder;
}