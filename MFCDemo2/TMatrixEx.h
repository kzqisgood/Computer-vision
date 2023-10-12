// TMatrixEx.h: interface for the CTMatrixEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMATRIXEX_H__E3C63904_F0FA_43D2_9888_779A42AEADCB__INCLUDED_)
#define AFX_TMATRIXEX_H__E3C63904_F0FA_43D2_9888_779A42AEADCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TArrayEx.h"
#include "ImagePoint.h"

// [ ******************************************************************* ]
// [ ģ���� CTMatrix< TYPE > �� CTMatrixEx< TYPE > ����������:           ]
// [        �ڹ����������( ������ )����( Construction )�н��о����� ]
// [        ʱ, CTMatrix �����������ڴ�Ŀ���, CTMatrixEx ���������Ԫ�� ]
// [        �Ŀ���.                                                      ]
// [ ����˵��: CTMatrix �е� TYPE �ǲ����ڶ�̬�����ڴ����������, ������ ]
// [               ������������( �� int, double �� )�� struct ���� �Լ�  ]
// [               class ����( �� CPoint �� ).                           ]
// [           CTMatrixEx �е� TYPE ����Ǵ��ڶ�̬�����ڴ����������, �� ]
// [               �� struct ���ͺ� class ����( �� CString �� )��        ]
// [           CTMatrixEx �������е���������, �������ٶ���; CTMatrix ֻ  ]
// [               ���ò����ڶ�̬�����ڴ����������, �������ٶȿ�.       ]
// [           ����˵, CTMatrixEx �� CTMatrix ����չ, �� CTMatrix ��     ]
// [               CTMatrixEx ������.                                    ]
// [ ******************************************************************* ]

// [ ******************************************************************* ]
// [ ********************* << ��ά����ģ����չ >> ********************** ]
// [ ******************************************************************* ]

template <class TYPE> class CTMatrixEx  
{
	// [ ******** ] ......................................................
    // [ ��Ա���� ] ......................................................
	// [ ******** ] ......................................................
private:
	TYPE* m_memory;  // [ ���������ڴ�ָ�� ]
	long  m_rows;    // [ ��������� ]
	long  m_columns; // [ ��������� ]

	// [ ****************** ] ............................................
	// [ ���캯������������ ] ............................................
	// [ ****************** ] ............................................
public:
	CTMatrixEx()                                    // [ ���캯��: ȱʡ���캯�� ]
	{
	    Initialization(); // [ ��ʼ����Ա���� ]
    }

	CTMatrixEx( long rows, long columns )           // [ ���캯��: ��������ڴ� ]
	{
	    Initialization(); // [ ��ʼ����Ա���� ]

	    Construction( rows, columns ); // [ �����������: ��������ڴ� ]
    }

	CTMatrixEx( const CTMatrixEx< TYPE >& matrix )  // [ ���ƹ��캯�� ]
	{
	    Initialization(); // [ ��ʼ����Ա���� ]

	    Construction( matrix ); // [ �����������: ������ ]
    }

	~CTMatrixEx()                                   // [ �������� ]
	{
	    Destruction( ); // [ ������������ ]
    }

	// [ ************ ] ..................................................
	// [ ���ܽӿں��� ] ..................................................
	// [ ************ ] ..................................................
public:
	void Construction( long rows, long columns )          // [ �����������: ��������ڴ� ]
	{
		if( m_rows == rows && m_columns == columns && m_memory != NULL ) // [ �����Ҫ����ľ����ڴ��С���䣬����ԭ�о����ڴ� ]
			return ;

		Destruction( ); // [ ������������ ]

		if( rows > 0 && columns > 0 ) // [ ����������������������� 0 ]
		{
			long elements = rows * columns ; // [ ����Ԫ�ظ��� ]

			m_memory = new TYPE[ elements ]; // [ �����ڴ� ]

			if( m_memory != NULL ) // [ �жϷ����ڴ��Ƿ�ɹ� ]
			{
				m_rows = rows; // [ ���þ������� ]
				m_columns = columns; // [ ���þ������� ]
			}
			else
			{
				Verification( "System Error << Can not allocate memory correctly >>" ); // [ ���Ա��� ]
			}
		}
		else
		{
			Verification( "Program Error << Matrix rows and columns must be bigger than 0 >>" ); // [ ���Ա��� ]
		}
	}

	void Construction( const CTMatrixEx< TYPE >& matrix ) // [ �����������: ������ ]
	{
		if( matrix.m_memory != NULL ) // [ �жϾ��������Ƿ�Ϊ�� ]
		{
			Construction( matrix.m_rows, matrix.m_columns ); // [ ������ͬ�ߴ�ľ���: ��������ڴ� ]

			long elements = m_rows * m_columns; // [ ����Ԫ�ظ��� ]

			for( int index = 0; index < elements; index++ ) // [ ������ƾ���Ԫ�� ]
			{
				m_memory[ index ] = matrix.m_memory[ index ]; // [ ���ƾ���Ԫ�� ]
			}
		}
		else
		{
			Destruction( ); // [ ������������ ]
		}
	}

	void Destruction()                                    // [ ������������ ]
	{
		if( m_memory != NULL ) // [ �ж��Ƿ�������ڴ� ]
		{
			delete [] m_memory; // [ �ͷ��ڴ� ]

			Initialization( ); // [ ��ʼ����Ա���� ]
		}
	}   

	CTMatrixEx< TYPE >& operator=( const CTMatrixEx< TYPE >& matrix )  // [ ��ֵ������ ]
	{
	    Construction( matrix ); // [ �����������: ������ ]

	    return * this; // [ ���ص�ǰ���� ]
    }
	
#ifdef _DEBUG
	CTArrayEx< TYPE >& operator[]( long index ) const                  // [ �±������ ]
	{
    	static CTArrayEx< TYPE > error; // [ �������صľ�̬���� ]

		if( !( index >= 0 && index < m_rows ) ) // [ �����ж�λ��Ч�Բ��� ]
		{
			Verification( "Program Error << Matrix row index beyond bound >>" ); // [ ���Ա��� ]

			return error; // [ �������� ]
		}

		static CTArrayEx< TYPE > rowArray; // [ ���������� ]

		rowArray.Construction( m_memory + index * m_columns, m_columns ); // [ ������������: ָ�������ڴ� ]

		return rowArray; // [ ���ؾ��������� ]
	}

#else // _DEBUG
	TYPE*              operator[]( long index ) const                  // [ �±������ ]
	{
	    return m_memory + index * m_columns; // [ ���ؾ����������ַ ]
    }

#endif // _DEBUG

	TYPE&              operator[]( const CImagePoint& point ) const    // [ �±������ ]
	{
#ifdef _DEBUG

	    return ( *this )[ point.m_row ][ point.m_column ] ;

#else // _DEBUG

	    return *( m_memory + point.m_row * m_columns + point.m_column );

#endif // _DEBUG
    }

	bool Is_point_valid( const CImagePoint& point ) const // [ �������Ƿ���ȷ ]
	{
	    return point.m_row    >= 0 && point.m_row    < m_rows     // [ ������ ]
	    	&& point.m_column >= 0 && point.m_column < m_columns; // [ ������ ]
    }

	bool Is_row_valid( long row ) const                   // [ �������Ƿ���ȷ ]
	{
	    return row >= 0 && row < m_rows; // [ ������ ]
    }

	bool Is_column_valid( long column ) const             // [ �������Ƿ���ȷ ]
	{
	    return column >= 0 && column < m_columns; // [ ������ ]
    }

	// [ ************ ] ..................................................
    // [ ״̬�������� ] ..................................................
	// [ ************ ] ..................................................
public:
	bool IsNull() const              // [ �Ƿ�Ϊ�� ]
	{
	    return ( m_memory == NULL );
    } 

	bool IsNotNull() const           // [ �Ƿ񲻿� ]
	{
	    return ( m_memory != NULL );
    }

	long GetRows() const             // [ �������� ]
	{
	    return m_rows;
    }

	long GetColumns() const          // [ �������� ]
	{
	    return m_columns;
    }

	long Get_height() const          // [ ����߶�( �������� ) ]
	{
	    return m_rows;
    }

	long Get_width() const           // [ �������( �������� ) ]
	{
	    return m_columns;
    }

	long GetRowUpperBound() const    // [ �������Ͻ� ]
	{
	    return ( m_rows - 1 );
    }

	long GetColumnUpperBound() const // [ �������Ͻ� ]
	{
	    return ( m_columns - 1 );
    }

	long GetElements() const         // [ ����Ԫ�ظ��� ]
	{
	    return ( m_rows * m_columns );
    }

	long GetSizeInBytes() const      // [ ����������ռ�ֽ��� ]
	{
	    return ( m_rows * m_columns * sizeof( TYPE ) );
    }

	CImagePoint Get_size() const     // [ ����ߴ�( �������������� ) ]
	{
	    return CImagePoint( m_rows, m_columns );
    }

	// [ ************ ] ..................................................
	// [ �ڲ�ʵ�ֺ��� ] ..................................................
	// [ ************ ] ..................................................
private:
	void Initialization() // [ ��ʼ����Ա���� ]
	{
		m_memory = NULL; // [ �����ڴ�ָ����� ]
		m_rows = 0;      // [ ������������ ]
		m_columns = 0;   // [ ������������ ]

		// [ *************************************************************** ]
		// [ ��ʼ����Ա����( Initialization )��, ��Ա�����������¹���:       ]
		// [     1. ��������ڴ�ָ�벻��( m_memory != NULL ), ������������� ]
		// [        ������Ϊ��( m_rows > 0 && m_columns > 0 ).               ]
		// [     2. ��������ڴ�ָ��Ϊ��( m_memory == NULL ), ������������� ]
		// [        ����Ϊ��( m_rows == 0 && m_columns == 0 ).               ]
		// [     3. ��������������Ҫô��Ϊ�㣬Ҫô����Ϊ��.                  ]
		// [ *************************************************************** ]
    } 

	void Verification( CString message ) const // [ ���Ա��� ]
	{
#ifdef _DEBUG

	    AfxMessageBox( message ); // [ ���Ա��� ]

#endif // _DEBUG
    }

};

#endif // !defined(AFX_TMATRIXEX_H__E3C63904_F0FA_43D2_9888_779A42AEADCB__INCLUDED_)