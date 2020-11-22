#include "stdafx.h"
#include "DrawImage.h"
#include <assert.h>
#include <math.h>

CDrawImage::CDrawImage(void)
{
	m_nCount=24;
	m_szSourceData = nullptr;
}


CDrawImage::~CDrawImage(void)
{
	Destroy();
}

void CDrawImage::DisplayFrame( HDC hDCDst ,int width,int height,RECT* pDstRect)
{
	if( pDstRect && m_szSourceData )
	{
		unsigned char buffer[sizeof(BITMAPINFOHEADER) + 1024]={0};
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		int bmp_w = width, bmp_h = height;
		int dstW = pDstRect->right -pDstRect->left;
		int dstH = pDstRect->bottom -pDstRect->top;
		if( width == dstW && height == dstH )
		{
			Show(hDCDst, m_szSourceData, pDstRect->left, pDstRect->top,  dstW, dstH, 0, 0);
			return;
		}
		if( width > width )
		{
			SetStretchBltMode(
				hDCDst,   
				HALFTONE );
		}
		else
		{
			SetStretchBltMode(
				hDCDst, 
				COLORONCOLOR );
		}
		FillBitmapInfo( bmi, bmp_w, bmp_h, Bpp(), 0 );
		::StretchDIBits(
			hDCDst,
			pDstRect->left, pDstRect->top, dstW, dstH,
			0, 0, width,height,
			m_szSourceData, bmi, DIB_RGB_COLORS, SRCCOPY );
	}
}

void CDrawImage:: FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin )
{
	assert( bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));

	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

	memset( bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = width;
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;
	if( bpp == 8 )
	{
		RGBQUAD* palette = bmi->bmiColors;
		int i;
		for( i = 0; i < 256; i++ )
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
}

void CDrawImage::Show( HDC dc, const char* _data,int x, int y, int width, int height,
	int from_x, int from_y )
{
	if( _data )
	{
		unsigned char buffer[sizeof(BITMAPINFOHEADER) + 1024]={0};
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		int bmp_w = width, bmp_h = height;
		FillBitmapInfo( bmi, bmp_w, bmp_h, Bpp(), 0 );
		from_x = min( max( from_x, 0 ), bmp_w - 1 );
		from_y = min( max( from_y, 0 ), bmp_h - 1 );
		int sw = max( min( bmp_w - from_x, width ), 0 );
		int sh = max( min( bmp_h - from_y, height ), 0 );
		SetDIBitsToDevice(
			dc, x, y, sw, sh, from_x, from_y, from_y, sh,
			_data,
			bmi, DIB_RGB_COLORS );
	}
}

bool CDrawImage::InitCommonPlayer( int FrameType )
{
	switch (FrameType)
	{
	case BIT_COUNT_24:
		m_nCount =24;
		break;
	case BIT_COUNT_16:
		m_nCount = 16;
		break;
	case BIT_COUNT_8:
		m_nCount =8;
		break;
	default:
		return false;
	}
	return true;
}

void CDrawImage::Destroy()
{
	if(m_szSourceData != nullptr)
	{
		delete[] m_szSourceData;
		m_szSourceData = nullptr;
	}
}

void CDrawImage::CopyOf( const char* _tmpBuffer,int width,int height )
{
	Destroy();
	int dataLen = width*height*Bpp()>>3;
	m_szSourceData = new char[dataLen+1];//ÉêÇë´óÐ¡
	memset(m_szSourceData,0,dataLen);
	::memcpy(m_szSourceData,_tmpBuffer,dataLen);
}

#if 0
void CDrawImage::Fill( int rgbValue )
{
	int r = rgbValue&255;
	int g = (rgbValue>>8)&255;
	int b =(rgbValue>>16)&255;
	int alpha =(rgbValue>>24)&255;
	for (int i=0;i<m_nImageWidth;++i)
		for (int j=0;j<m_nImageHeight;++j)
		{
			m_szSourceData[j+3*i*m_nImageWidth] =r;
			m_szSourceData[j+3*i*m_nImageWidth+1] =g;
			m_szSourceData[j+3*i*m_nImageWidth+2] =b;
		}
}

void CDrawImage::SaveBmpImage( const char* filename,const char* _dataBuffer,int width,int height )
{
	BITMAPFILEHEADER bfh;
	bfh.bfType = 0x4d42;  
	int size = width*height*Bpp()>>3;
	bfh.bfSize = size+ sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) ;
	bfh.bfReserved1 = 0; 
	bfh.bfReserved2 = 0; 
	bfh.bfOffBits = bfh.bfSize - size;
	BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = 0;
	bih.biSizeImage = size;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;  
	FILE* file = fopen(filename,"ab+");
	fwrite(&bfh,1,sizeof(BITMAPFILEHEADER),file);
	fwrite(&bih,1,sizeof(BITMAPINFOHEADER),file);
	fwrite(_dataBuffer,1,size,file);
	fclose(file);
}

bool CDrawImage::SaveImage( const char* filename )
{
	if(::lstrlen(filename) < 1)
	{
		return false;
	}

	// SaveBmpImage();
}
#endif