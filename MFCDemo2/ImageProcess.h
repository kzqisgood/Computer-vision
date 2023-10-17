#ifndef _MFCDEMO2_IMAGEPROCESS_H_
#define _MFCDEMO2_IMAGEPROCESS_H_

#include "Image.h"
#include "TArrayEx.h"
#include "TMatrixEx.h"

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
};

#endif//_MFCDEMO2_IMAGEPROCESS_H_