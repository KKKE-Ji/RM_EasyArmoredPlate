#include"head.h"
//灯条类：便于绘图
class LineDraw
{
	LineDraw() {};
	//边框定义
public:
	float wide;
	float high;
	float angle;
	double area;
	cv::Point2f center;

public:
	//套上 旋转矩形便于识别
	LineDraw(const cv::RotatedRect& light)
	{
		wide = light.size.width;
		high = light.size.height;
		center = light.center;
		angle = light.angle;
		area = light.size.area();
	}
	const LineDraw& operator =(const LineDraw& ld)
	{
		this->wide = ld.wide;
		this->high = ld.high;
		this->center = ld.center;
		this->angle = ld.angle;
		this->area = ld.area;
		return *this;
	}
	cv::RotatedRect rec() const
	{
		return cv::RotatedRect(center, cv::Size2f(wide, high), angle);
	}
};

//-----------------------------图像预处理------------------------------//
Mat img_processing(Mat frame)
{
	vector<Mat>channels;
	//分离色彩通道(通道相减法“红色”)
	split(frame, channels);
	Mat red_channels = channels.at(2);
	Mat blue_channels = channels.at(0);
	red_channels -= blue_channels;

	//图像二值化
	inRange(red_channels, Scalar(100, 40, 50), Scalar(200, 200, 200), red_channels);
	//高斯滤波降噪
	GaussianBlur(red_channels, red_channels, Size(3, 3), 0, 0);
	//膨胀处理扩充边缘
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));//得到算子
	dilate(red_channels, red_channels, element);
	//闭运算
	morphologyEx(red_channels, red_channels, MORPH_CLOSE, element);
	return red_channels;
}
//------------------------------------------轮廓---------------------------------//
Mat findcon(Mat deimg, Mat red_channels, vector<vector<Point> >& lightContours)
{
	//找轮廓
	findContours(red_channels, lightContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (size_t i = 0; i < lightContours.size(); i++)
	{

		drawContours(deimg, lightContours, i, Scalar(0, 0, 255), 1, 8);
	}
	return deimg;
}
//------------------------------角度限制----------------------------------//
void adjustRec(cv::RotatedRect& rec)
{
	using std::swap;

	float& width = rec.size.width;
	float& height = rec.size.height;
	float& angle = rec.angle;



	while (angle >= 90.0) angle -= 180.0;
	while (angle < -90.0) angle += 180.0;


	if (angle >= 45.0)
	{
		swap(width, height);
		angle -= 90.0;
	}
	else if (angle < -45.0)
	{
		swap(width, height);
		angle += 90.0;
	}
}
//---------------------------------灯条检测-------------------------------//
//          传入（轮廓数组）->灯条数组
void filtercon(vector<vector<Point> >& lightContours, vector<LineDraw>& lightInfos)
{
	for (const auto& contour : lightContours)//遍历
	{
		//筛选
		double light_contourArea = contourArea(contour);
		if (light_contourArea < light_min_area)
		{
			continue;
		}
		//椭圆拟合
		RotatedRect light_rec = fitEllipse(contour);
		adjustRec(light_rec);
		if (light_rec.size.width / light_rec.size.height > light_max_ratio || light_contourArea / light_rec.size.area() < light_contour_min_solidity)
		{
			continue;
		}
		light_rec.size.width *= light_rec_expand;
		light_rec.size.height *= light_rec_expand;
		lightInfos.push_back(LineDraw(light_rec));
	}
}

//-------------------------------------------------------绘制旋转矩形---------------------------------//
//void drawLightInfo(vector<LineDraw>& LD, Mat& frame)
void drawLightInfo(vector<LineDraw>& LD, vector<LineDraw>& lightInfos, Mat& frame)
{
	vector<std::vector<cv::Point> > cons;

	long int i = 0;
	for (auto& light_info : LD)
	{
		RotatedRect rotate = light_info.rec();
		auto vertices = new cv::Point2f[4];
		rotate.points(vertices);
		vector<Point> con;
		for (int i = 0; i < 4; i++) 
		{
			con.push_back(vertices[i]);
		}
		cons.push_back(con);
		drawContours(frame, cons, i, Scalar(255, 255, 255), 1, 8);

		i++;
		for (size_t t = 0; t < lightInfos.size(); t++) 
		{
			for (size_t j = t + 1; (j < lightInfos.size()); j++)
			{
				float wideth = lightInfos[t].center.x - lightInfos[j].center.x;
				float highth = Light_high;
				float rate =  highth/wideth ;
				float l_point_y = lightInfos[t].center.y + lightInfos[t].high/2;
				float r_point_y = lightInfos[j].center.y - lightInfos[j].high/2;
				float rate_y = wideth / (l_point_y - r_point_y);
				if (rate > light_rate_up||
					rate < light_rate_down||
					rate_y>rate_up_y||
					rate_y<rate_down_y
					)
					continue;
				cv::Point2f lLight(lightInfos[t].center.x, l_point_y);
				cv::Point2f rLight(lightInfos[j].center.x, r_point_y);
				rectangle(frame,lLight,rLight, Scalar(0, 255, 0), 2);
			}
		}
	}
}





