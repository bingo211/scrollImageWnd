// ImageDislayWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Intelligent.h"
#include "ImageDislayWnd.h"
#include "PictureDrawer.h"

static char m_szPathResource[][MAX_PATH] = { "1.png", \
"2.png", \
"3.png",
"3.png" };

// ImageDislayWnd
IMPLEMENT_DYNAMIC(ImageDislayWnd, CStatic)

ImageDislayWnd::ImageDislayWnd()
{
	m_nImageWndRect = CRect(0,0,0,0);
}

ImageDislayWnd::~ImageDislayWnd()
{
}

BEGIN_MESSAGE_MAP(ImageDislayWnd, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()
//创建窗口
BOOL ImageDislayWnd::CreateImageWnd(CRect rcRect, CWnd* pParentWnd, UINT nID)
{
	if (!Create(NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTERIMAGE | SS_CENTER, rcRect, pParentWnd, nID))
		return FALSE;
	ModifyStyleEx(0, WS_EX_TRANSPARENT);
	ShowWindow(FALSE);
	return TRUE;
}
// ImageDislayWnd 消息处理程序
void ImageDislayWnd::DrawGridRect(COLORREF backgrdColor, COLORREF gridColor)
{
	if (m_recognitionResult == nullptr)
	{
		return;
	}
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	HDC hdc = ::GetDC(m_hWnd);
	int divWidth = rcClient.right - rcClient.left;
	int divHeight = rcClient.bottom - rcClient.top;
	int nGridCount = 3;//默认为车辆
	if (m_recognitionResult != nullptr && m_recognitionResult->m_nType == 2)
	{
		nGridCount = 2;
	}
	auto memdc = ::CreateCompatibleDC(hdc);
	auto membmp = ::CreateCompatibleBitmap(hdc, divWidth, divHeight);
	auto bmpOld = ::SelectObject(memdc, membmp);
	DrawBackground(memdc, rcClient, backgrdColor);
	DrawBorderLine(memdc, rcClient, gridColor);
	DrawChamferAngle(memdc, rcClient, RGB(66, 123, 194));
	if (m_recognitionResult != nullptr  && m_recognitionResult->ImageData.buf != nullptr)//识别结果的图片
	{
		RECT rectPicture = rcClient;
		rectPicture.right = rectPicture.left + divWidth / 2;
		DrawPictureToControl(memdc, rectPicture, &m_recognitionResult->ImageData);
	}
	char markArray[MAX_PENDANT_NUM][MAX_PATH] = { 0 };
	do{
		//文本内容的显示
		RECT recogitionRect = rcClient;
		recogitionRect.left = recogitionRect.right - divWidth / 2;
		recogitionRect.bottom = rcClient.bottom - divHeight * 3 / 5;
		int nPendantNum = 0;
		int nCount = 0;
		int nResourecCount = 0;
		if (nGridCount == 3)
		{
			VehicleResultInfo *pVehicleResult = dynamic_cast<VehicleResultInfo*>(m_recognitionResult.get());//车辆
			if (pVehicleResult == nullptr) break;

			int nHeightReg = recogitionRect.bottom - recogitionRect.top;
			recogitionRect.left = recogitionRect.left + 10;
			recogitionRect.top = recogitionRect.top + 20;
			DrawRecogitionText(memdc, recogitionRect, RGB(255, 255, 255), pVehicleResult->szVehicleName);

			recogitionRect.top = recogitionRect.top + nHeightReg / 2 - 10;
			DrawRecogitionText(memdc, recogitionRect, RGB(255, 255, 255), pVehicleResult->szVehcileNumber);
			nPendantNum = pVehicleResult->m_nRecogitionResultBits;
			while (nPendantNum)
			{
				if (nPendantNum & 0x01)//存在1
				{
					strncpy(markArray[nCount++], m_szPathResource[nResourecCount], strlen(m_szPathResource[nResourecCount]));
				}
				nResourecCount++;
				nPendantNum = nPendantNum >> 1;
			}

		}
		else
		{
			PersonResultInfo *pPersonResult = dynamic_cast<PersonResultInfo*>(m_recognitionResult.get());//行人
			if (pPersonResult == nullptr) break;
			int nHeightReg = recogitionRect.bottom - recogitionRect.top;
			recogitionRect.left = recogitionRect.left + 20;
			recogitionRect.top = recogitionRect.top + 15;
			DrawRecogitionIcon(memdc, recogitionRect, pPersonResult->bPersonSex);
			recogitionRect.top = recogitionRect.top + nHeightReg / 2;
			DrawRecogitionText(memdc, recogitionRect, RGB(255, 255, 255), pPersonResult->szPersonSexResult);
			//计算识别到挂件的类型
			nPendantNum = pPersonResult->m_nRecogitionResultBits;
			while (nPendantNum)//获取挂件的数目
			{
				if (nPendantNum & 0x01)//存在1
				{
					strncpy(markArray[nCount++], m_szPathResource[nResourecCount], strlen(m_szPathResource[nResourecCount]));
				}
				nResourecCount++;
				nPendantNum = nPendantNum >> 1;
			}
		}
	} while (0);


	RECT   bottomRect = rcClient;
	bottomRect.left = bottomRect.right - divWidth / 2;
	bottomRect.top = rcClient.top + divHeight*2 / 5;
	DrawGrid(memdc, bottomRect, gridColor, nGridCount);
	DrawMark(memdc, bottomRect, nGridCount, markArray);
	::BitBlt(hdc,
		rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		memdc,
		0,
		0,
		SRCCOPY);
	::SelectObject(memdc, bmpOld);
	::DeleteObject(membmp);
	::DeleteDC(memdc);
	::DeleteDC(hdc);
}

//显示为男士还是女士图标
void ImageDislayWnd::DrawRecogitionIcon(HDC hdc, RECT TextRect, int nIndex)
{
	CDC *pDC = CDC::FromHandle(hdc);
	PictureDrawer objPictureDrawer((long)NULL);
	switch (nIndex)
	{
	case 0://男士
		objPictureDrawer.DrawPngImage(*pDC, ".\\2.png", TextRect, PictureDrawer::ENUM_BITMAP_POS_LEFT_TOP);
		break;
	case 1://女士
		objPictureDrawer.DrawPngImage(*pDC, ".\\3.png", TextRect, PictureDrawer::ENUM_BITMAP_POS_LEFT_TOP);
		break;
	default:
		break;
	}
}

//画图标（6宫格（2*3）/4宫格（2*2））
void ImageDislayWnd::DrawMark(HDC dc, RECT rc, int nGridCount, char mark[][MAX_PATH])
{
	if (nGridCount < 2)
		return;
	int cellw = (rc.right - rc.left) / nGridCount;
	int cellh = (rc.bottom - rc.top) / 2;
	int markCount = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < nGridCount; ++j)
		{
			if (strlen(mark[markCount]) == 0)
				return;
			CDC *pDC = CDC::FromHandle(dc);
			CRect rcMark = { rc.left + j * cellw, rc.top + i * cellh, rc.left + (j + 1) * cellw, rc.top + (i + 1) * cellh };
			PictureDrawer objPictureDrawer((long)NULL);
			objPictureDrawer.DrawPngImage(*pDC, mark[markCount++], rcMark, 5);
		}
	}
}
//mark  为nIDResource  资源
void  ImageDislayWnd::DrawMark(HDC dc, RECT rc, int nGridCount, int mark[])
{
	if (nGridCount < 2)
		return;
	int cellw = (rc.right - rc.left) / nGridCount;
	int cellh = (rc.bottom - rc.top) / 2;
	int markCount = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < nGridCount; ++j)
		{
			if (mark[markCount] <= 0)
			{
				return;
			}
			CDC *pDC = CDC::FromHandle(dc);
			CRect rcMark = { rc.left + j * cellw, rc.top + i * cellh, rc.left + (j + 1) * cellw, rc.top + (i + 1) * cellh };
			PictureDrawer objPictureDrawer((long)NULL);
			objPictureDrawer.DrawPngImage(*pDC, mark[markCount++], rcMark, false);
		}
	}
}
//textColor  为字体的颜色
void ImageDislayWnd::DrawRecogitionText(HDC hdc, RECT TextRect, COLORREF textColor, const char* pRecogitionText)
{
	LOGFONT LogFont;
	memset(&LogFont, 0, sizeof(LOGFONT));
	LogFont.lfHeight = 14;
	swprintf_s(LogFont.lfFaceName, L"%s", _T("宋体"));
	CFont objFont;//字体
	objFont.CreateFontIndirect(&LogFont);
	CDC *pDC = CDC::FromHandle(hdc);
	auto  lpObjFont = pDC->SelectObject(&objFont);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(textColor);
	DrawTextA(hdc, pRecogitionText, strlen(pRecogitionText), &TextRect, DT_SINGLELINE | DT_LEFT);
	pDC->SelectObject(lpObjFont);
	::DeleteObject(objFont);

}
//绘制倒角
void ImageDislayWnd::DrawChamferAngle(HDC hdc, RECT AngleRect, COLORREF angleColor)
{
	int width = AngleRect.right - AngleRect.left;
	int height = AngleRect.bottom - AngleRect.top;
	if (width == 0 || height == 0)
	{
		return;
	}
	
	CPen m_ObjPen;
	m_ObjPen.CreatePen(PS_SOLID, 8, angleColor);
	CDC *pDC = CDC::FromHandle(hdc);
	//坐标的转换
	CPoint center;
	center.x = AngleRect.left + width / 2;
	center.y = AngleRect.top + height / 2;
	int rectWidth = width, rectHeight = height;
	int step = 20;
	CPen *oldPen = pDC->SelectObject(&m_ObjPen);

	CPoint corner = CPoint(center.x - rectWidth / 2, center.y - rectHeight / 2);
	CPoint start = corner;
	CPoint end = CPoint(start.x, start.y + step);
	pDC->MoveTo(start);
	pDC->LineTo(end);
	end = CPoint(start.x + step, start.y);
	pDC->MoveTo(start);
	pDC->LineTo(end);
	corner = CPoint(center.x + rectWidth / 2, center.y - rectHeight / 2);
	start = corner;
	end = CPoint(start.x, start.y + step);
	pDC->MoveTo(start);
	pDC->LineTo(end);
	end = CPoint(start.x - step, start.y);
	pDC->MoveTo(start);
	pDC->LineTo(end);
	corner = CPoint(center.x + rectWidth / 2, center.y + rectHeight / 2);
	start = corner;
	end = CPoint(start.x, start.y - step);
	pDC->MoveTo(start);
	pDC->LineTo(end);
	end = CPoint(start.x - step, start.y);
	pDC->MoveTo(start);
	pDC->LineTo(end);
	corner = CPoint(center.x - rectWidth / 2, center.y + rectHeight / 2);
	start = corner;
	end = CPoint(start.x, start.y - step);
	pDC->MoveTo(start);
	pDC->LineTo(end);
	end = CPoint(start.x + step, start.y);
	pDC->MoveTo(start);
	pDC->LineTo(end);

	pDC->SelectObject(oldPen);
	m_ObjPen.DeleteObject();
}
//绘制边界线
void ImageDislayWnd::DrawBorderLine(HDC hdc, RECT BorderLineRect, COLORREF BorderLineColor)
{
	auto penLine = ::CreatePen(PS_SOLID, 4, BorderLineColor);
	auto penOld = ::SelectObject(hdc, static_cast<HPEN>(penLine));
	::SetBkMode(hdc, TRANSPARENT);
	CRect rectBorder(BorderLineRect);
	rectBorder.DeflateRect(1, 1, 1, 1);
	CPoint start = CPoint(rectBorder.left, rectBorder.top);
	CPoint end = CPoint(rectBorder.right, rectBorder.top);
	::MoveToEx(hdc, rectBorder.left, rectBorder.top, nullptr);
	::LineTo(hdc, rectBorder.right, rectBorder.top);
	::LineTo(hdc, rectBorder.right, rectBorder.bottom);
	::LineTo(hdc, rectBorder.left, rectBorder.bottom);
	::LineTo(hdc, rectBorder.left, rectBorder.top);
	//画中间分割线
	::MoveToEx(hdc, start.x + rectBorder.Width() / 2, start.y, nullptr);
	::LineTo(hdc, start.x + rectBorder.Width() / 2, start.y + rectBorder.Height());
	::SelectObject(hdc, penOld);
	::DeleteObject(penLine);
}
//绘制背景
void ImageDislayWnd::DrawBackground(HDC dc, RECT rc, COLORREF  _backColor)
{
	auto objBrush = ::CreateSolidBrush(_backColor);
	::SetBkMode(dc, TRANSPARENT);
	::FillRect(dc, &rc, static_cast<HBRUSH>(objBrush));
	::DeleteObject(objBrush);
}
//绘制网格线
void ImageDislayWnd::DrawGrid(HDC dc, RECT rc, COLORREF  _gridColor, int nGridCount)
{
	if (nGridCount < 2)
		return;
	auto penLine = ::CreatePen(PS_SOLID, 1, _gridColor);
	auto penOld = ::SelectObject(dc, static_cast<HPEN>(penLine));
	int cellw = (rc.right - rc.left) / nGridCount;
	int cellh = (rc.bottom - rc.top) / 2;

	for (int i = 0; i < nGridCount; ++i)//绘制列
	{
		::MoveToEx(dc, rc.left, rc.top + i*cellh, nullptr);
		::LineTo(dc, rc.right, rc.top + i*cellh);
	}
	for (int j = 0; j < nGridCount - 1; ++j)
	{
		::MoveToEx(dc, rc.left + (j + 1)*cellw, rc.top, nullptr);
		::LineTo(dc, rc.left + (j + 1)*cellw, rc.bottom);
	}

	::SelectObject(dc, penOld);
	::DeleteObject(penLine);
}

//绘制图片到控件上
void ImageDislayWnd::DrawPictureToControl(HDC hdc, RECT _pictureRect, lpPictureData imageData)
{
	if (imageData->buf == nullptr)
		return;
	CRect rectPicture(_pictureRect);
	rectPicture.DeflateRect(4, 4, 0, 4);
	m_drawImg.CopyOf(imageData->buf, imageData->nWidth, imageData->nHeight);
	m_drawImg.DisplayFrame(hdc, imageData->nWidth, imageData->nHeight, &rectPicture);
	m_drawImg.Destroy();
}

void ImageDislayWnd::DrawPictureToControl(HDC hdc, RECT _pictureRect, const char* imageData, int width, int height)
{
	if (imageData == nullptr)
		return;
	CRect rectPicture(_pictureRect);
	rectPicture.DeflateRect(4, 4, 0, 4);
	m_drawImg.CopyOf(imageData, width, height);
	m_drawImg.DisplayFrame(hdc, width, height, &rectPicture);
	m_drawImg.Destroy();
}

void ImageDislayWnd::OnPaint()
{
	CPaintDC dc(this);
	DrawGridRect(RGB(0, 36, 78), RGB(11, 46, 88));
}

BOOL ImageDislayWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return TRUE;
}

void ImageDislayWnd::PreSubclassWindow()
{
	// TODO:  在此添加专用代码和/或调用基类
	DWORD dwStyle = GetStyle();
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | WS_CHILD | WS_CLIPSIBLINGS | SS_OWNERDRAW);
	CStatic::PreSubclassWindow();
}

void ImageDislayWnd::SetBackgroudPicture(boost::shared_ptr<BaseRecognitionResult> recognitionResult)
{
	m_recognitionResult.reset();//内存的清理
	m_recognitionResult = recognitionResult;
}
