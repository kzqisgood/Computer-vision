#include "pch.h"
#include "ImageProcess.h"
#include "math.h"
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <queue>

CImageProcess::CImageProcess(void)
{
}

CImageProcess::~CImageProcess(void)
{
}

// [ ******** ] ..........................................................
// [ 图像平移 ] ..........................................................
// [ ******** ] ..........................................................
CTMatrix< RGB_TRIPLE > CImageProcess::Image_translation(const CTMatrix< RGB_TRIPLE >& color_image, long delta_x, long delta_y)
{
	long image_height = color_image.Get_height();
	long image_width = color_image.Get_width();

	long new_height = image_height + abs(delta_y);
	long new_width = image_width + abs(delta_x);

	CTMatrix< RGB_TRIPLE > translated_image(new_height, new_width);

	for (int row = 0; row < new_height; row++)
		for (int column = 0; column < new_width; column++)
		{
			int row_mapped = row;
			if (delta_y > 0)
			{
				row_mapped = row - delta_y;
			}

			int column_mapped = column;
			if (delta_x > 0)
			{
				column_mapped = column - delta_x;
			}

			if (color_image.Is_point_valid(CImagePoint(row_mapped, column_mapped)))
			{
				translated_image[row][column] = color_image[row_mapped][column_mapped];
			}
			else
			{
				translated_image[row][column] = RGB_TRIPLE(128, 128, 128);
			}
		}

	return translated_image;
}

// [ ******** ] ..........................................................
// [ 图像镜像 ] ..........................................................
// [ ******** ] ..........................................................
CTMatrix< RGB_TRIPLE > CImageProcess::Image_mirror(const CTMatrix< RGB_TRIPLE >& color_image, bool is_horizontal)
{
	long image_height = color_image.Get_height();
	long image_width = color_image.Get_width();
	CTMatrix< RGB_TRIPLE > dest_image(image_height, image_width);
	for (long row = 0; row < image_height; row++)
		for (long column = 0; column < image_width; column++)
		{
			long source_row = row;
			long source_column = column;
			if (is_horizontal == true)
				source_column = image_width - column - 1;
			else
				source_row = image_height - row - 1;
			dest_image[row][column] = color_image[source_row][source_column];
		}
	return dest_image;
}

// [ ******** ] ..........................................................
// [ 图像仿射变换 ] ..........................................................
// [ ******** ] ..........................................................
CTMatrix< RGB_TRIPLE > CImageProcess::Image_affine(const CTMatrix< RGB_TRIPLE >& color_image, double m00, double m01, double m02, double m10, double m11, double m12)
{
	long source_height = color_image.Get_height();
	long source_width = color_image.Get_width();
	long dest_height = source_height * 2;
	long dest_width = source_width * 2;
	CTMatrix< RGB_TRIPLE > dest_image(dest_height, dest_width);
	for (int dest_row = 0; dest_row < dest_height; dest_row++)
		for (int dest_column = 0; dest_column < dest_width; dest_column++)
		{
			int source_column = int(m00 * dest_column + m01 * dest_row + m02);
			int source_row = int(m10 * dest_column + m11 * dest_row + m12);
			if (color_image.Is_point_valid(CImagePoint(source_row, source_column)))
			{
				dest_image[dest_row][dest_column] = color_image[source_row][source_column];
			}
			else
			{
				dest_image[dest_row][dest_column] = RGB_TRIPLE(128, 128, 128);
			}
		}
	return dest_image;
}

// [ ******** ] ..........................................................
// [ 图像转置 ] ..........................................................
// [ ******** ] ..........................................................
CTMatrix< RGB_TRIPLE > CImageProcess::Image_transpose(const CTMatrix< RGB_TRIPLE >& color_image)
{
	long image_height = color_image.Get_height();
	long image_width = color_image.Get_width();
	CTMatrix< RGB_TRIPLE > dest_image(image_width, image_height);
	for (long row = 0; row < image_width; row++)
		for (long column = 0; column < image_height; column++)
		{
			long source_row = column;
			long source_column = row;
			dest_image[row][column] = color_image[source_row][source_column];
		}
	return dest_image;
}

// [ ******** ] ..........................................................
// [ 图像旋转 ] ..........................................................
// [ ******** ] ..........................................................
CTMatrix< RGB_TRIPLE > CImageProcess::Image_rotation(const CTMatrix< RGB_TRIPLE >& color_image, long degrees)
{
	long source_height = color_image.Get_height();
	long source_width = color_image.Get_width();

	CImagePoint source_center(source_height / 2, source_width / 2);

	long target_height = long(sqrt(double(source_height * source_height + source_width * source_width)));
	long target_width = target_height;

	CTMatrix< RGB_TRIPLE > target_image(target_height, target_width);

	CImagePoint target_center(target_height / 2, target_width / 2);

	double cosine_of_degree = cos(3.14159265358979323846 * degrees / 180.0);
	double sine_of_degree = sin(3.14159265358979323846 * degrees / 180.0);

	for (int target_row = 0; target_row < target_height; target_row++)
		for (int target_column = 0; target_column < target_width; target_column++)
		{
			int source_row = int(-sine_of_degree * (target_column - target_center.m_column)
				+ cosine_of_degree * (target_row - target_center.m_row));
			int source_column = int(cosine_of_degree * (target_column - target_center.m_column)
				+ sine_of_degree * (target_row - target_center.m_row));

			source_row += source_center.m_row;
			source_column += source_center.m_column;

			if (color_image.Is_point_valid(CImagePoint(source_row, source_column)))
			{
				target_image[target_row][target_column] = color_image[source_row][source_column];
			}
			else
			{
				target_image[target_row][target_column] = RGB_TRIPLE(0, 0, 0);
			}
		}

	return target_image;
}

// [ ******** ] ..........................................................
// [ 图像缩放 ] ..........................................................
// [ ******** ] ..........................................................
CTMatrix< RGB_TRIPLE > CImageProcess::Image_zoom(const CTMatrix< RGB_TRIPLE >& color_image, double horizontal_scale, double vertical_scale)
{
	long source_height = color_image.Get_height();
	long source_width = color_image.Get_width();

	long dest_height = long(source_height * vertical_scale);
	long dest_width = long(source_width * horizontal_scale);

	CTMatrix< RGB_TRIPLE > dest_image(dest_height, dest_width);

	for (int dest_row = 0; dest_row < dest_height; dest_row++)
		for (int dest_column = 0; dest_column < dest_width; dest_column++)
		{
			long source_row = long(dest_row / vertical_scale);
			long source_column = long(dest_column / horizontal_scale);

			if (color_image.Is_point_valid(CImagePoint(source_row, source_column)))
				dest_image[dest_row][dest_column] = color_image[source_row][source_column];
			else
				dest_image[dest_row][dest_column] = RGB_TRIPLE(128, 128, 128);
		}

	return dest_image;
}

// [ *************** ] ...................................................
// [ Robert 边缘算子 ] ...................................................
// [ *************** ] ...................................................
CTMatrix< BYTE > CImageProcess::Robert_edge_operator(const CTMatrix< BYTE >& gray_image)
{
	long image_height = gray_image.Get_height();
	long image_width = gray_image.Get_width();

	CTMatrix< BYTE > edge_image(image_height, image_width);

	for (int row = 0; row < image_height; row++)
		for (int column = 0; column < image_width; column++)
		{
			if (row == 0 || row == image_height - 1 || column == 0 || column == image_width - 1)
			{
				edge_image[row][column] = 0;
			}
			else
			{
				edge_image[row][column] = max(abs(gray_image[row - 1][column - 1] - gray_image[row + 1][column + 1]),
					abs(gray_image[row - 1][column + 1] - gray_image[row + 1][column - 1]));
			}
		}

	return edge_image;
}



// [ ************ ] ......................................................
// [ 二维卷积操作 ] ......................................................
// [ ************ ] ......................................................
CTMatrix< BYTE > CImageProcess::Convolution_operation(const CTMatrix< BYTE >& gray_image, const CTMatrix< float >& mask)
{
	float sum_of_mask = 0;
	for (int row = 0; row < mask.Get_height(); row++)
		for (int column = 0; column < mask.Get_width(); column++)
			sum_of_mask += mask[row][column];
	if (sum_of_mask != 0)
		for (int row = 0; row < mask.Get_height(); row++)
			for (int column = 0; column < mask.Get_width(); column++)
				mask[row][column] /= sum_of_mask;

	int mask_half_height = mask.Get_height() / 2;
	int mask_half_width = mask.Get_width() / 2;

	CTMatrix< BYTE > edge_image = gray_image;

	for (int row = 0; row < gray_image.Get_height(); row++)
		for (int column = 0; column < gray_image.Get_width(); column++)
			if (row < mask_half_height || row > gray_image.Get_height() - 1 - mask_half_height ||
				column < mask_half_width || column > gray_image.Get_width() - 1 - mask_half_width)
			{
				edge_image[row][column] = 0;
			}
			else
			{
				double sum_of_convolution = 0;

				for (int sub_row = 0; sub_row < mask.Get_height(); sub_row++)
					for (int sub_column = 0; sub_column < mask.Get_width(); sub_column++)
						sum_of_convolution += mask[sub_row][sub_column]
						* gray_image[row + sub_row - mask_half_height][column + sub_column - mask_half_width];

				edge_image[row][column] = BYTE( abs(sum_of_convolution));
			}

	return edge_image;
}

// [ ************************** ] ........................................
// [ Sobel 边缘算子（梯度强度） ] ........................................
// [ ************************** ] ........................................
CTMatrix< BYTE > CImageProcess::Sobel_edge_operator(const CTMatrix< BYTE >& gray_image)
{
	CTMatrix< BYTE > horizontal = Sobel_edge_horizontal(gray_image);
	CTMatrix< BYTE > vertical = Sobel_edge_vertical(gray_image);

	CTMatrix< BYTE > edge_image = gray_image;

	for (int row = 0; row < gray_image.Get_height(); row++)
		for (int column = 0; column < gray_image.Get_width(); column++)
		{
			edge_image[row][column] = BYTE(sqrt(double(horizontal[row][column] * horizontal[row][column]
				+ vertical[row][column] * vertical[row][column])) / 1.414);
		}

	return edge_image;
}


// [ ************************** ] ........................................
// [ Sobel 边缘算子（垂直方向） ] ........................................
// [ ************************** ] ........................................
CTMatrix< BYTE > CImageProcess::Sobel_edge_vertical(const CTMatrix< BYTE >& gray_image)
{
	CTMatrix< float > mask_one(1, 3);
	CTMatrix< float > mask_two(3, 1);

	mask_one[0][0] = 1;
	mask_one[0][1] = 2;
	mask_one[0][2] = 1;

	mask_two[0][0] = 1;
	mask_two[1][0] = 0;
	mask_two[2][0] = -1;

	CTMatrix< BYTE > temp = Convolution_operation(gray_image, mask_one);
	return Convolution_operation(temp, mask_two);
}

// [ ************************** ] ........................................
// [ Sobel 边缘算子（水平方向） ] ........................................
// [ ************************** ] ........................................
CTMatrix< BYTE > CImageProcess::Sobel_edge_horizontal(const CTMatrix< BYTE >& gray_image)
{
	CTMatrix< float > mask_one(1, 3);
	CTMatrix< float > mask_two(3, 1);

	mask_one[0][0] = 1;
	mask_one[0][1] = 0;
	mask_one[0][2] = -1;

	mask_two[0][0] = 1;
	mask_two[1][0] = 2;
	mask_two[2][0] = 1;

	CTMatrix< BYTE > temp = Convolution_operation(gray_image, mask_one);
	return Convolution_operation(temp, mask_two);
}

struct PixlNode // 记录Canny算子中双阈值法中第一次扫描不能处理点结构
{
	int h;
	int w;
	int gradvalue;
	PixlNode* next;
};

// [ **************** ] ..................................................
// [ 高斯滤波数组操作 ] ..................................................
// [ **************** ] ..................................................
void GausFilterOp(int height, int width, CTMatrix<BYTE>& matrix)
{
	CTMatrix<BYTE> tm = matrix;
	int h = 0, w = 0;
	int temp = 0;

	for (h = 1; h < height - 1; h++)
	{
		for (w = 1; w < width - 1; w++)
		{
			temp = (tm[h - 1][w - 1] + 2 * tm[h - 1][w] + tm[h - 1][w + 1]
				+ 2 * tm[h][w - 1] + 4 * tm[h][w] + 2 * tm[h][w + 1]
				+ tm[h + 1][w - 1] + 2 * tm[h + 1][w] + tm[h + 1][w + 1]) / 12;

			if (temp > 255)
			{
				temp = 255;
			}
			matrix[h][w] = (BYTE)temp;
		}
	}
}

// [ ************************************* ] .............................
// [ 求Canny算子中处于高低阈值之间的像素点 ] .............................
// [ ************************************* ] .............................
void CannyPixlProc(struct PixlNode** ph, struct PixlNode** pt, CTMatrix<BYTE>& tm, int thi, int tlo, int& glcounter)
{
	struct PixlNode* p, * q;
	int    h = 0, w = 0;
	bool   flag1 = false, flag2 = false;

	// 处理高低阈值之间的像素点
	p = q = *ph;
	while (q)
	{
		h = q->h;
		w = q->w;

		if (tm[h - 1][w - 1] > thi || tm[h - 1][w] > thi ||
			tm[h - 1][w + 1] > thi || tm[h][w - 1] > thi ||
			tm[h][w + 1] > thi || tm[h + 1][w - 1] > thi ||
			tm[h + 1][w] > thi || tm[h + 1][w + 1] > thi)
		{ // 如果邻接点是边缘点则此点也是
			tm[h][w] = 255;
			glcounter--;
			flag1 = true;
		}
		else if (tm[h - 1][w - 1] < tlo && tm[h - 1][w] < tlo &&
			tm[h - 1][w + 1] < tlo && tm[h][w - 1] < tlo &&
			tm[h][w + 1] < tlo && tm[h + 1][w - 1] < tlo &&
			tm[h + 1][w] < tlo && tm[h + 1][w + 1] < tlo)
		{ // 邻接点都不是边缘点则此点也不是
			tm[h][w] = 0;
			glcounter--;
			flag2 = true;
		}
		else
		{ // 仍然不确定
			p = q;
			q = q->next;
		}
		if (flag1 == true || flag2 == true) // if0
		{ // 处理一个则从链表删除一个
			flag1 = flag2 = false;
			if (q == *ph)
			{ // 删除头部
				*ph = (*ph)->next;
				delete q;
				p = q = *ph;
			}
			else if (q == *pt)
			{ // 删除尾巴
				*pt = p;
				(*pt)->next = NULL;
				delete q;
				p = q = NULL;
			}
			else
			{ // 删除中间
				p->next = q->next;
				delete q;
				q = p->next;
			}
		} // end if0
	}// end while 
}

// [ ************** ] ....................................................
// [ Canny 边缘算子 ] ....................................................
// [ ************** ] ....................................................
CTMatrix< BYTE > CImageProcess::Canny_edge_operator(const CTMatrix< BYTE >& gray_image)
{
	CTMatrix<BYTE> temp_matrix, tm; // 灰度值数组
	CTMatrix<BYTE> di;              // 对应梯度方向角
	int temp, tparr[2];             // 临时变量
	int his[256];                   // 灰度直方图
	int width;                      // 图像宽度
	int height;                     // 图像高度
	int h = 0, w = 0;               // 数组下标 h：行 w：列
	int i = 0;                      // 数组下标访问变量
	int max = 0, min = 0;

	// 得到欲加工图像信息 图像宽度 图像高度	
	temp_matrix = tm = gray_image;
	width = gray_image.Get_width();
	height = gray_image.Get_height();

	// 分下面几个步骤实现 Canny 算子
	//
	// step 1. 高斯滤波消除噪声
	GausFilterOp(height, width, temp_matrix);

	// step 2. Prewitt模板求梯度大小、梯度方向角
	tm = di = temp_matrix;
	for (h = 1; h < height - 1; h++)
	{
		for (w = 1; w < width - 1; w++)
		{
			// 垂直方向
			tparr[0] = -tm[h - 1][w - 1] + tm[h - 1][w + 1]
				- tm[h][w - 1] + tm[h][w + 1]
				- tm[h + 1][w - 1] + tm[h + 1][w + 1];

			// 水平方向
			tparr[1] = -tm[h - 1][w - 1] - tm[h - 1][w] - tm[h - 1][w + 1]
				+ tm[h + 1][w - 1] + tm[h + 1][w] + tm[h + 1][w + 1];

			// a. 计算梯度值
			for (i = 0; i < 2; i++)
			{
				if (tparr[i] < 0)
				{
					tparr[i] = -tparr[i];
				}
				else if (tparr[i] > 255)
				{
					tparr[i] = 255;
				}
			}

			temp_matrix[h][w] = max(abs(tparr[0]), abs(tparr[1]));

			// b. 计算方向角
			temp = (int)(atan((double)tparr[0] / (double)tparr[1]));

			if (temp < 0)
			{
				temp += 180;
			}

			di[h][w] = (BYTE)(temp * 180 / 3.14);
		}
	}

	// step 3. 将方向角分类 0-22.5、157.5-180 是0度， 22.5-67.5 是45度，67.5-112.5是90度，112.5-157.5是135度
	for (h = 1; h < height - 1; h++)
	{
		for (w = 1; w < width - 1; w++)
		{
			temp = di[h][w];
			if ((temp >= 0 && temp <= 22.5) || (temp >= 157.5 && temp <= 180))
			{
				di[h][w] = 0;
			}
			else if (temp > 22.5 && temp <= 67.5)
			{
				di[h][w] = 45;
			}
			else if (temp > 67.5 && temp <= 112.5)
			{
				di[h][w] = 90;
			}
			else if (temp > 112.5 && temp <= 157.5)
			{
				di[h][w] = 135;
			}
			else
			{
				di[h][w] = -1;
			}
		}
	}

	// step 4. 非最大抑制 
	for (h = 1; h < height - 1; h++)
	{
		for (w = 1; w < width - 1; w++)
		{
			bool flag = true;

			if (di[h - 1][w] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w]) flag = false; // 上				
			if (di[h][w - 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h][w - 1]) flag = false; // 前
			if (di[h][w + 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h][w + 1]) flag = false; // 后				
			if (di[h + 1][w] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w]) flag = false; // 下
			if (di[h - 1][w - 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w - 1]) flag = false; // 左上
			if (di[h - 1][w + 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w + 1]) flag = false; // 右上
			if (di[h + 1][w - 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h + 1][w - 1]) flag = false; // 左下
			if (di[h + 1][w + 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w + 1]) flag = false; // 右下			

			if (flag == false)
			{
				temp_matrix[h][w] = 0;
			}
		}
	}

	// step 5. 双阈值法判断边缘点 
	//
	// step 5.1 选取阈值 tHigh tLow
	//
	int tHigh, tLow; // 用来寻找边缘点的阈值
	// 直方图初始化 
	for (i = 0; i < 256; i++)
	{
		his[i] = 0;
	}

	// 得到直方图 
	for (h = 1; h < height - 1; h++)
	{
		for (w = 1; w < width - 1; w++)
		{
			his[temp_matrix[h][w]]++;
		}
	}

	// 得到灰度值最大点
	h = 255;
	while (h--)
	{
		if (his[h] != 0)
		{
			max = h;
			break;
		}
	}

	// 得到灰度值最小点 
	h = 0;
	while (h++)
	{
		if (his[h] != 0)
		{
			min = h;
			break;
		}
	}

	// 得到双阈值 高阈值取 0.25 处、低阈值取高阈值的 0.4
	tHigh = (int)(min + 0.25 * (max - min));
	tLow = (int)(0.4 * tHigh);

	// step 5.2 利用双阈值找边缘点
	long int sum = 0;
	struct PixlNode* pHead, * pTail;

	pHead = pTail = NULL;
	int glcounter = (height - 4) * (width - 4);

	// step 5.2.1 首次处理 确定大多数点 记录不确定点
	for (h = 2; h < height - 2; h++) // for1
	{
		for (w = 2; w < width - 2; w++) // for2
		{

			if (temp_matrix[h][w] >= tHigh)
			{ // 大于高阈值肯定是边缘点
				temp_matrix[h][w] = 255;
				glcounter--;
			}
			else if (temp_matrix[h][w] < tLow)
			{ // 小于低阈值肯定不是边缘点
				temp_matrix[h][w] = 0;
				glcounter--;
			}
			else // else1
			{ // 在两者之间的通过判断邻接8像素点确定
				if (temp_matrix[h - 1][w - 1] > tHigh || temp_matrix[h - 1][w] > tHigh ||
					temp_matrix[h - 1][w + 1] > tHigh || temp_matrix[h][w - 1] > tHigh ||
					temp_matrix[h][w + 1] > tHigh || temp_matrix[h + 1][w - 1] > tHigh ||
					temp_matrix[h + 1][w] > tHigh || temp_matrix[h + 1][w + 1] > tHigh)
				{ // 如果邻接点是边缘点则此点也是
					temp_matrix[h][w] = 255;
					glcounter--;
				}
				else if (temp_matrix[h - 1][w - 1] < tLow && temp_matrix[h - 1][w] < tLow &&
					temp_matrix[h - 1][w + 1] < tLow && temp_matrix[h][w - 1] < tLow &&
					temp_matrix[h][w + 1] < tLow && temp_matrix[h + 1][w - 1] < tLow &&
					temp_matrix[h + 1][w] < tLow && temp_matrix[h + 1][w + 1] < tLow)
				{ // 邻接点都不是边缘点则此点也不是
					temp_matrix[h][w] = 0;
					glcounter--;
				}
				else // else2
				{ // 如果边缘点的灰度值在高低阈值之间则记录下来
					struct PixlNode* p = new struct PixlNode;
					if (p) // if1
					{
						sum++;
						p->h = h;
						p->w = w;
						p->gradvalue = temp_matrix[h][w];
						p->next = NULL;
						if (pHead == NULL)
						{
							pHead = pTail = p;
						}
						else
						{
							pTail->next = p;
							pTail = p;
						}
					}
				} // else2
			} // else1 
		} // for2
	} // for1


	// step 5.2.2 处理未确定点
	int glcounterpre = 0;
	while (glcounter != glcounterpre)
	{
		glcounterpre = glcounter;
		CannyPixlProc(&pHead, &pTail, temp_matrix, tHigh, tLow, glcounter);
	}

	// 假设最终不能处理掉的像素认为是和边缘无关的 置为 0
	struct PixlNode* q = pHead;
	while (q)
	{
		h = q->h;
		w = q->w;
		temp_matrix[h][w] = 0;
		q = q->next;
	}

	// 释放动态申请的内存
	q = pHead;
	while (q)
	{
		pHead = q->next;
		delete q;
		q = pHead;
	}

	return temp_matrix;
}

// [ **************** ] ..................................................
// [ Prewitt 边缘算子 ] ..................................................
// [ **************** ] ..................................................
CTMatrix< BYTE > CImageProcess::Prewitt_edge_operator(const CTMatrix< BYTE >& gray_image)
{
	float mask_one[3][3] = { { -1,  0,  1 },
								 { -1,  0,  1 },
								 { -1,  0,  1 } };
	float mask_two[3][3] = { { -1, -1, -1 },
								 { 0,   0,  0 },
								 { 1,   1,  1 } };

	CTMatrix< float > mask(3, 3);

	mask.ImportFrom(mask_one);
	CTMatrix< BYTE > horizontal = Convolution_operation(gray_image, mask);

	mask.ImportFrom(mask_two);
	CTMatrix< BYTE > vertical = Convolution_operation(gray_image, mask);

	CTMatrix< BYTE > edge_image = gray_image;

	for (int row = 0; row < gray_image.Get_height(); row++)
		for (int column = 0; column < gray_image.Get_width(); column++)
		{
			edge_image[row][column] = max(horizontal[row][column], vertical[row][column]);
		}

	return edge_image;
}

// [ ******** ] ..........................................................
// [ 区域生长 ] ..........................................................
// [ ******** ] ..........................................................
CTMatrix< BlackWhite > CImageProcess::Region_growing(const CTMatrix< BYTE >& gray_image, BYTE original_seed, BYTE range_of_similarity)
{
	long image_height = gray_image.Get_height();
	long image_width = gray_image.Get_width();

	CTMatrix< BlackWhite > growing_result(image_height, image_width);

	for (int row = 0; row < image_height; row++)
		for (int column = 0; column < image_width; column++)
		{
			if (gray_image[row][column] >= original_seed)
				growing_result[row][column] = White;
			else
				growing_result[row][column] = Black;
		}

	bool is_end = false;

	while (is_end == false)
	{
		is_end = true;

		for (int row = 0; row < image_height; row++)
			for (int column = 0; column < image_width; column++)
				if (growing_result[row][column] == White)
				{
					for (int sub_row = -1; sub_row <= 1; sub_row++)
						for (int sub_column = -1; sub_column <= 1; sub_column++)
						{
							int new_row = row + sub_row;
							int new_column = column + sub_column;

							if (gray_image.Is_point_valid(CImagePoint(new_row, new_column)))
								if (growing_result[new_row][new_column] == Black)
									if (abs(gray_image[row][column] - gray_image[new_row][new_column] < range_of_similarity))
									{
										is_end = false;
										growing_result[new_row][new_column] = White;
									}
						}
				}
	}

	return growing_result;
}

// [ ********** ] ........................................................
// [ 分水岭算法 ] ........................................................
// [ ********** ] ........................................................
CTMatrix< int > CImageProcess::Watershed(const CTMatrix< BYTE >& OriginalImage, BYTE seed_threshold)
{
	using namespace std;

	int image_height = OriginalImage.Get_height();
	int image_width = OriginalImage.Get_width();

	CTMatrix< char > SeedImage(image_height, image_width);

	for (int row = 0; row < image_height; row++)
		for (int column = 0; column < image_width; column++)
			SeedImage[row][column] = 0;
	//设置初始水位，淹没低于阈值的像素点，形成积水坑。
	unsigned char uszThre = seed_threshold;
	for (int i = 2; i < image_height - 2; i++)
		for (int k = 3; k < image_width - 3; k++)
		{
			BYTE uszTem = OriginalImage[i][k];
			if (uszTem < uszThre)
			{
				SeedImage[i][k] = 1;
			}
		}
	//用于标记当前像素点属用哪一个积水坑，0：不属于任何积水坑 1-n是积水坑编号
	CTMatrix< int > LabelImage(image_height, image_width);//分类

	for (int i = 0; i < image_height; i++)
	{
		for (int j = 0; j < image_width; j++)
			LabelImage[i][j] = 0;
	}

	int Num = 0;//积水坑编号
	int i, j;
	//记录各积水坑不同灰度像素值[0-255]像素点[n]的个数
	vector<int*> SeedCounts;//队列的长度
	queue<POINT> quetem;//像素点(队列中)结构临时变量
	//记录各积水坑不同灰度像素值[0-255]像素点的队列
	vector<queue<POINT>*> vque;//数组里头挂数组 颜色

	int* array;
	queue<POINT>* pque;
	POINT temp;

	int m, n, k = 0;
	BOOL up, down, right, left, upleft, upright, downleft, downright;//8 directions...

	for (i = 0; i < image_height; i++)
	{
		for (j = 0; j < image_width; j++)
		{
			if (SeedImage[i][j] == 1)//第一个水坑
			{
				Num++;//当前积水坑编号

				array = new int[256];
				ZeroMemory(array, 256 * sizeof(int));

				SeedCounts.push_back(array);//当前积水坑统计信息的初始化

				pque = new queue<POINT>[256];

				vque.push_back(pque);//当前积水坑的队列节点初始化

				temp.x = i;
				temp.y = j;
				quetem.push(temp);

				LabelImage[i][j] = Num;
				SeedImage[i][j] = 127;

				while (!quetem.empty())
				{
					up = down = right = left = FALSE;
					upleft = upright = downleft = downright = FALSE;

					temp = quetem.front();
					m = temp.x;
					n = temp.y;
					quetem.pop();//弹出队列

					if (m > 0)
					{
						if (SeedImage[m - 1][n] == 1)//当前节点的上边，第一个坐标指垂直方向
						{
							temp.x = m - 1;
							temp.y = n;
							quetem.push(temp);//入队列

							LabelImage[m - 1][n] = Num;//标记为哪一个坑
							SeedImage[m - 1][n] = 127;//标记被使用避免死循环
						}
						else
						{
							up = TRUE;
						}
					}
					if (m > 0 && n > 0)
					{
						if (SeedImage[m - 1][n - 1] == 1)
						{
							temp.x = m - 1;
							temp.y = n - 1;
							quetem.push(temp);

							LabelImage[m - 1][n - 1] = Num;
							SeedImage[m - 1][n - 1] = 127;
						}
						else
						{
							upleft = TRUE;
						}
					}

					if (m < image_height - 1)
					{
						if (SeedImage[m + 1][n] == 1)
						{
							temp.x = m + 1;
							temp.y = n;
							quetem.push(temp);

							LabelImage[m + 1][n] = Num;
							SeedImage[m + 1][n] = 127;
						}
						else
						{
							down = TRUE;
						}
					}
					if (m < (image_height - 1) && n < (image_width - 1))
					{
						if (SeedImage[m + 1][n + 1] == 1)
						{
							temp.x = m + 1;
							temp.y = n + 1;
							quetem.push(temp);

							LabelImage[m + 1][n + 1] = Num;
							SeedImage[m + 1][n + 1] = 127;
						}
						else
						{
							downright = TRUE;
						}
					}

					if (n < image_width - 1)
					{
						if (SeedImage[m][n + 1] == 1)
						{
							temp.x = m;
							temp.y = n + 1;
							quetem.push(temp);

							LabelImage[m][n + 1] = Num;
							SeedImage[m][n + 1] = 127;
						}
						else
						{
							right = TRUE;
						}
					}
					if (m > 0 && n < (image_width - 1))
					{
						if (SeedImage[m - 1][n + 1] == 1)
						{
							temp.x = m - 1;
							temp.y = n + 1;
							quetem.push(temp);

							LabelImage[m - 1][n + 1] = Num;
							SeedImage[m - 1][n + 1] = 127;
						}
						else
						{
							upright = TRUE;
						}
					}

					if (n > 0)
					{
						if (SeedImage[m][n - 1] == 1)
						{
							temp.x = m;
							temp.y = n - 1;
							quetem.push(temp);

							LabelImage[m][n - 1] = Num;
							SeedImage[m][n - 1] = 127;
						}
						else
						{
							left = TRUE;
						}
					}
					if (m < (image_height - 1) && n>0)
					{
						if (SeedImage[m + 1][n - 1] == 1)
						{
							temp.x = m + 1;
							temp.y = n - 1;
							quetem.push(temp);

							LabelImage[m + 1][n - 1] = Num;
							SeedImage[m + 1][n - 1] = 127;
						}
						else
						{
							downleft = TRUE;
						}
					}

					if (up || down || right || left ||
						upleft || downleft || upright || downright)//该点不是积水坑边上的点
					{
						temp.x = m;
						temp.y = n;
						vque[Num - 1][OriginalImage[m][n]].push(temp);//当前积水坑的像素值
						SeedCounts[Num - 1][OriginalImage[m][n]]++;//当前积水坑的灰度值像素
					}
				}
			}
		}
	}

	bool actives;
	int WaterLevel;

	for (WaterLevel = 0; WaterLevel < 180; WaterLevel++) //涨水到180
	{
		actives = true;//控制所有的水都处理一遍
		while (actives)
		{
			actives = false;

			for (i = 0; i < Num; i++)//积水坑下标
			{
				if (!vque[i][WaterLevel].empty())//查看该值是否为空，是否继续迭代
				{
					actives = true;
					while (SeedCounts[i][WaterLevel] > 0)
					{
						SeedCounts[i][WaterLevel]--;
						temp = vque[i][WaterLevel].front();//拿到节点

						vque[i][WaterLevel].pop();//弹出节点
						m = temp.x;
						n = temp.y;
						if (m > 0)
						{
							if (!LabelImage[m - 1][n])
							{
								temp.x = m - 1;
								temp.y = n;
								LabelImage[m - 1][n] = i + 1;//记录当前的坑

								if (OriginalImage[m - 1][n] <= WaterLevel)
								{
									vque[i][WaterLevel].push(temp);
								}
								else//水过来了，该点虽然很高但也算该坑。p115
								{
									vque[i][OriginalImage[m - 1][n]].push(temp);
									SeedCounts[i][OriginalImage[m - 1][n]]++;
								}
							}
						}

						if (m < image_height - 1)
						{
							if (!LabelImage[m + 1][n])
							{
								temp.x = m + 1;
								temp.y = n;
								LabelImage[m + 1][n] = i + 1;

								if (OriginalImage[m + 1][n] <= WaterLevel)
								{
									vque[i][WaterLevel].push(temp);
								}
								else
								{
									vque[i][OriginalImage[m + 1][n]].push(temp);
									SeedCounts[i][OriginalImage[m + 1][n]]++;
								}
							}
						}

						if (n < image_width - 1)
						{
							if (!LabelImage[m][n + 1])
							{
								temp.x = m;
								temp.y = n + 1;
								LabelImage[m][n + 1] = i + 1;

								if (OriginalImage[m][n + 1] <= WaterLevel)
								{
									vque[i][WaterLevel].push(temp);
								}
								else
								{
									vque[i][OriginalImage[m][n + 1]].push(temp);
									SeedCounts[i][OriginalImage[m][n + 1]]++;
								}
							}
						}

						if (n > 0)
						{
							if (!LabelImage[m][n - 1])
							{
								temp.x = m;
								temp.y = n - 1;
								LabelImage[m][n - 1] = i + 1;

								if (OriginalImage[m][n - 1] <= WaterLevel)
								{
									vque[i][WaterLevel].push(temp);
								}
								else
								{
									vque[i][OriginalImage[m][n - 1]].push(temp);
									SeedCounts[i][OriginalImage[m][n - 1]]++;
								}
							}
						}
					}
					SeedCounts[i][WaterLevel] = vque[i][WaterLevel].size();
				}
			}
		}
	}

	while (!vque.empty())//释放所有的点
	{
		pque = vque.back();
		delete[] pque;
		vque.pop_back();
	}
	while (!SeedCounts.empty())
	{
		array = SeedCounts.back();
		delete[] array;
		SeedCounts.pop_back();
	}

	return LabelImage;
}

double Distance_of_location_and_color(CImagePoint pixel_1, RGB_TRIPLE color_1,
	CImagePoint pixel_2, RGB_TRIPLE color_2,
	long image_height, long image_width)
{	//像素点一
	double first_1 = double(pixel_1.m_row) / double(image_height);
	double second_1 = double(pixel_1.m_column) / double(image_width);
	double third_1 = double(color_1.m_Red) / 255.0;	//红色通道归一到0-1区间
	double fourth_1 = double(color_1.m_Green) / 255.0;
	double fifth_1 = double(color_1.m_Blue) / 255.0;
	//像素点二
	double first_2 = double(pixel_2.m_row) / double(image_height);
	double second_2 = double(pixel_2.m_column) / double(image_width);
	double third_2 = double(color_2.m_Red) / 255.0;
	double fourth_2 = double(color_2.m_Green) / 255.0;
	double fifth_2 = double(color_2.m_Blue) / 255.0;

	return sqrt((first_1 - first_2) * (first_1 - first_2)
		+ (second_1 - second_2) * (second_1 - second_2)
		+ (third_1 - third_2) * (third_1 - third_2)
		+ (fourth_1 - fourth_2) * (fourth_1 - fourth_2)
		+ (fifth_1 - fifth_2) * (fifth_1 - fifth_2));//距离
}

int Update_index_into_clusters(CImagePoint current_pixel, RGB_TRIPLE current_color,
	CTArray< CImagePoint> centers_of_pixel, CTArray< RGB_TRIPLE > centers_of_color,
	long image_height, long image_width)//当前的像素点，颜色，中心点
{
	long dimension = centers_of_pixel.GetDimension();//计算聚类中心点的个数

	CTArray< double > array_of_distances(dimension);

	double current_distance;
	int current_index;

	for (int index = 0; index < dimension; index++)
	{
		array_of_distances[index] = Distance_of_location_and_color(current_pixel, current_color,
			centers_of_pixel[index], centers_of_color[index],
			image_height, image_width);//算中心点的距离
		if (index == 0)//从零开始排序
		{
			current_distance = array_of_distances[index];
			current_index = index;
		}
		else//比较下一个距离
		{
			if (array_of_distances[index] < current_distance)
			{
				current_distance = array_of_distances[index];
				current_index = index;
			}
		}
	}

	return current_index;
}

void Update_centers_of_clusters(const CTMatrix< RGB_TRIPLE >& color_image, const CTMatrix< int >& cluster_result,
	int number_of_clusters,
	CTArray< CImagePoint >& centers_of_pixel, CTArray< RGB_TRIPLE >& centers_of_color)
{
	long image_height = color_image.Get_height();
	long image_width = color_image.Get_width();

	CTArray< int > scale_of_clusters(number_of_clusters);
	CTMatrix< double > sum_of_clusters(number_of_clusters, 5);

	for (int index = 0; index < number_of_clusters; index++)
	{
		scale_of_clusters[index] = 0;

		for (int sub_index = 0; sub_index < 5; sub_index++)
		{
			sum_of_clusters[index][sub_index] = 0;
		}
	}

	for (int row = 0; row < image_height; row++)
		for (int column = 0; column < image_width; column++)
		{
			int current_index = cluster_result[row][column];

			scale_of_clusters[current_index]++;
			sum_of_clusters[current_index][0] += row;
			sum_of_clusters[current_index][1] += column;
			sum_of_clusters[current_index][2] += color_image[row][column].m_Red;
			sum_of_clusters[current_index][3] += color_image[row][column].m_Green;
			sum_of_clusters[current_index][4] += color_image[row][column].m_Blue;
		}

	for (int index = 0; index < number_of_clusters; index++)
		if (scale_of_clusters[index] != 0)
		{
			centers_of_pixel[index].m_row = long(sum_of_clusters[index][0] / scale_of_clusters[index]);
			centers_of_pixel[index].m_column = long(sum_of_clusters[index][1] / scale_of_clusters[index]);
			centers_of_color[index].m_Red = BYTE(sum_of_clusters[index][2] / scale_of_clusters[index]);
			centers_of_color[index].m_Green = BYTE(sum_of_clusters[index][3] / scale_of_clusters[index]);
			centers_of_color[index].m_Blue = BYTE(sum_of_clusters[index][4] / scale_of_clusters[index]);
		}

	return;
}

bool Are_centers_same(const CTArray< CImagePoint>& centers_of_pixel, const CTArray< RGB_TRIPLE >& centers_of_color,
	const CTArray< CImagePoint>& copies_of_pixel, const CTArray< RGB_TRIPLE >& copies_of_color)
{
	bool is_same = true;

	long dimension = centers_of_pixel.GetDimension();

	for (int index = 0; index < dimension; index++)
	{
		if (centers_of_pixel[index] != copies_of_pixel[index]
			|| centers_of_color[index].m_Red != copies_of_color[index].m_Red
			|| centers_of_color[index].m_Green != copies_of_color[index].m_Green
			|| centers_of_color[index].m_Blue != copies_of_color[index].m_Blue)
		{
			is_same = false;
			break;
		}
	}

	return is_same;
}

// [ ********** ] ........................................................
// [ K 均值聚类 ] ........................................................
// [ ********** ] ........................................................
CTMatrix< int > CImageProcess::K_means_clustering(const CTMatrix< RGB_TRIPLE >& color_image, int number_of_clusters)
{
	long image_height = color_image.Get_height();
	long image_width = color_image.Get_width();

	CTMatrix< int > cluster_result(image_height, image_width);

	CTArray< CImagePoint> centers_of_pixel(number_of_clusters);
	CTArray< RGB_TRIPLE > centers_of_color(number_of_clusters);

	for (int index = 0; index < number_of_clusters; index++)
	{
		int row = image_height * (index + 1) / (number_of_clusters + 1);
		int column = image_width * (index + 1) / (number_of_clusters + 1);

		centers_of_pixel[index] = CImagePoint(row, column);
		centers_of_color[index] = color_image[row][column];
	}

	CTArray< CImagePoint> copies_of_pixel;
	CTArray< RGB_TRIPLE > copies_of_color;

	long iteration = 0;

	do
	{
		iteration++;

		copies_of_pixel = centers_of_pixel;
		copies_of_color = centers_of_color;

		for (int row = 0; row < image_height; row++)
			for (int column = 0; column < image_width; column++)
			{
				cluster_result[row][column] = Update_index_into_clusters(CImagePoint(row, column), color_image[row][column],
					centers_of_pixel, centers_of_color, image_height, image_width);
			}

		Update_centers_of_clusters(color_image, cluster_result, number_of_clusters, centers_of_pixel, centers_of_color);

	} while (!Are_centers_same(centers_of_pixel, centers_of_color, copies_of_pixel, copies_of_color) && iteration < 1000);

	return cluster_result;
}

// [ ******** ] ..........................................................
// [ 冒泡排序 ] ..........................................................
// [ ******** ] ..........................................................
CTArray< BYTE > CImageProcess::Sort_with_bubbling(CTArray< BYTE > array_of_elements)
{
	long dimension = array_of_elements.GetDimension();

	for (int index = 0; index < dimension; index++)
	{
		for (int sub_index = dimension - 1; sub_index > index; sub_index--)
		{
			if (array_of_elements[index] < array_of_elements[sub_index])
			{
				BYTE temp = array_of_elements[index];
				array_of_elements[index] = array_of_elements[sub_index];
				array_of_elements[sub_index] = temp;
			}
		}
	}

	return array_of_elements;
}

// [ ******** ] ..............................................................
// [ 中值滤波 ] ..............................................................
// [ ******** ] ..............................................................
CTMatrix< BYTE > CImageProcess::Median_filter(const CTMatrix< BYTE >& gray_image, long delta)
{
	long image_height = gray_image.Get_height();
	long image_width = gray_image.Get_width();

	CTMatrix< BYTE > filter_result(image_height, image_width);

	for (int row = 0; row < image_height; row++)
		for (int column = 0; column < image_width; column++)
		{
			long number = 0;

			for (int sub_row = -delta; sub_row <= delta; sub_row++)
				for (int sub_col = -delta; sub_col <= delta; sub_col++)
					if (gray_image.Is_point_valid(CImagePoint(row + sub_row, column + sub_col)))
					{
						number++;
					}

			CTArray< BYTE > array_of_elements(number);

			int index = -1;

			for (int sub_row = -delta; sub_row <= delta; sub_row++)
				for (int sub_col = -delta; sub_col <= delta; sub_col++)
					if (gray_image.Is_point_valid(CImagePoint(row + sub_row, column + sub_col)))
					{
						array_of_elements[++index] = gray_image[row + sub_row][column + sub_col];
					}

			ASSERT(index == array_of_elements.GetUpperBound());

			array_of_elements = Sort_with_bubbling(array_of_elements);

			filter_result[row][column] = array_of_elements[array_of_elements.GetDimension() / 2];
		}

	return filter_result;
}