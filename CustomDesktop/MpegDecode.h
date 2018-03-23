#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __STDC_CONSTANT_MACROS
#define UINT64_C
#define _XOPEN_SOURCE 600 /* for usleep */
extern "C"
{
#include "libavutil/avutil.h"  
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h"  
#include "libavdevice/avdevice.h"  
#include "libswscale/swscale.h" 
//#include "unistd.h"

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/opt.h"
}
#pragma comment(lib,"avutil.lib")  
#pragma comment(lib,"avcodec.lib")  
#pragma comment(lib,"avformat.lib")  
#pragma comment(lib,"swscale.lib")  
#pragma comment(lib,"avfilter.lib")  
#pragma comment(lib,"avfilter.lib")  
#include <list>
class MpegDecode
{
public:
	MpegDecode();
	~MpegDecode();
	void Init();

	const char *filter_descr = "scale=640:480 ";
	/* other way:
	scale=78:24 [scl]; [scl] transpose=cclock // assumes "[in]" and "[out]" to be input output pads respectively
	*/
	int ret;
	AVPacket packet;
	AVFrame *frame ;
	AVFrame *filt_frame;
	AVFormatContext *fmt_ctx;
	AVCodecContext *dec_ctx;
	AVFilterContext *buffersink_ctx;
	AVFilterContext *buffersrc_ctx;
	AVFilterGraph *filter_graph;
	int video_stream_index = -1;
	int64_t last_pts = AV_NOPTS_VALUE;
	int init_filters(const char *filters_descr);
	int open_input_file(const char *filename);
	void display_frame(const AVFrame *frame, AVRational time_base);
	typedef struct _IMGFrame
	{
		int width=0, height=0,linesize=0;
		char *data=0;
	}IMGFrame;
	IMGFrame m_CurrentFrame;

	void Reset(char * file);
	bool NextFrame();
	void End();
	//void Decode();
};

