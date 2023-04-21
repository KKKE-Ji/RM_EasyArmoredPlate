#pragma once
#include<opencv2\\opencv.hpp>
#include<iostream>

#include<math.h>
#define video "D:\\zkq\\openCV_Code\\picture\\3.mp4"
using namespace cv;
using namespace std;

/*------------------存储条件---（可修改）-----------*/
#define light_min_area 100
#define light_max_angle 45.0
#define light_min_size 5.0
#define light_contour_min_solidity 0.5
#define light_max_ratio 0.5
#define light_rec_expand 1.4
#define light_rate_up 2.2
#define light_rate_down 0.7
#define Light_high 65
#define rate_up_y 1.5
#define rate_down_y 0.7

