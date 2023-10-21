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
	// [                    �� 3 �� ���α任                          ]
	// [                      ����ͼ����                            ]
	// [          ��̿�ܡ����۷�����ʵ��Ӧ�ú�Դ��ʵ��              ]
	// [ ************************************************************ ]
public:
	static CTMatrix< RGB_TRIPLE > Image_translation(const CTMatrix< RGB_TRIPLE >& color_image, long delta_x, long delta_y); // [ ͼ��ƽ�� ]
	static CTMatrix< RGB_TRIPLE > Image_mirror(const CTMatrix< RGB_TRIPLE >& color_image, bool is_horizontal);
};

#endif//_MFCDEMO2_IMAGEPROCESS_H_