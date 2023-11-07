#include "pch.h"
#include "ImageProcess.h"
#include "math.h"
#include <stdlib.h>
#include <cstdlib>

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