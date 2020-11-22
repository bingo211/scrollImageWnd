/*********************************
公司：寰景信息
版本：V1.0
作者：FSB
日期：2016.12.20
功能：
**********************************/
#pragma once
#include "RecogitionResult.h"
#include "DrawImage.h"
#include <boost/shared_ptr.hpp>
#include <atomic>
#include "SyncQueue.h"
using namespace std;
// ImageDislayWnd
class ImageDislayWnd : public CStatic
{
	DECLARE_DYNAMIC(ImageDislayWnd)

public:
	ImageDislayWnd();
	virtual ~ImageDislayWnd();
	CRect m_nImageWndRect;

	BOOL CreateImageWnd(CRect rcRect, CWnd* pParentWnd, UINT nID);//创建窗口

	void SetBackgroudPicture(boost::shared_ptr<BaseRecognitionResult> recognitionResult);
private:

	/*********************************
	backgrdColor  背景颜色
	gridColor  网格线颜色
	************************************/
	void DrawGridRect(COLORREF backgrdColor, COLORREF gridColor/*, BaseRecognitionResult* recognitionResult*/);

	void DrawBackground(HDC dc, RECT rc, COLORREF  _backColor);
	void DrawGrid(HDC dc, RECT rc, COLORREF  _gridColor, int nGridCount);
	void   DrawMark(HDC dc, RECT rc, int nGridCount, char mark[][MAX_PATH]);
	//加载本地资源
	void  DrawMark(HDC dc, RECT rc, int nGridCount, int mark[]);
	//待实现
	void DrawRecogitionText(HDC hdc, RECT TextRect, COLORREF textColor, const char* pRecogitionText);
	void DrawRecogitionIcon(HDC hdc, RECT TextRect, int nIndex);
	//绘制倒角
	void DrawChamferAngle(HDC hdc, RECT AngleRect, COLORREF angleColor);
	//绘制dlg的边界线
	void DrawBorderLine(HDC hdc, RECT BorderLineRect, COLORREF BorderLineColor);
	//绘制图片
	void DrawPictureToControl(HDC hdc, RECT _pictureRect, lpPictureData imageData);
	void DrawPictureToControl(HDC hdc, RECT _pictureRect, const char* imageData, int width, int height);
private:
	CDrawImage m_drawImg;

	boost::shared_ptr<BaseRecognitionResult> m_recognitionResult;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PreSubclassWindow();
};

template<typename DataE>
class SyncQueueEx :public SyncQueue<DataE>
{
public:
	SyncQueueEx(){}
	virtual ~SyncQueueEx(){}

	void NotifyWndRadio(double nZoomRatio)
	{
		boost::mutex::scoped_lock lock(m_mtx);
		for (auto imageListIter : m_objList)
		{
			CRect& rect = imageListIter->m_nImageWndRect;
			rect = CRect(rect.left* nZoomRatio, rect.top*nZoomRatio, rect.right * nZoomRatio, rect.bottom* nZoomRatio);
		}
	}
};