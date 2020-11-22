/*********************************
��˾��徾���Ϣ
�汾��V1.0
���ߣ�FSB
���ڣ�2016.12.20
���ܣ�
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

	BOOL CreateImageWnd(CRect rcRect, CWnd* pParentWnd, UINT nID);//��������

	void SetBackgroudPicture(boost::shared_ptr<BaseRecognitionResult> recognitionResult);
private:

	/*********************************
	backgrdColor  ������ɫ
	gridColor  ��������ɫ
	************************************/
	void DrawGridRect(COLORREF backgrdColor, COLORREF gridColor/*, BaseRecognitionResult* recognitionResult*/);

	void DrawBackground(HDC dc, RECT rc, COLORREF  _backColor);
	void DrawGrid(HDC dc, RECT rc, COLORREF  _gridColor, int nGridCount);
	void   DrawMark(HDC dc, RECT rc, int nGridCount, char mark[][MAX_PATH]);
	//���ر�����Դ
	void  DrawMark(HDC dc, RECT rc, int nGridCount, int mark[]);
	//��ʵ��
	void DrawRecogitionText(HDC hdc, RECT TextRect, COLORREF textColor, const char* pRecogitionText);
	void DrawRecogitionIcon(HDC hdc, RECT TextRect, int nIndex);
	//���Ƶ���
	void DrawChamferAngle(HDC hdc, RECT AngleRect, COLORREF angleColor);
	//����dlg�ı߽���
	void DrawBorderLine(HDC hdc, RECT BorderLineRect, COLORREF BorderLineColor);
	//����ͼƬ
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