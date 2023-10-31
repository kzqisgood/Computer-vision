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
// [ Í¼ÏñÆ½ÒÆ ] ..........................................................
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
// [ Í¼Ïñ¾µÏñ ] ..........................................................
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
// [ Í¼Ïñ·ÂÉä±ä»» ] ..........................................................
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
// [ Í¼Ïñ×ªÖÃ ] ..........................................................
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
// [ Í¼ÏñÐý×ª ] ..........................................................
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
// [ Í¼ÏñËõ·Å ] ..........................................................
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
// [ Robert ±ßÔµËã×Ó ] ...................................................
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

//// [ ************ ] ......................................................
//// [ ¶þÎ¬¾í»ý²Ù×÷ ] ......................................................
//// [ ************ ] ......................................................
//CTMatrix< BYTE > Convolution_operation(const CTMatrix< BYTE >& gray_image, const CTMatrix< float >& mask)
//{
//	float sum_of_mask = 0;
//	for (int row = 0; row < mask.Get_height(); row++)
//		for (int column = 0; column < mask.Get_width(); column++)
//			sum_of_mask += mask[row][column];
//	if (sum_of_mask != 0)
//		for (int row = 0; row < mask.Get_height(); row++)
//			for (int column = 0; column < mask.Get_width(); column++)
//				mask[row][column] /= sum_of_mask;
//
//	int mask_half_height = mask.Get_height() / 2;
//	int mask_half_width = mask.Get_width() / 2;
//
//	CTMatrix< BYTE > edge_image = gray_image;
//
//	for (int row = 0; row < gray_image.Get_height(); row++)
//		for (int column = 0; column < gray_image.Get_width(); column++)
//			if (row < mask_half_height || row > gray_image.Get_height() - 1 - mask_half_height ||
//				column < mask_half_width || column > gray_image.Get_width() - 1 - mask_half_width)
//			{
//				edge_image[row][column] = 0;
//			}
//			else
//			{
//				double sum_of_convolution = 0;
//
//				for (int sub_row = 0; sub_row < mask.Get_height(); sub_row++)
//					for (int sub_column = 0; sub_column < mask.Get_width(); sub_column++)
//						sum_of_convolution += mask[sub_row][sub_column]
//						* gray_image[row + sub_row - mask_half_height][column + sub_column - mask_half_width];
//
//				edge_image[row][column] = BYTE( abs(sum_of_convolution));
//			}
//
//	return edge_image;
//}
//
//// [ ************************** ] ........................................
//// [ Sobel ±ßÔµËã×Ó£¨ÌÝ¶ÈÇ¿¶È£© ] ........................................
//// [ ************************** ] ........................................
//CTMatrix< BYTE > CImageProcess::Sobel_edge_operator(const CTMatrix< BYTE >& gray_image)
//{
//	CTMatrix< BYTE > horizontal = Sobel_edge_horizontal(gray_image);
//	CTMatrix< BYTE > vertical = Sobel_edge_vertical(gray_image);
//
//	CTMatrix< BYTE > edge_image = gray_image;
//
//	for (int row = 0; row < gray_image.Get_height(); row++)
//		for (int column = 0; column < gray_image.Get_width(); column++)
//		{
//			edge_image[row][column] = BYTE(sqrt(double(horizontal[row][column] * horizontal[row][column]
//				+ vertical[row][column] * vertical[row][column])) / 1.414);
//		}
//
//	return edge_image;
//}
//
//
//// [ ************************** ] ........................................
//// [ Sobel ±ßÔµËã×Ó£¨´¹Ö±·½Ïò£© ] ........................................
//// [ ************************** ] ........................................
//CTMatrix< BYTE > CImageProcess::Sobel_edge_vertical(const CTMatrix< BYTE >& gray_image)
//{
//	CTMatrix< float > mask_one(1, 3);
//	CTMatrix< float > mask_two(3, 1);
//
//	mask_one[0][0] = 1;
//	mask_one[0][1] = 2;
//	mask_one[0][2] = 1;
//
//	mask_two[0][0] = 1;
//	mask_two[1][0] = 0;
//	mask_two[2][0] = -1;
//
//	CTMatrix< BYTE > temp = Convolution_operation(gray_image, mask_one);
//	return Convolution_operation(temp, mask_two);
//}
//
//// [ ************************** ] ........................................
//// [ Sobel ±ßÔµËã×Ó£¨Ë®Æ½·½Ïò£© ] ........................................
//// [ ************************** ] ........................................
//CTMatrix< BYTE > CImageProcess::Sobel_edge_horizontal(const CTMatrix< BYTE >& gray_image)
//{
//	CTMatrix< float > mask_one(1, 3);
//	CTMatrix< float > mask_two(3, 1);
//
//	mask_one[0][0] = 1;
//	mask_one[0][1] = 0;
//	mask_one[0][2] = -1;
//
//	mask_two[0][0] = 1;
//	mask_two[1][0] = 2;
//	mask_two[2][0] = 1;
//
//	CTMatrix< BYTE > temp = Convolution_operation(gray_image, mask_one);
//	return Convolution_operation(temp, mask_two);
//}