#include "MpegDecode.h"
#include <Windows.h>

#define INBUF_SIZE 4096

MpegDecode::MpegDecode()
{
}


MpegDecode::~MpegDecode()
{
}

void MpegDecode::Init()
{
	

	/* read all packets */
	



	


	
}

int MpegDecode::open_input_file(const char *filename)
{
	int ret;
	AVCodec *dec;

	if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
		return ret;
	}

	if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
		return ret;
	}

	
	/* select the video stream */
	ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
		return ret;
	}
	video_stream_index = ret;

	/* create decoding context */
	dec_ctx = avcodec_alloc_context3(dec);
	if (!dec_ctx)
		return AVERROR(ENOMEM);
	avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_stream_index]->codecpar);
	av_opt_set_int(dec_ctx, "refcounted_frames", 1, 0);

	/* init the video decoder */
	if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
		return ret;
	}
	
	
	
	return 0;
}

void MpegDecode::display_frame(const AVFrame * frame, AVRational time_base)
{
	int x, y;
	uint8_t *p0, *p;
	int64_t delay;
	char str[32];
	/* Trivial ASCII grayscale display. */
	p0 = frame->data[0];
	delete m_CurrentFrame.data;

	m_CurrentFrame.height = frame->height;
	m_CurrentFrame.linesize = frame->linesize[0];
	m_CurrentFrame.width = frame->width;
	m_CurrentFrame.data = new char[m_CurrentFrame.height*m_CurrentFrame.linesize];


	memcpy (m_CurrentFrame.data , p0, m_CurrentFrame.linesize*m_CurrentFrame.height);
	
	//for (int i = 0; i < m_CurrentFrame.height*m_CurrentFrame.linesize;i++)
	//{
	//	sprintf(str, "%3d ", m_CurrentFrame.data[i]);
	//	OutputDebugStringA(str);
	//	if (i%m_CurrentFrame.linesize == 0)
	//	{
	//		OutputDebugStringA("\n");
	//	}
	//}
	
	return;
}

void MpegDecode::Reset(char * file)
{
	frame = av_frame_alloc();
	filt_frame = av_frame_alloc();

	if (!frame || !filt_frame) {
		perror("Could not allocate frame");
		exit(1);
	}


	avfilter_register_all();

	if ((ret = open_input_file(file)) < 0)
		goto end;
	if ((ret = init_filters(filter_descr)) < 0)
		goto end;

	//av_seek_frame(this->fmt_ctx, 0, 100, AVSEEK_FLAG_FRAME);

	return;
end:
	avfilter_graph_free(&filter_graph);
	avcodec_free_context(&dec_ctx);
	avformat_close_input(&fmt_ctx);
	av_frame_free(&frame);
	av_frame_free(&filt_frame);
	return;
}

bool MpegDecode::NextFrame()
{
	int S_flag = 0;
	while (1) {
		S_flag = 0;
		if ((ret = av_read_frame(fmt_ctx, &packet)) < 0)
			break;

		if (packet.stream_index == video_stream_index) {
			ret = avcodec_send_packet(dec_ctx, &packet);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
				break;
			}

			while (ret >= 0) {
				ret = avcodec_receive_frame(dec_ctx, frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
					break;
				}
				else if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
					goto end;
				}

				if (ret >= 0) {
					frame->pts = frame->best_effort_timestamp;

					/* push the decoded frame into the filtergraph */
					if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
						av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
						break;
					}

					/* pull filtered frames from the filtergraph */
					while (1) {
						ret = av_buffersink_get_frame(buffersink_ctx, filt_frame);
						if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
							break;
						if (ret < 0)
							goto end;
						S_flag = 1;
						display_frame(filt_frame, buffersink_ctx->inputs[0]->time_base);
						
						av_frame_unref(filt_frame);
					}
					av_frame_unref(frame);
				}
			}
		}
		av_packet_unref(&packet);
		if (S_flag == 1)
		{
			goto finish;
		}
	}

	
end:
	return 0;
finish:
	return 1;

}

void MpegDecode::End()
{
	
	avfilter_graph_free(&filter_graph);
	avcodec_free_context(&dec_ctx);
	avformat_close_input(&fmt_ctx);
	av_frame_free(&frame);
	av_frame_free(&filt_frame);

	if (ret < 0 && ret != AVERROR_EOF) {
		//fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
		//exit(1);
	}
}

int MpegDecode::init_filters(const char *filters_descr)
{
	char args[512];
	int ret = 0;
	const AVFilter *buffersrc = avfilter_get_by_name("buffer");
	const AVFilter *buffersink = avfilter_get_by_name("buffersink");
	AVFilterInOut *outputs = avfilter_inout_alloc();
	AVFilterInOut *inputs = avfilter_inout_alloc();
	AVRational time_base = fmt_ctx->streams[video_stream_index]->time_base;
	enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_BGR0, AV_PIX_FMT_NONE };

	filter_graph = avfilter_graph_alloc();
	if (!outputs || !inputs || !filter_graph) {
		ret = AVERROR(ENOMEM);
		goto end;
	}

	/* buffer video source: the decoded frames from the decoder will be inserted here. */
	snprintf(args, sizeof(args),
		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
		dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
		time_base.num, time_base.den,
		dec_ctx->sample_aspect_ratio.num, dec_ctx->sample_aspect_ratio.den);

	ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
		args, NULL, filter_graph);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
		goto end;
	}

	/* buffer video sink: to terminate the filter chain. */
	ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
		NULL, NULL, filter_graph);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
		goto end;
	}

	ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
		AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
		goto end;
	}

	/*
	* Set the endpoints for the filter graph. The filter_graph will
	* be linked to the graph described by filters_descr.
	*/

	/*
	* The buffer source output must be connected to the input pad of
	* the first filter described by filters_descr; since the first
	* filter input label is not specified, it is set to "in" by
	* default.
	*/
	outputs->name = av_strdup("in");
	outputs->filter_ctx = buffersrc_ctx;
	outputs->pad_idx = 0;
	outputs->next = NULL;

	/*
	* The buffer sink input must be connected to the output pad of
	* the last filter described by filters_descr; since the last
	* filter output label is not specified, it is set to "out" by
	* default.
	*/
	inputs->name = av_strdup("out");
	inputs->filter_ctx = buffersink_ctx;
	inputs->pad_idx = 0;
	inputs->next = NULL;

	if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
		&inputs, &outputs, NULL)) < 0)
		goto end;

	if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
		goto end;

end:
	avfilter_inout_free(&inputs);
	avfilter_inout_free(&outputs);

	return ret;
}