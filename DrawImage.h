/*********************************
公司：寰景信息
版本：V1.0
作者：FSB
日期：2016.12.20
功能：
**********************************/
#pragma once
class CDrawImage
{
public:
	CDrawImage(void);
	~CDrawImage(void);

	bool InitCommonPlayer(int FrameType);

	void  CopyOf(const char* _tmpBuffer,int width,int height);

	void DisplayFrame(HDC hDCDst,int width,int height,RECT* pDstRect);

	void Destroy();
//	bool SaveImage(const char* filename);
	enum
	{
		BIT_COUNT_8=1,
		BIT_COUNT_16=2,
		BIT_COUNT_24
	};
	//void Fill(int rgbValue);
private:
	void FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin );
	void  Show( HDC dc, const char* _data, int x, int y, int width, int height,int from_x = 0, int from_y = 0 );
	int Bpp() { return m_nCount;}
//	void SaveBmpImage(const char* filename,const char* _dataBuffer,int width,int height);
	int m_nCount;
	char* m_szSourceData;
	//int m_nImageWidth;
//	int m_nImageHeight;
};

