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
// [ ͼ��ƽ�� ] ..........................................................
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
// [ ͼ���� ] ..........................................................
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
// [ ͼ�����任 ] ..........................................................
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
// [ ͼ��ת�� ] ..........................................................
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
// [ ͼ����ת ] ..........................................................
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
// [ ͼ������ ] ..........................................................
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
// [ Robert ��Ե���� ] ...................................................
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
// [ ��ά������� ] ......................................................
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
// [ Sobel ��Ե���ӣ��ݶ�ǿ�ȣ� ] ........................................
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
// [ Sobel ��Ե���ӣ���ֱ���� ] ........................................
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
// [ Sobel ��Ե���ӣ�ˮƽ���� ] ........................................
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

struct PixlNode // ��¼Canny������˫��ֵ���е�һ��ɨ�費�ܴ����ṹ
{
	int h;
	int w;
	int gradvalue;
	PixlNode* next;
};

// [ **************** ] ..................................................
// [ ��˹�˲�������� ] ..................................................
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
// [ ��Canny�����д��ڸߵ���ֵ֮������ص� ] .............................
// [ ************************************* ] .............................
void CannyPixlProc(struct PixlNode** ph, struct PixlNode** pt, CTMatrix<BYTE>& tm, int thi, int tlo, int& glcounter)
{
	struct PixlNode* p, * q;
	int    h = 0, w = 0;
	bool   flag1 = false, flag2 = false;

	// ����ߵ���ֵ֮������ص�
	p = q = *ph;
	while (q)
	{
		h = q->h;
		w = q->w;

		if (tm[h - 1][w - 1] > thi || tm[h - 1][w] > thi ||
			tm[h - 1][w + 1] > thi || tm[h][w - 1] > thi ||
			tm[h][w + 1] > thi || tm[h + 1][w - 1] > thi ||
			tm[h + 1][w] > thi || tm[h + 1][w + 1] > thi)
		{ // ����ڽӵ��Ǳ�Ե����˵�Ҳ��
			tm[h][w] = 255;
			glcounter--;
			flag1 = true;
		}
		else if (tm[h - 1][w - 1] < tlo && tm[h - 1][w] < tlo &&
			tm[h - 1][w + 1] < tlo && tm[h][w - 1] < tlo &&
			tm[h][w + 1] < tlo && tm[h + 1][w - 1] < tlo &&
			tm[h + 1][w] < tlo && tm[h + 1][w + 1] < tlo)
		{ // �ڽӵ㶼���Ǳ�Ե����˵�Ҳ����
			tm[h][w] = 0;
			glcounter--;
			flag2 = true;
		}
		else
		{ // ��Ȼ��ȷ��
			p = q;
			q = q->next;
		}
		if (flag1 == true || flag2 == true) // if0
		{ // ����һ���������ɾ��һ��
			flag1 = flag2 = false;
			if (q == *ph)
			{ // ɾ��ͷ��
				*ph = (*ph)->next;
				delete q;
				p = q = *ph;
			}
			else if (q == *pt)
			{ // ɾ��β��
				*pt = p;
				(*pt)->next = NULL;
				delete q;
				p = q = NULL;
			}
			else
			{ // ɾ���м�
				p->next = q->next;
				delete q;
				q = p->next;
			}
		} // end if0
	}// end while 
}

// [ ************** ] ....................................................
// [ Canny ��Ե���� ] ....................................................
// [ ************** ] ....................................................
CTMatrix< BYTE > CImageProcess::Canny_edge_operator(const CTMatrix< BYTE >& gray_image)
{
	CTMatrix<BYTE> temp_matrix, tm; // �Ҷ�ֵ����
	CTMatrix<BYTE> di;              // ��Ӧ�ݶȷ����
	int temp, tparr[2];             // ��ʱ����
	int his[256];                   // �Ҷ�ֱ��ͼ
	int width;                      // ͼ����
	int height;                     // ͼ��߶�
	int h = 0, w = 0;               // �����±� h���� w����
	int i = 0;                      // �����±���ʱ���
	int max = 0, min = 0;

	// �õ����ӹ�ͼ����Ϣ ͼ���� ͼ��߶�	
	temp_matrix = tm = gray_image;
	width = gray_image.Get_width();
	height = gray_image.Get_height();

	// �����漸������ʵ�� Canny ����
	//
	// step 1. ��˹�˲���������
	GausFilterOp(height, width, temp_matrix);

	// step 2. Prewittģ�����ݶȴ�С���ݶȷ����
	tm = di = temp_matrix;
	for (h = 1; h < height - 1; h++)
	{
		for (w = 1; w < width - 1; w++)
		{
			// ��ֱ����
			tparr[0] = -tm[h - 1][w - 1] + tm[h - 1][w + 1]
				- tm[h][w - 1] + tm[h][w + 1]
				- tm[h + 1][w - 1] + tm[h + 1][w + 1];

			// ˮƽ����
			tparr[1] = -tm[h - 1][w - 1] - tm[h - 1][w] - tm[h - 1][w + 1]
				+ tm[h + 1][w - 1] + tm[h + 1][w] + tm[h + 1][w + 1];

			// a. �����ݶ�ֵ
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

			// b. ���㷽���
			temp = (int)(atan((double)tparr[0] / (double)tparr[1]));

			if (temp < 0)
			{
				temp += 180;
			}

			di[h][w] = (BYTE)(temp * 180 / 3.14);
		}
	}

	// step 3. ������Ƿ��� 0-22.5��157.5-180 ��0�ȣ� 22.5-67.5 ��45�ȣ�67.5-112.5��90�ȣ�112.5-157.5��135��
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

	// step 4. ��������� 
	for (h = 1; h < height - 1; h++)
	{
		for (w = 1; w < width - 1; w++)
		{
			bool flag = true;

			if (di[h - 1][w] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w]) flag = false; // ��				
			if (di[h][w - 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h][w - 1]) flag = false; // ǰ
			if (di[h][w + 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h][w + 1]) flag = false; // ��				
			if (di[h + 1][w] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w]) flag = false; // ��
			if (di[h - 1][w - 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w - 1]) flag = false; // ����
			if (di[h - 1][w + 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w + 1]) flag = false; // ����
			if (di[h + 1][w - 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h + 1][w - 1]) flag = false; // ����
			if (di[h + 1][w + 1] == di[h][w] && temp_matrix[h][w] < temp_matrix[h - 1][w + 1]) flag = false; // ����			

			if (flag == false)
			{
				temp_matrix[h][w] = 0;
			}
		}
	}

	// step 5. ˫��ֵ���жϱ�Ե�� 
	//
	// step 5.1 ѡȡ��ֵ tHigh tLow
	//
	int tHigh, tLow; // ����Ѱ�ұ�Ե�����ֵ
	// ֱ��ͼ��ʼ�� 
	for (i = 0; i < 256; i++)
	{
		his[i] = 0;
	}

	// �õ�ֱ��ͼ 
	for (h = 1; h < height - 1; h++)
	{
		for (w = 1; w < width - 1; w++)
		{
			his[temp_matrix[h][w]]++;
		}
	}

	// �õ��Ҷ�ֵ����
	h = 255;
	while (h--)
	{
		if (his[h] != 0)
		{
			max = h;
			break;
		}
	}

	// �õ��Ҷ�ֵ��С�� 
	h = 0;
	while (h++)
	{
		if (his[h] != 0)
		{
			min = h;
			break;
		}
	}

	// �õ�˫��ֵ ����ֵȡ 0.25 ��������ֵȡ����ֵ�� 0.4
	tHigh = (int)(min + 0.25 * (max - min));
	tLow = (int)(0.4 * tHigh);

	// step 5.2 ����˫��ֵ�ұ�Ե��
	long int sum = 0;
	struct PixlNode* pHead, * pTail;

	pHead = pTail = NULL;
	int glcounter = (height - 4) * (width - 4);

	// step 5.2.1 �״δ��� ȷ��������� ��¼��ȷ����
	for (h = 2; h < height - 2; h++) // for1
	{
		for (w = 2; w < width - 2; w++) // for2
		{

			if (temp_matrix[h][w] >= tHigh)
			{ // ���ڸ���ֵ�϶��Ǳ�Ե��
				temp_matrix[h][w] = 255;
				glcounter--;
			}
			else if (temp_matrix[h][w] < tLow)
			{ // С�ڵ���ֵ�϶����Ǳ�Ե��
				temp_matrix[h][w] = 0;
				glcounter--;
			}
			else // else1
			{ // ������֮���ͨ���ж��ڽ�8���ص�ȷ��
				if (temp_matrix[h - 1][w - 1] > tHigh || temp_matrix[h - 1][w] > tHigh ||
					temp_matrix[h - 1][w + 1] > tHigh || temp_matrix[h][w - 1] > tHigh ||
					temp_matrix[h][w + 1] > tHigh || temp_matrix[h + 1][w - 1] > tHigh ||
					temp_matrix[h + 1][w] > tHigh || temp_matrix[h + 1][w + 1] > tHigh)
				{ // ����ڽӵ��Ǳ�Ե����˵�Ҳ��
					temp_matrix[h][w] = 255;
					glcounter--;
				}
				else if (temp_matrix[h - 1][w - 1] < tLow && temp_matrix[h - 1][w] < tLow &&
					temp_matrix[h - 1][w + 1] < tLow && temp_matrix[h][w - 1] < tLow &&
					temp_matrix[h][w + 1] < tLow && temp_matrix[h + 1][w - 1] < tLow &&
					temp_matrix[h + 1][w] < tLow && temp_matrix[h + 1][w + 1] < tLow)
				{ // �ڽӵ㶼���Ǳ�Ե����˵�Ҳ����
					temp_matrix[h][w] = 0;
					glcounter--;
				}
				else // else2
				{ // �����Ե��ĻҶ�ֵ�ڸߵ���ֵ֮�����¼����
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


	// step 5.2.2 ����δȷ����
	int glcounterpre = 0;
	while (glcounter != glcounterpre)
	{
		glcounterpre = glcounter;
		CannyPixlProc(&pHead, &pTail, temp_matrix, tHigh, tLow, glcounter);
	}

	// �������ղ��ܴ������������Ϊ�Ǻͱ�Ե�޹ص� ��Ϊ 0
	struct PixlNode* q = pHead;
	while (q)
	{
		h = q->h;
		w = q->w;
		temp_matrix[h][w] = 0;
		q = q->next;
	}

	// �ͷŶ�̬������ڴ�
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
// [ Prewitt ��Ե���� ] ..................................................
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
// [ �������� ] ..........................................................
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
// [ ��ˮ���㷨 ] ........................................................
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
	//���ó�ʼˮλ����û������ֵ�����ص㣬�γɻ�ˮ�ӡ�
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
	//���ڱ�ǵ�ǰ���ص�������һ����ˮ�ӣ�0���������κλ�ˮ�� 1-n�ǻ�ˮ�ӱ��
	CTMatrix< int > LabelImage(image_height, image_width);//����

	for (int i = 0; i < image_height; i++)
	{
		for (int j = 0; j < image_width; j++)
			LabelImage[i][j] = 0;
	}

	int Num = 0;//��ˮ�ӱ��
	int i, j;
	//��¼����ˮ�Ӳ�ͬ�Ҷ�����ֵ[0-255]���ص�[n]�ĸ���
	vector<int*> SeedCounts;//���еĳ���
	queue<POINT> quetem;//���ص�(������)�ṹ��ʱ����
	//��¼����ˮ�Ӳ�ͬ�Ҷ�����ֵ[0-255]���ص�Ķ���
	vector<queue<POINT>*> vque;//������ͷ������ ��ɫ

	int* array;
	queue<POINT>* pque;
	POINT temp;

	int m, n, k = 0;
	BOOL up, down, right, left, upleft, upright, downleft, downright;//8 directions...

	for (i = 0; i < image_height; i++)
	{
		for (j = 0; j < image_width; j++)
		{
			if (SeedImage[i][j] == 1)//��һ��ˮ��
			{
				Num++;//��ǰ��ˮ�ӱ��

				array = new int[256];
				ZeroMemory(array, 256 * sizeof(int));

				SeedCounts.push_back(array);//��ǰ��ˮ��ͳ����Ϣ�ĳ�ʼ��

				pque = new queue<POINT>[256];

				vque.push_back(pque);//��ǰ��ˮ�ӵĶ��нڵ��ʼ��

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
					quetem.pop();//��������

					if (m > 0)
					{
						if (SeedImage[m - 1][n] == 1)//��ǰ�ڵ���ϱߣ���һ������ָ��ֱ����
						{
							temp.x = m - 1;
							temp.y = n;
							quetem.push(temp);//�����

							LabelImage[m - 1][n] = Num;//���Ϊ��һ����
							SeedImage[m - 1][n] = 127;//��Ǳ�ʹ�ñ�����ѭ��
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
						upleft || downleft || upright || downright)//�õ㲻�ǻ�ˮ�ӱ��ϵĵ�
					{
						temp.x = m;
						temp.y = n;
						vque[Num - 1][OriginalImage[m][n]].push(temp);//��ǰ��ˮ�ӵ�����ֵ
						SeedCounts[Num - 1][OriginalImage[m][n]]++;//��ǰ��ˮ�ӵĻҶ�ֵ����
					}
				}
			}
		}
	}

	bool actives;
	int WaterLevel;

	for (WaterLevel = 0; WaterLevel < 180; WaterLevel++) //��ˮ��180
	{
		actives = true;//�������е�ˮ������һ��
		while (actives)
		{
			actives = false;

			for (i = 0; i < Num; i++)//��ˮ���±�
			{
				if (!vque[i][WaterLevel].empty())//�鿴��ֵ�Ƿ�Ϊ�գ��Ƿ��������
				{
					actives = true;
					while (SeedCounts[i][WaterLevel] > 0)
					{
						SeedCounts[i][WaterLevel]--;
						temp = vque[i][WaterLevel].front();//�õ��ڵ�

						vque[i][WaterLevel].pop();//�����ڵ�
						m = temp.x;
						n = temp.y;
						if (m > 0)
						{
							if (!LabelImage[m - 1][n])
							{
								temp.x = m - 1;
								temp.y = n;
								LabelImage[m - 1][n] = i + 1;//��¼��ǰ�Ŀ�

								if (OriginalImage[m - 1][n] <= WaterLevel)
								{
									vque[i][WaterLevel].push(temp);
								}
								else//ˮ�����ˣ��õ���Ȼ�ܸߵ�Ҳ��ÿӡ�p115
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

	while (!vque.empty())//�ͷ����еĵ�
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
{	//���ص�һ
	double first_1 = double(pixel_1.m_row) / double(image_height);
	double second_1 = double(pixel_1.m_column) / double(image_width);
	double third_1 = double(color_1.m_Red) / 255.0;	//��ɫͨ����һ��0-1����
	double fourth_1 = double(color_1.m_Green) / 255.0;
	double fifth_1 = double(color_1.m_Blue) / 255.0;
	//���ص��
	double first_2 = double(pixel_2.m_row) / double(image_height);
	double second_2 = double(pixel_2.m_column) / double(image_width);
	double third_2 = double(color_2.m_Red) / 255.0;
	double fourth_2 = double(color_2.m_Green) / 255.0;
	double fifth_2 = double(color_2.m_Blue) / 255.0;

	return sqrt((first_1 - first_2) * (first_1 - first_2)
		+ (second_1 - second_2) * (second_1 - second_2)
		+ (third_1 - third_2) * (third_1 - third_2)
		+ (fourth_1 - fourth_2) * (fourth_1 - fourth_2)
		+ (fifth_1 - fifth_2) * (fifth_1 - fifth_2));//����
}

int Update_index_into_clusters(CImagePoint current_pixel, RGB_TRIPLE current_color,
	CTArray< CImagePoint> centers_of_pixel, CTArray< RGB_TRIPLE > centers_of_color,
	long image_height, long image_width)//��ǰ�����ص㣬��ɫ�����ĵ�
{
	long dimension = centers_of_pixel.GetDimension();//����������ĵ�ĸ���

	CTArray< double > array_of_distances(dimension);

	double current_distance;
	int current_index;

	for (int index = 0; index < dimension; index++)
	{
		array_of_distances[index] = Distance_of_location_and_color(current_pixel, current_color,
			centers_of_pixel[index], centers_of_color[index],
			image_height, image_width);//�����ĵ�ľ���
		if (index == 0)//���㿪ʼ����
		{
			current_distance = array_of_distances[index];
			current_index = index;
		}
		else//�Ƚ���һ������
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
// [ K ��ֵ���� ] ........................................................
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
// [ ð������ ] ..........................................................
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

#ifndef ERROR
#define ERROR -1
#define NO_ERROR 0
#endif

#define FFT_FORWARD	0
#define FFT_INVERSE 1

#define PI      3.1415926535897932
#define TWOPI   6.2831853071795865 /* 2.0 * PI */
#define HALFPI  1.5707963267948966 /* PI / 2.0 */
#define PI8 	0.392699081698724 /* PI / 8.0 */
#define RT2 	1.4142135623731  /* sqrt(2.0) */
#define IRT2 	0.707106781186548  /* 1.0/sqrt(2.0) */

/*
*	These somewhat awkward macros move data between the input/output array
*  and stageBuff, while also performing any conversions float<->double.
*/

#define LoadRow(buffer,row,cols) if (1){\
	register int j,k;\
	for (j = row*cols, k = 0 ; k < cols ; j++, k++){\
	stageBuff[k].m_re = buffer[j].m_re;\
	stageBuff[k].m_im = buffer[j].m_im;\
	}\
} else {}

#define StoreRow(buffer,row,cols) if (1){\
	register int j,k;\
	for (j = row*cols, k = 0 ; k < cols ; j++, k++){\
	buffer[j].m_re = stageBuff[k].m_re;\
	buffer[j].m_im = stageBuff[k].m_im;\
	}\
} else {}

#define LoadCol(buffer,col,rows,cols) if (1){\
	register int j,k;\
	for (j = i,k = 0 ; k < rows ; j+=cols, k++){\
	stageBuff[k].m_re = buffer[j].m_re;\
	stageBuff[k].m_im = buffer[j].m_im;\
	}\
} else {}

#define StoreCol(buffer,col,rows,cols) if (1){\
	register int j,k;\
	for (j = i,k = 0 ; k < rows ; j+=cols, k++){\
	buffer[j].m_re = stageBuff[k].m_re;\
	buffer[j].m_im = stageBuff[k].m_im;\
	}\
} else {}


/* Do something useful with an error message */
#define handle_error(msg) fprintf(stderr,msg)

complex* stageBuff;  /* buffer to hold a row or column at a time */
complex* bigBuffd;   /* a pointer to a double input array */


/* Allocate space for stageBuff */
int allocateBuffer(int size)
{
	stageBuff = (complex*)malloc(size * sizeof(complex));
	if (stageBuff == (complex*)NULL)
	{
		handle_error("Insufficient storage for fft buffers");
		return(ERROR);
	}
	else return(NO_ERROR);
}


/* Free space for stageBuff */
void freeBuffer(void)
{
	free((char*)stageBuff);
}


/* See if exactly one bit is set in integer argument */
int power_of_2(int n)
{
	int bits = 0;
	while (n) {
		bits += n & 1;
		n >>= 1;
	}
	return(bits == 1);
}

/* Get binary log of integer argument - exact if n is a power of 2 */
int fastlog2(int n)
{
	int log = -1;
	while (n) {
		log++;
		n >>= 1;
	}
	return(log);
}

/* Radix-2 iteration subroutine */
void R2TX(int nthpo, complex* c0, complex* c1)
{
	int k, kk;
	double* cr0, * ci0, * cr1, * ci1, r1, fi1;

	cr0 = &(c0[0].m_re);
	ci0 = &(c0[0].m_im);
	cr1 = &(c1[0].m_re);
	ci1 = &(c1[0].m_im);

	for (k = 0; k < nthpo; k += 2)
	{
		kk = k * 2;

		r1 = cr0[kk] + cr1[kk];
		cr1[kk] = cr0[kk] - cr1[kk];
		cr0[kk] = r1;
		fi1 = ci0[kk] + ci1[kk];
		ci1[kk] = ci0[kk] - ci1[kk];
		ci0[kk] = fi1;
	}
}


/* Radix-4 iteration subroutine */
void R4TX(int nthpo, complex* c0, complex* c1,
	complex* c2, complex* c3)
{
	int k, kk;
	double* cr0, * ci0, * cr1, * ci1, * cr2, * ci2, * cr3, * ci3;
	double r1, r2, r3, r4, i1, i2, i3, i4;

	cr0 = &(c0[0].m_re);
	cr1 = &(c1[0].m_re);
	cr2 = &(c2[0].m_re);
	cr3 = &(c3[0].m_re);
	ci0 = &(c0[0].m_im);
	ci1 = &(c1[0].m_im);
	ci2 = &(c2[0].m_im);
	ci3 = &(c3[0].m_im);

	for (k = 1; k <= nthpo; k += 4)
	{
		kk = (k - 1) * 2;  /* real and imag parts alternate */

		r1 = cr0[kk] + cr2[kk];
		r2 = cr0[kk] - cr2[kk];
		r3 = cr1[kk] + cr3[kk];
		r4 = cr1[kk] - cr3[kk];
		i1 = ci0[kk] + ci2[kk];
		i2 = ci0[kk] - ci2[kk];
		i3 = ci1[kk] + ci3[kk];
		i4 = ci1[kk] - ci3[kk];
		cr0[kk] = r1 + r3;
		ci0[kk] = i1 + i3;
		cr1[kk] = r1 - r3;
		ci1[kk] = i1 - i3;
		cr2[kk] = r2 - i4;
		ci2[kk] = i2 + r4;
		cr3[kk] = r2 + i4;
		ci3[kk] = i2 - r4;
	}
}

/* Radix-8 iteration subroutine */
void R8TX(int nxtlt, int nthpo, int length,
	complex* cc0, complex* cc1, complex* cc2, complex* cc3,
	complex* cc4, complex* cc5, complex* cc6, complex* cc7)
{
	int j, k, kk;
	double scale, arg, tr, ti;
	double c1, c2, c3, c4, c5, c6, c7;
	double s1, s2, s3, s4, s5, s6, s7;
	double ar0, ar1, ar2, ar3, ar4, ar5, ar6, ar7;
	double ai0, ai1, ai2, ai3, ai4, ai5, ai6, ai7;
	double br0, br1, br2, br3, br4, br5, br6, br7;
	double bi0, bi1, bi2, bi3, bi4, bi5, bi6, bi7;

	double* cr0, * cr1, * cr2, * cr3, * cr4, * cr5, * cr6, * cr7;
	double* ci0, * ci1, * ci2, * ci3, * ci4, * ci5, * ci6, * ci7;

	cr0 = &(cc0[0].m_re);
	cr1 = &(cc1[0].m_re);
	cr2 = &(cc2[0].m_re);
	cr3 = &(cc3[0].m_re);
	cr4 = &(cc4[0].m_re);
	cr5 = &(cc5[0].m_re);
	cr6 = &(cc6[0].m_re);
	cr7 = &(cc7[0].m_re);

	ci0 = &(cc0[0].m_im);
	ci1 = &(cc1[0].m_im);
	ci2 = &(cc2[0].m_im);
	ci3 = &(cc3[0].m_im);
	ci4 = &(cc4[0].m_im);
	ci5 = &(cc5[0].m_im);
	ci6 = &(cc6[0].m_im);
	ci7 = &(cc7[0].m_im);

	scale = TWOPI / length;

	for (j = 1; j <= nxtlt; j++)
	{
		arg = (j - 1) * scale;
		c1 = cos(arg);
		s1 = sin(arg);
		c2 = c1 * c1 - s1 * s1;
		s2 = c1 * s1 + c1 * s1;
		c3 = c1 * c2 - s1 * s2;
		s3 = c2 * s1 + s2 * c1;
		c4 = c2 * c2 - s2 * s2;
		s4 = c2 * s2 + c2 * s2;
		c5 = c2 * c3 - s2 * s3;
		s5 = c3 * s2 + s3 * c2;
		c6 = c3 * c3 - s3 * s3;
		s6 = c3 * s3 + c3 * s3;
		c7 = c3 * c4 - s3 * s4;
		s7 = c4 * s3 + s4 * c3;

		for (k = j; k <= nthpo; k += length)
		{
			kk = (k - 1) * 2; /* index by twos; m_re & m_im alternate */

			ar0 = cr0[kk] + cr4[kk];
			ar1 = cr1[kk] + cr5[kk];
			ar2 = cr2[kk] + cr6[kk];
			ar3 = cr3[kk] + cr7[kk];
			ar4 = cr0[kk] - cr4[kk];
			ar5 = cr1[kk] - cr5[kk];
			ar6 = cr2[kk] - cr6[kk];
			ar7 = cr3[kk] - cr7[kk];
			ai0 = ci0[kk] + ci4[kk];
			ai1 = ci1[kk] + ci5[kk];
			ai2 = ci2[kk] + ci6[kk];
			ai3 = ci3[kk] + ci7[kk];
			ai4 = ci0[kk] - ci4[kk];
			ai5 = ci1[kk] - ci5[kk];
			ai6 = ci2[kk] - ci6[kk];
			ai7 = ci3[kk] - ci7[kk];
			br0 = ar0 + ar2;
			br1 = ar1 + ar3;
			br2 = ar0 - ar2;
			br3 = ar1 - ar3;
			br4 = ar4 - ai6;
			br5 = ar5 - ai7;
			br6 = ar4 + ai6;
			br7 = ar5 + ai7;
			bi0 = ai0 + ai2;
			bi1 = ai1 + ai3;
			bi2 = ai0 - ai2;
			bi3 = ai1 - ai3;
			bi4 = ai4 + ar6;
			bi5 = ai5 + ar7;
			bi6 = ai4 - ar6;
			bi7 = ai5 - ar7;
			cr0[kk] = br0 + br1;
			ci0[kk] = bi0 + bi1;
			if (j > 1)
			{
				cr1[kk] = c4 * (br0 - br1) - s4 * (bi0 - bi1);
				cr2[kk] = c2 * (br2 - bi3) - s2 * (bi2 + br3);
				cr3[kk] = c6 * (br2 + bi3) - s6 * (bi2 - br3);
				ci1[kk] = c4 * (bi0 - bi1) + s4 * (br0 - br1);
				ci2[kk] = c2 * (bi2 + br3) + s2 * (br2 - bi3);
				ci3[kk] = c6 * (bi2 - br3) + s6 * (br2 + bi3);
				tr = IRT2 * (br5 - bi5);
				ti = IRT2 * (br5 + bi5);
				cr4[kk] = c1 * (br4 + tr) - s1 * (bi4 + ti);
				ci4[kk] = c1 * (bi4 + ti) + s1 * (br4 + tr);
				cr5[kk] = c5 * (br4 - tr) - s5 * (bi4 - ti);
				ci5[kk] = c5 * (bi4 - ti) + s5 * (br4 - tr);
				tr = -IRT2 * (br7 + bi7);
				ti = IRT2 * (br7 - bi7);
				cr6[kk] = c3 * (br6 + tr) - s3 * (bi6 + ti);
				ci6[kk] = c3 * (bi6 + ti) + s3 * (br6 + tr);
				cr7[kk] = c7 * (br6 - tr) - s7 * (bi6 - ti);
				ci7[kk] = c7 * (bi6 - ti) + s7 * (br6 - tr);
			}
			else
			{
				cr1[kk] = br0 - br1;
				cr2[kk] = br2 - bi3;
				cr3[kk] = br2 + bi3;
				ci1[kk] = bi0 - bi1;
				ci2[kk] = bi2 + br3;
				ci3[kk] = bi2 - br3;
				tr = IRT2 * (br5 - bi5);
				ti = IRT2 * (br5 + bi5);
				cr4[kk] = br4 + tr;
				ci4[kk] = bi4 + ti;
				cr5[kk] = br4 - tr;
				ci5[kk] = bi4 - ti;
				tr = -IRT2 * (br7 + bi7);
				ti = IRT2 * (br7 - bi7);
				cr6[kk] = br6 + tr;
				ci6[kk] = bi6 + ti;
				cr7[kk] = br6 - tr;
				ci7[kk] = bi6 - ti;
			}
		}
	}
}

/*
* FFT842 (Name kept from the original Fortran version)
* This routine replaces the input complex vector by its
* finite discrete complex fourier transform if in==FFT_FORWARD.
* It replaces the input complex vector by its finite discrete
* complex inverse fourier transform if in==FFT_INVERSE.
*
* The implementation is a radix-2 FFT, but with faster shortcuts for
* radix-4 and radix-8. It performs as many radix-8 iterations as
* possible, and then finishes with a radix-2 or -4 iteration if needed.
*/
void FFT842(int direction, int n, complex* b)
/* direction: FFT_FORWARD or FFT_INVERSE
* n:  length of vector
* *b: input vector */
{
	double fn, r, fi;

	int L[16], L1, L2, L3, L4, L5, L6, L7, L8, L9, L10, L11, L12, L13, L14, L15;
	/*  int j0,j1,j2,j3,j4,j5,j6,j7,j8,j9,j10,j11,j12,j13,j14;*/
	int j1, j2, j3, j4, j5, j6, j7, j8, j9, j10, j11, j12, j13, j14;
	int i, j, ij, ji, ij1, ji1;
	/*  int nn, n2pow, n8pow, nthpo, ipass, nxtlt, length;*/
	int n2pow, n8pow, nthpo, ipass, nxtlt, length;

	n2pow = fastlog2(n);
	nthpo = n;
	fn = 1.0 / (double)nthpo; /* Scaling factor for inverse transform */

	if (direction == FFT_FORWARD)
		/* Conjugate the input */
		for (i = 0; i < n; i++) {
			b[i].m_im = -b[i].m_im;
		}

	if (direction == FFT_INVERSE)
		/* Scramble the inputs */
		for (i = 0, j = n / 2; j < n; i++, j++)
		{
			r = b[j].m_re; fi = b[j].m_im;
			b[j].m_re = b[i].m_re; b[j].m_im = b[i].m_im;
			b[i].m_re = r; b[i].m_im = fi;
		}

	n8pow = n2pow / 3;

	if (n8pow)
	{
		/* Radix 8 iterations */
		for (ipass = 1; ipass <= n8pow; ipass++)
		{
			nxtlt = 0x1 << (n2pow - 3 * ipass);
			length = 8 * nxtlt;
			R8TX(nxtlt, nthpo, length,
				b, b + nxtlt, b + 2 * nxtlt, b + 3 * nxtlt,
				b + 4 * nxtlt, b + 5 * nxtlt, b + 6 * nxtlt, b + 7 * nxtlt);
		}
	}

	if (n2pow % 3 == 1)
	{
		/* A final radix 2 iteration is needed */
		R2TX(nthpo, b, b + 1);
	}

	if (n2pow % 3 == 2)
	{
		/* A final radix 4 iteration is needed */
		R4TX(nthpo, b, b + 1, b + 2, b + 3);
	}

	for (j = 1; j <= 15; j++)
	{
		L[j] = 1;
		if (j - n2pow <= 0) L[j] = 0x1 << (n2pow + 1 - j);
	}

	L15 = L[1]; L14 = L[2]; L13 = L[3]; L12 = L[4]; L11 = L[5]; L10 = L[6]; L9 = L[7];
	L8 = L[8]; L7 = L[9]; L6 = L[10]; L5 = L[11]; L4 = L[12]; L3 = L[13]; L2 = L[14]; L1 = L[15];

	ij = 1;

	for (j1 = 1; j1 <= L1; j1++)
		for (j2 = j1; j2 <= L2; j2 += L1)
			for (j3 = j2; j3 <= L3; j3 += L2)
				for (j4 = j3; j4 <= L4; j4 += L3)
					for (j5 = j4; j5 <= L5; j5 += L4)
						for (j6 = j5; j6 <= L6; j6 += L5)
							for (j7 = j6; j7 <= L7; j7 += L6)
								for (j8 = j7; j8 <= L8; j8 += L7)
									for (j9 = j8; j9 <= L9; j9 += L8)
										for (j10 = j9; j10 <= L10; j10 += L9)
											for (j11 = j10; j11 <= L11; j11 += L10)
												for (j12 = j11; j12 <= L12; j12 += L11)
													for (j13 = j12; j13 <= L13; j13 += L12)
														for (j14 = j13; j14 <= L14; j14 += L13)
															for (ji = j14; ji <= L15; ji += L14)
															{
																ij1 = ij - 1;
																ji1 = ji - 1;
																if (ij - ji < 0)
																{
																	r = b[ij1].m_re;
																	b[ij1].m_re = b[ji1].m_re;
																	b[ji1].m_re = r;
																	fi = b[ij1].m_im;
																	b[ij1].m_im = b[ji1].m_im;
																	b[ji1].m_im = fi;
																}
																ij++;
															}

	if (direction == FFT_FORWARD)  /* Take conjugates & unscramble outputs */
		for (i = 0, j = n / 2; j < n; i++, j++)
		{
			r = b[j].m_re; fi = b[j].m_im;
			b[j].m_re = b[i].m_re; b[j].m_im = -b[i].m_im;
			b[i].m_re = r; b[i].m_im = -fi;
		}

	if (direction == FFT_INVERSE) /* Scale outputs */
		for (i = 0; i < nthpo; i++)
		{
			b[i].m_re *= fn;
			b[i].m_im *= fn;
		}
}

/*
* Compute 2D DFT on double data:
* forward if direction==FFT_FORWARD,
* inverse if direction==FFT_INVERSE.
*/
int fft2d(complex* array, int rows, int cols, int direction)
{
	int i, maxsize, errflag;

	if (!power_of_2(rows) || !power_of_2(cols)) {
		handle_error("fft: input array must have dimensions a power of 2");
		return(ERROR);
	}

	/* Allocate 1D buffer */
	bigBuffd = array;
	maxsize = rows > cols ? rows : cols;
	errflag = allocateBuffer(maxsize);
	if (errflag != NO_ERROR) return(errflag);

	/* Compute transform row by row */
	if (cols > 1)
		for (i = 0; i < rows; i++)
		{
			LoadRow(bigBuffd, i, cols);
			FFT842(direction, cols, stageBuff);
			StoreRow(bigBuffd, i, cols);
		}

	/* Compute transform column by column */
	if (rows > 1)
		for (i = 0; i < cols; i++)
		{
			LoadCol(bigBuffd, i, rows, cols);
			FFT842(direction, rows, stageBuff);
			StoreCol(bigBuffd, i, rows, cols);
		}

	freeBuffer();
	return(NO_ERROR);
}

// [ ************************************************ ] ..................
// [ Perform forward 2D transform on a complex array.] ..................
// [ ************************************************ ] ..................
int CImageProcess::forward_fft2d(complex* array, int rows, int cols)
{
	return(fft2d(array, rows, cols, FFT_FORWARD));
}

// [ ************************************************ ] ..................
// [ Perform inverse 2D transform on a complex array.] ..................
// [ ************************************************ ] ..................
int CImageProcess::inverse_fft2d(complex* array, int rows, int cols)
{
	return(fft2d(array, rows, cols, FFT_INVERSE));
}

// [ ******************** ] ..............................................
// [ ��ͼ��ת��Ϊ������ʽ ] ..............................................
// [ ******************** ] ..............................................
CTArray< complex > CImageProcess::Image_to_complex(const CTMatrix< BYTE >& gray_image, long& new_height, long& new_width)
{
	long image_height = gray_image.Get_height();
	long image_width = gray_image.Get_width();

	new_height = image_height;
	new_width = image_width;

	if (!(power_of_2(image_height))) new_height = long(pow(2.0f, fastlog2(image_height)));
	if (!(power_of_2(image_width))) new_width = long(pow(2.0f, fastlog2(image_width)));

	CTArray< complex > array_of_complex(new_height * new_width);

	for (int row = 0; row < new_height; row++)
		for (int column = 0; column < new_width; column++)
		{
			array_of_complex[row * new_width + column].m_re = gray_image[row][column];
			array_of_complex[row * new_width + column].m_im = 0;
		}

	return array_of_complex;
}

// [ ******************** ] ..............................................
// [ ��������ʽת��Ϊͼ�� ] ..............................................
// [ ******************** ] ..............................................
CTMatrix< BYTE > CImageProcess::Complex_to_image(const CTArray< complex >& complex_array, long image_height, long image_width)
{
	CTMatrix< BYTE > gray_image(image_height, image_width);

	ASSERT(complex_array.GetDimension() == image_height * image_width);

	for (int row = 0; row < image_height; row++)
		for (int column = 0; column < image_width; column++)
			gray_image[row][column] = BYTE(complex_array[row * image_width + column].m_re);

	return gray_image;
}

// [ ********** ] ........................................................
// [ ��ͨ�˲��� ] ........................................................
// [ ********** ] ........................................................
CTArray< complex > CImageProcess::Low_pass_filter(CTArray< complex > original_signal)
{
	long dimension = original_signal.GetDimension();

	double threshold = 0;
	for (int index = 0; index < dimension; index++)
	{
		double magnitude = sqrt(original_signal[index].m_re * original_signal[index].m_re
			+ original_signal[index].m_im * original_signal[index].m_im);
		if (magnitude > threshold) threshold = magnitude;
	}
	threshold /= 100;

	for (int index = 0; index < dimension; index++)
	{
		double magnitude = sqrt(original_signal[index].m_re * original_signal[index].m_re
			+ original_signal[index].m_im * original_signal[index].m_im);

		double eplon = 1.0 / sqrt(1 + (threshold / magnitude) * (threshold / magnitude));

		original_signal[index].m_re *= eplon;
		original_signal[index].m_im *= eplon;
	}

	return original_signal;
}

// [ ********** ] ........................................................
// [ ��ͨ�˲��� ] ........................................................
// [ ********** ] ........................................................
CTArray< complex > CImageProcess::High_pass_filter(CTArray< complex > original_signal, long image_height, long image_width)
{
	long dimension = original_signal.GetDimension();

	long radius = min(image_height, image_width) / 7;

	for (int row = 0; row < image_height; row++)
		for (int column = 0; column < image_width; column++)
		{
			if ((row - image_height / 2) * (row - image_height / 2)
				+ (column - image_width / 2) * (column - image_width / 2) < radius * radius)
			{
				original_signal[row * image_width + column].m_re = 0;
				original_signal[row * image_width + column].m_im = 0;
			}
		}

	return original_signal;
}

// [ ******** ] ..............................................................
// [ ��ֵ�˲� ] ..............................................................
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