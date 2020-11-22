#include "stdafx.h"
#include "PictureDrawer.h"
#include <GdiPlus.h>
#include <atlimage.h> 


using namespace Gdiplus;

PictureDrawer::PictureDrawer(long nModuleHandle)
	:m_nModuleHandle(nModuleHandle)
{
	if(m_nModuleHandle == 0)
	{
		m_nModuleHandle = (long)AfxGetResourceHandle();//获得资源的句柄
	}
}

PictureDrawer::~PictureDrawer(void)
{
}
void PictureDrawer::DrawBitmap(CPaintDC &dc, char *pszImageFile, RECT *rc, BOOL bStretch)
{ 
	if (::PathFileExists((LPCTSTR)(LPCSTR)pszImageFile))//路径有效
	{
		ATL::CImage pimage;
		HRESULT bRet = pimage.Load((LPCTSTR)(LPCSTR)pszImageFile);
		if (bRet!=S_OK)  return;
		HBITMAP hbm=pimage.Detach();
		if (hbm==NULL)  return;
		CBitmap logobmp;
		logobmp.Attach(hbm);

		BITMAP bm;
		logobmp.GetBitmap(&bm); 

		CDC memdc;

		memdc.CreateCompatibleDC(&dc);
		memdc.SelectObject(&logobmp);

		if(bStretch == TRUE)
		{
			int nModeOld = dc.SetStretchBltMode(HALFTONE);
			dc.StretchBlt(rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, &memdc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			dc.SetStretchBltMode(nModeOld);
		}
		else
			dc.BitBlt(rc->left, rc->top, bm.bmWidth, bm.bmHeight, &memdc, 0, 0, SRCCOPY);	

		memdc.DeleteDC();

		::DeleteObject ( hbm );
	}
}

void PictureDrawer::DrawBitmap(CPaintDC &dc, HANDLE hbm, RECT *rc, BOOL bStretch)
{
	CBitmap logobmp;
	logobmp.Attach(hbm);

	BITMAP bm;
	logobmp.GetBitmap(&bm); 

	CDC memdc;

	memdc.CreateCompatibleDC(&dc);
	memdc.SelectObject(&logobmp);

	if(bStretch == TRUE)
		dc.StretchBlt(rc->left, rc->top, rc->right, rc->bottom, &memdc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	else
		dc.BitBlt(rc->left, rc->top, bm.bmWidth, bm.bmHeight, &memdc, 0, 0, SRCCOPY);

	memdc.DeleteDC();
}

void PictureDrawer::DrawBitmap(CPaintDC &dc, UINT nIDResource, RECT *rc, BOOL bStretch)
{
	CBitmap logobmp;
	logobmp.LoadBitmap(nIDResource);

	BITMAP bm;
	logobmp.GetBitmap(&bm); 

	CDC memdc;

	memdc.CreateCompatibleDC(&dc);
	memdc.SelectObject(&logobmp);

	if(bStretch == TRUE)
		dc.StretchBlt(rc->left, rc->top, rc->right, rc->bottom, &memdc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	else
		dc.BitBlt(rc->left, rc->top, bm.bmWidth, bm.bmHeight, &memdc, 0, 0, SRCCOPY);

	memdc.DeleteDC();
}

//PNG图形绘制
BOOL PictureDrawer::DrawPngImage(CDC &dc, UINT nIDResource, CRect rcRect, BOOL bStretch)
{
	// 查找资源
	HRSRC hRsrc = ::FindResource((HMODULE)m_nModuleHandle, MAKEINTRESOURCE(nIDResource), _T("PNG"));
	if (hRsrc == NULL)
		return FALSE;

	// 加载资源
	HGLOBAL hImgData = ::LoadResource((HMODULE)m_nModuleHandle, hRsrc);
	if (hImgData == NULL)
	{
		::FreeResource(hImgData);
		return FALSE;
	}

	// 锁定内存中的指定资源
	LPVOID lpVoid    = ::LockResource(hImgData);
	LPSTREAM pStream = NULL;
	DWORD dwSize    = ::SizeofResource((HMODULE)m_nModuleHandle, hRsrc);
	HGLOBAL hNew    = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte    = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize);

	// 解除内存中的指定资源
	::GlobalUnlock(hNew);

	//从指定内存创建流对象
	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
	if ( ht != S_OK )
	{
		GlobalFree(hNew);
		::FreeResource(hImgData);
	}
	else
	{
		// 加载图片
		Bitmap* pobjImage = new Bitmap(pStream, TRUE);
		if (pobjImage == NULL)
		{
			GlobalFree(hNew);
			::FreeResource(hImgData);
			return FALSE;
		}

		int nImgWidth = pobjImage->GetWidth();
		int nImgHeight = pobjImage->GetHeight();

		Rect objRect;
		objRect.X = rcRect.left;
		objRect.Y = rcRect.top;

		if(bStretch)
		{
			//GDI+
			objRect.Width = rcRect.Width();
			objRect.Height = rcRect.Height();

			////待修正减1像素问题
			Graphics graphics(dc.GetSafeHdc());
			graphics.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
			graphics.DrawImage(pobjImage, objRect, 
				0, 0, nImgWidth - 1, nImgHeight - 1, UnitPixel, NULL, NULL, NULL);

			//GDI
			//CDC MemDC;
			//CBitmap cbitmap;
			//CBitmap* pOldBmp = NULL;
			//HBITMAP hBitmap;
			//// Bitmap To HBITMAP
			//// HBITMAP To CBitmap *
			//pobjImage->GetHBITMAP(Color(0,0,0),&hBitmap);       // Bitmap To HBITMAP
			//CBitmap* cBitmap = CBitmap::FromHandle(hBitmap);	// HBITMAP To CBitmap *
			////显示图片
			//MemDC.CreateCompatibleDC(&dc);   
			//pOldBmp = MemDC.SelectObject(cBitmap);   
			//dc.StretchBlt(rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, &MemDC, 0, 0, nImgWidth, nImgHeight, SRCCOPY);
			////releaseDC
			//MemDC.SelectObject(pOldBmp);
			//MemDC.DeleteDC();
			//cBitmap->DeleteObject();
		}
		else
		{
			objRect.Width = nImgWidth;
			objRect.Height = nImgHeight;

			//待修正减1像素问题
			Graphics graphics(dc.GetSafeHdc());
			//消除锯齿
			//	graphics.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
			graphics.DrawImage(pobjImage, objRect, 
				0, 0, nImgWidth, nImgHeight, UnitPixel, NULL, NULL, NULL);
		}

		delete pobjImage;
		pobjImage = NULL;

		GlobalFree(hNew);
	}

	// 释放资源
	::FreeResource(hImgData);

	return TRUE;
}

BOOL PictureDrawer::DrawPngImageTile(CPaintDC &dc, UINT nIDResource, CRect rcRect)
{
	// 查找资源
	HRSRC hRsrc = ::FindResource((HMODULE)m_nModuleHandle, MAKEINTRESOURCE(nIDResource), _T("PNG"));
	if (hRsrc == NULL)
		return FALSE;

	// 加载资源
	HGLOBAL hImgData = ::LoadResource((HMODULE)m_nModuleHandle, hRsrc);
	if (hImgData == NULL)
	{
		::FreeResource(hImgData);
		return FALSE;
	}

	// 锁定内存中的指定资源
	LPVOID lpVoid    = ::LockResource(hImgData);
	LPSTREAM pStream = NULL;
	DWORD dwSize    = ::SizeofResource((HMODULE)m_nModuleHandle, hRsrc);

	HGLOBAL hNew    = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte    = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize);

	// 解除内存中的指定资源
	::GlobalUnlock(hNew);

	//从指定内存创建流对象
	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
	if ( ht != S_OK )
	{
		GlobalFree(hNew);
		::FreeResource(hImgData);
	}
	else
	{
		// 加载图片
		Bitmap* pobjImage = new Bitmap(pStream, TRUE);
		if (pobjImage == NULL)
		{
			GlobalFree(hNew);
			::FreeResource(hImgData);
			return FALSE;
		}
		Rect objRect;
		objRect.X = rcRect.left;
		objRect.Y = rcRect.top;

		//GDI+
		objRect.Width = rcRect.Width();
		objRect.Height = rcRect.Height();

		////待修正减1像素问题
		Graphics graphics(dc.GetSafeHdc());
		TextureBrush brush(pobjImage, WrapModeTile/*FlipXY*/ );
		graphics.FillRectangle(&brush, objRect);


		delete pobjImage;
		pobjImage = NULL;

		GlobalFree(hNew);
	}

	// 释放资源
	::FreeResource(hImgData);

	return TRUE;
}

BOOL PictureDrawer::DrawPngImage(CDC &dc, char *pszImageFile, CRect rcRect, int nPos)
{
	WCHAR wchImgFileName[MAX_PATH] = {0};
	::MultiByteToWideChar(CP_ACP, 0, pszImageFile, -1, wchImgFileName, MAX_PATH);

	Bitmap* pImage = new Bitmap(wchImgFileName);
	if (pImage == NULL)
		return FALSE;

	int nImgWidth = pImage->GetWidth();
	int nImgHeight = pImage->GetHeight();

	//GDI+
	switch(nPos)
	{
	case ENUM_BITMAP_POS_STRETCH:
		{
			Rect objRect;
			objRect.X = rcRect.left;
			objRect.Y = rcRect.top;
			objRect.Width = rcRect.Width();
			objRect.Height = rcRect.Height();

			////待修正减1像素问题
			Graphics graphics(dc.GetSafeHdc());
			graphics.DrawImage(pImage, objRect, 
				0, 0, nImgWidth - 1, nImgHeight - 1, UnitPixel, NULL, NULL, NULL);
		}
		break;
	case ENUM_BITMAP_POS_LEFT_TOP:
		{
			Rect objRect;
			objRect.X = rcRect.left;
			objRect.Y = rcRect.top;
			objRect.Width = nImgWidth;
			objRect.Height = nImgHeight;

			Graphics graphics(dc.GetSafeHdc());
			graphics.DrawImage(pImage, objRect, 
				0, 0, nImgWidth, nImgHeight, UnitPixel, NULL, NULL, NULL);
		}
		break;
	case ENUM_BITMAP_POS_CENTER:
		{
			Rect objRect;
			objRect.X = rcRect.left + rcRect.Width() / 2 - nImgWidth / 2;
			objRect.Y = rcRect.top + rcRect.Height() / 2 - nImgHeight / 2;
			objRect.Width = nImgWidth;
			objRect.Height = nImgHeight;
			Graphics graphics(dc.GetSafeHdc());
			graphics.DrawImage(pImage, objRect, 
				0, 0, nImgWidth, nImgHeight, UnitPixel, NULL, NULL, NULL);
		}
		break;
	case ENUM_BITMAP_POS_RIGHT_TOP:
		{
			Rect objRect;
			objRect.X = rcRect.right - nImgWidth;
			objRect.Y = rcRect.top;
			objRect.Width = nImgWidth;
			objRect.Height = nImgHeight;
			Graphics graphics(dc.GetSafeHdc());
			graphics.DrawImage(pImage, objRect, 
				0, 0, nImgWidth, nImgHeight, UnitPixel, NULL, NULL, NULL);
		}
		break;
	default:
		break;
	}

	if(pImage != NULL)
	{
		delete pImage;
		pImage = NULL;
	}

	return TRUE;
}