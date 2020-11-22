#pragma once

class PictureDrawer
{
public:
	PictureDrawer(long nModuleHandle);

	~PictureDrawer(void);

	enum ENUM_BITMAP_POS
	{
		ENUM_BITMAP_POS_LEFT_TOP = 1,
		ENUM_BITMAP_POS_LEFT_BOTTOM,
		ENUM_BITMAP_POS_RIGHT_TOP,
		ENUM_BITMAP_POS_RIGHT_BOTTOM,
		ENUM_BITMAP_POS_CENTER,
		ENUM_BITMAP_POS_STRETCH,
	};

public:

	//BMP图形绘制
	void DrawBitmap(CPaintDC &dc, HANDLE hbm, RECT *rc, BOOL bStretch = TRUE);
	void DrawBitmap(CPaintDC &dc, char *pszImageFile, RECT *rc, BOOL bStretch = TRUE);
	void DrawBitmap(CPaintDC &dc, UINT nIDResource, RECT *rc, BOOL bStretch = TRUE);
	//PNG图形绘制
	BOOL DrawPngImage( CDC &dc, UINT nIDResource, CRect rcRect, BOOL bStretch = TRUE);
	BOOL DrawPngImageTile(CPaintDC &dc, UINT nIDResource, CRect rcRect);
	BOOL DrawPngImage(CDC &dc, char *pszImageFile, CRect rcRect, int nPos = ENUM_BITMAP_POS_STRETCH);

private:
	long m_nModuleHandle;
};
