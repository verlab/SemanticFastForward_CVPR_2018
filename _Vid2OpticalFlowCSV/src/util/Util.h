#pragma once

namespace Util
{
	//void GetDirectoryListing(std::string directory, std::string ext_to_match, std::vector<std::string>& listing);
	bool FileExists(const char *filename );	
	std::vector<std::string> Tokenize(const std::string& str, const std::string& delimiters);
	std::string GetFileExt(std::string& filename);
	//void FindFrameRange(std::string directory,std::string frame_file_pattern,int &first_frame,int &last_frame);

	cv::Mat FindHomography_SIFT(cv::Mat& cur, cv::Mat& next, cv::Mat& warped_next,
		double min_trans_x=0, double min_trans_y=0, double max_trans_x=1000, double max_trans_y=1000);

	static Mat rotateImage(IplImage* frame, double angle)
	{
		Mat source(frame);
		Point2d src_center(source.cols/2.0F, source.rows/2.0F);
		Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
		Mat dst;
		warpAffine(source, dst, rot_mat, source.size());
		return dst;
	}

	template<class T>
	static std::vector<std::vector<T> > ReadCSV(std::string csv_filename)
	{
		std::vector<std::vector<T> > csv_vec;

		std::ifstream file ( csv_filename.c_str() ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		std::string value_string, line_string;
		while ( file.good() )
		{
			std::getline ( file, line_string, '\n' );
			std::stringstream line_stream;
			line_stream << line_string;

			std::vector<T> row;
			while(line_stream.good())
			{
				std::getline ( line_stream, value_string, ',' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/

				std::stringstream value_stream;
				value_stream << value_string;
				T value;
				value_stream >> value;
				if(value_stream)
					row.push_back(value);

			}
			if(!row.empty())
				csv_vec.push_back(row);
		}

		return csv_vec;
	}

	vector<double> RandArray(int size, int64 key);
	std::vector<cv::Point2d> AddMorePointsInVicinity(std::vector<cv::Point2d>& inp, double radius, int num_points, int64 key);
	double Round(double f); 
	std::vector<cv::Point2d> Shift(std::vector<cv::Point2d>& inp, cv::Point2d offset);
	void Perturb(std::vector<cv::Point2d>& inp, double amplitude, int64 key=-1LL);
	template<class T>
	static T Min(std::vector<T>& points)
	{
		T min_p = points.front();
		int size = (int)points.size();
		for(int i=1; i<size; ++i)
		{
			T& pt = points[i];
			if(min_p.x > pt.x)
				min_p.x = pt.x;
			if(min_p.y > pt.y)
				min_p.y = pt.y;
		}
		return min_p;
	}

	template<class T>
	static T Max(std::vector<T>& points)
	{
		T max_p = points.front();
		int size = (int)points.size();
		for(int i=1; i<size; ++i)
		{
			T& pt = points[i];
			if(max_p.x < pt.x)
				max_p.x = pt.x;
			if(max_p.y < pt.y)
				max_p.y = pt.y;
		}
		return max_p;
	}

	template<class T>
	static void ScatterPlot(std::vector<T>& points, const char* win_name, int width=500, int height=500)
	{
		T max_p = Max<T>(points);
		T min_p = Min<T>(points);

		int w = cvCeil(max_p.x - min_p.x);
		int h = cvCeil(max_p.y - min_p.y);
		double scale_x = width/w;
		double scale_y = height/h;

		int radius = MIN(width/50,5);
		radius = MAX(radius, 1);

		radius=5;

		cv::Mat img(height, width, CV_8UC3, Scalar(255,255,255));
		int num_points = (int)points.size();
		for(int i=0; i<num_points; ++i)
		{
			T pt = points[i] - min_p;
			int x = cvRound(pt.x*scale_x);
			int y = cvRound(pt.y*scale_y);
			cv::circle(img, cv::Point(x,y), radius, Scalar(255,0,0));
		}

		char win_name_app[500];
		sprintf(win_name_app, "%s (%.1f,%.1f)(%.1f,%.1f)", win_name, (double)min_p.x,(double)min_p.y,(double)max_p.x,(double)max_p.y);

		imshow(win_name_app,img);
	}
};
