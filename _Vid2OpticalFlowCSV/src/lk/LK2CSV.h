#pragma once

#include "util/VideoReaders/AbstractVideoReader.h"
#include "util/Util.h"
#include "util/LensDistortionCorrector.h"
#include "lk/ConfigReader.h"
#include <unistd.h>
#include <sys/stat.h>
#include "lk/TiledMotion.h"
#include <ctime>

namespace LK2CSV
{
	static void DumpVideoInfo(AbstractVideoReader* vc, FILE* f, std::string video_file_name)
	{
		int fps = vc->GetFPS();
		int cell_width = cvRound(IMG_WIDTH/BLOCK_WIDTH_IMG_WIDTH_RATIO);
		int cell_height = cvRound(IMG_HEIGHT/BLOCK_HEIGHT_IMG_HEIGHT_RATIO);

		fprintf(f, "FILE_NAME=%s,", video_file_name.c_str());
		fprintf(f, "START_FRAME=%d,", START_FRAME);
		fprintf(f, "END_FRAME=%d,", END_FRAME);
		fprintf(f, "IMG_WIDTH=%d,", IMG_WIDTH);
		fprintf(f, "IMG_HEIGHT=%d,", IMG_HEIGHT);
		fprintf(f, "PROCESSING_WIDTH=%d,", PROCESSING_WIDTH);
		fprintf(f, "PROCESSING_HEIGHT=%d,", PROCESSING_HEIGHT);
		fprintf(f, "NUM_BLOCKS_X=%d,", NUM_BLOCKS_X);
		fprintf(f, "NUM_BLOCKS_Y=%d,", NUM_BLOCKS_Y);
		fprintf(f, "BLOCK_WIDTH=%d,", cell_width);
		fprintf(f, "BLOCK_HEIGHT=%d,", cell_height);
		fprintf(f, "FRAME_SKIP=%d,", FRAME_SKIP);
		fprintf(f, "FPS=%d", fps);
		fprintf(f, "\n");
	}

	static void GenerateLKData(std::string video_inp, std::string out_csv, std::string config_file, std::string dump_dir, std::string lens_params)
	{
		FILE* traj_file;
		traj_file = fopen(out_csv.c_str(),"w");
		if(!traj_file){
			PRINTF("Could not open output file for writing: %s\n", out_csv.c_str());
			return;
		}else{
			PRINTF("Output CSV file is: %s\n", out_csv.c_str());
		}

		if(!Util::FileExists(config_file.c_str())){
			PRINTF("Config file does not exists: %s\n", config_file.c_str());
			return;
		}else{
			PRINTF("Output Config file is: %s\n", config_file.c_str());
		}

		ConfigReader cf;

		if(!Util::FileExists(config_file.c_str())){
			PRINTF("Config file does not exists: %s\n", config_file.c_str());
			return;
		}
		cf.Read(config_file.c_str());

		if(!Util::FileExists(video_inp.c_str())){
			PRINTF("Video file does not exists: %s\n", video_inp.c_str());
			return;
		}else{
			PRINTF("Input Video file: %s\n", video_inp.c_str());
		}

		AbstractVideoReader* vc = AbstractVideoReader::Create(AbstractVideoReader::READER_CV);
		vc->Open(video_inp.c_str());

		Mat frame;
		*vc >> frame;

		if(frame.rows <= 0){
			PRINTF("Couldn't capture video: %s\n", video_inp.c_str());
			return;
		}

		IMG_WIDTH = frame.cols; IMG_HEIGHT = frame.rows;

		Mat resized_frame; cv::Size rescaled_sz; bool do_resize = false;
		if(fabs(LK_FRAME_RESCALE-1.0) > 0.01){
			do_resize = true;
			PROCESSING_WIDTH = cvRound(frame.cols*LK_FRAME_RESCALE);
			PROCESSING_HEIGHT = cvRound(frame.rows*LK_FRAME_RESCALE);
			rescaled_sz = cv::Size(PROCESSING_WIDTH, PROCESSING_HEIGHT);
		}else
		{
			PROCESSING_HEIGHT = IMG_HEIGHT;
			PROCESSING_WIDTH = IMG_WIDTH;
		}

		int ef = vc->GetTotalFrames();
		END_FRAME = MIN(END_FRAME, ef-1);

		DumpVideoInfo(vc, traj_file, video_inp);

		TiledMotion tr_finder;
		IplImage ipl_frame;

		bool dump_frames = !dump_dir.empty();
		std::string out_file_format_str;
		if(dump_frames)
		{
			PRINTF("Creating Directory for dumping frames: %s\n", dump_dir.c_str());
			mkdir(dump_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			if(!Util::FileExists(dump_dir.c_str())){
				PRINTF("Could not create directory for dumping frames: %s\n", dump_dir.c_str());
				return;
			}
			int num_zeros = cvCeil(log10(END_FRAME*1.0)) + 1;
			if(num_zeros < 5)
				num_zeros = 5;
			char temp_arr[500];
			sprintf(temp_arr, "%s/frame_%%0%dd.png", dump_dir.c_str(), num_zeros);
			out_file_format_str = std::string(temp_arr);
		}

		char filename[500];

		LensDistortionCorrector ldc;
		bool undist = !lens_params.empty();
		if (undist)
		{
			if(!Util::FileExists(lens_params.c_str())){
				PRINTF("File for Lens distorion params does not exists: %s\n", lens_params.c_str());
				return;
			}else{
				PRINTF("Lens distorion correction using: %s\n", lens_params.c_str());
			}
			ldc.Calibrate(lens_params.c_str());
		}

		cv::Mat corrected_frame;

		time_t start,
           start_total,
           end; 
		
		time(&start);

		for(int i=START_FRAME; i<END_FRAME; i+=(FRAME_SKIP+1))
		{
			//PRINTF("Processing Frame %d of %d\n", i, END_FRAME);
			vc->SetFrameNum(i);
			*vc >> frame;


			if (undist)
			{
				ldc.Undistort(frame,corrected_frame);
			}
			else
			{
				corrected_frame = frame;
			}

			if(do_resize)
			{
				resize(corrected_frame, resized_frame, rescaled_sz);
				ipl_frame = resized_frame;
			}else
			{
				ipl_frame = corrected_frame;
			}

			if(dump_frames){
				sprintf(filename, out_file_format_str.c_str(), i);
				imwrite(filename, corrected_frame);
			}

			tr_finder.FindTrajectories(i, &ipl_frame, traj_file);

			if(DISPLAY_LK_OUTPUT)
				waitKey(10);
		}

		time(&end);
    	double elapsed_secs = difftime(end,start);

		std::cout << "time: " << elapsed_secs << "s." << std::endl;

		fflush(traj_file);
		fclose(traj_file);
	}


};
