#ifndef _MFCDEMO2_IMAGEPROCESS_H_
#define _MFCDEMO2_IMAGEPROCESS_H_

#include "Image.h"
#include "TArrayEx.h"
#include "TMatrixEx.h"

typedef struct { double m_re; double m_im; } complex;

class CImageProcess
{
public:
	CImageProcess(void);
	~CImageProcess(void);

	// [ ************************************************************ ]
	// [                    第 3 章 几何变换                          ]
	// [                      数字图像处理                            ]
	// [          编程框架、理论分析、实例应用和源码实现              ]
	// [ ************************************************************ ]
public:
	static CTMatrix< RGB_TRIPLE > Image_translation(const CTMatrix< RGB_TRIPLE >& color_image, long delta_x, long delta_y); // [ 图像平移 ]
	static CTMatrix< RGB_TRIPLE > Image_mirror(const CTMatrix< RGB_TRIPLE >& color_image, bool is_horizontal);
	static CTMatrix< RGB_TRIPLE > Image_affine(const CTMatrix< RGB_TRIPLE >& color_image, double m00, double m01, double m02, double m10, double m11, double m12);
	static CTMatrix< RGB_TRIPLE > Image_transpose(const CTMatrix< RGB_TRIPLE >& color_image);
	static CTMatrix< RGB_TRIPLE > CImageProcess::Image_rotation(const CTMatrix< RGB_TRIPLE >& color_image, long degrees);
	static CTMatrix< RGB_TRIPLE > CImageProcess::Image_zoom(const CTMatrix< RGB_TRIPLE >& color_image, double horizontal_scale, double vertical_scale);
	static CTMatrix< BYTE > CImageProcess::Robert_edge_operator(const CTMatrix< BYTE >& gray_image);
	static CTMatrix< BYTE > CImageProcess::Convolution_operation(const CTMatrix< BYTE >& gray_image, const CTMatrix< float >& mask);
	static CTMatrix< BYTE > CImageProcess::Sobel_edge_operator(const CTMatrix< BYTE >& gray_image);
	static CTMatrix< BYTE > CImageProcess::Sobel_edge_vertical(const CTMatrix< BYTE >& gray_image);
	static CTMatrix< BYTE > CImageProcess::Sobel_edge_horizontal(const CTMatrix< BYTE >& gray_image);
	static CTMatrix< BYTE > CImageProcess::Canny_edge_operator(const CTMatrix< BYTE >& gray_image);
	static CTMatrix< BYTE > CImageProcess::Prewitt_edge_operator(const CTMatrix< BYTE >& gray_image);
	static CTMatrix< BlackWhite > CImageProcess::Region_growing(const CTMatrix< BYTE >& gray_image, BYTE original_seed, BYTE range_of_similarity);
	static CTMatrix< int > CImageProcess::Watershed(const CTMatrix< BYTE >& OriginalImage, BYTE seed_threshold);
	static CTMatrix< int > CImageProcess::K_means_clustering(const CTMatrix< RGB_TRIPLE >& color_image, int number_of_clusters);
	static CTMatrix< BYTE > CImageProcess::Median_filter(const CTMatrix< BYTE >& gray_image, long delta);
	static CTArray< BYTE > CImageProcess::Sort_with_bubbling(CTArray< BYTE > array_of_elements);
	static CTArray< complex > CImageProcess::Low_pass_filter(CTArray< complex > original_signal);
	static CTArray< complex > CImageProcess::High_pass_filter(CTArray< complex > original_signal, long image_height, long image_width);
	static int CImageProcess::inverse_fft2d(complex* array, int rows, int cols);
	static int CImageProcess::forward_fft2d(complex* array, int rows, int cols);
	static CTArray< complex > CImageProcess::Image_to_complex(const CTMatrix< BYTE >& gray_image, long& new_height, long& new_width);
	static CTMatrix< BYTE > CImageProcess::Complex_to_image(const CTArray< complex >& complex_array, long image_height, long image_width);
};

#endif//_MFCDEMO2_IMAGEPROCESS_H_