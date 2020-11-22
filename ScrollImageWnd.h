/********************************************
公司：寰景信息
版本：V1.0
作者：FSB
日期：2016.12.20
功能：滚动窗口
遗留问题：当窗口最小化时，getclientrect全部为0，
导致不能进行图片滚动，
推荐：GetWindowPlacement可以获取窗口的屏幕坐标
*********************************************/
#pragma once
#include <map>
#include <mutex>
#include <list>
#include <atomic>
#include "ImageDislayWnd.h"
using namespace std;
#define WM_USER_LIST_IMAGE_LIST   WM_USER+6020
// ScrollImageWnd
class ScrollImageWnd : public CStatic
{
	DECLARE_DYNAMIC(ScrollImageWnd)
public:
	ScrollImageWnd();
	virtual ~ScrollImageWnd();
	BOOL CreateScrollWnd(CRect rcRect, CWnd* pParentWnd, UINT nID);//创建窗口

	void PutRecogitionResultToQueue(boost::shared_ptr<BaseRecognitionResult> recogitionResult);

protected:
	DECLARE_MESSAGE_MAP()

private:
	unsigned int m_nBaseWndIDD;
	SyncQueue<boost::shared_ptr<BaseRecognitionResult>> m_SyncQueue;
	SyncQueueEx<ImageDislayWnd*> m_FreeImageList;
	//窗口列表
	std::list<ImageDislayWnd*> m_ImageDisplayWndList;
	boost::shared_mutex m_imageDisWndMutex;
	//滚动列表
	std::list<ImageDislayWnd*> m_ScrollImageList;
	boost::shared_mutex m_scrollImageMutex;

	boost::thread* m_objScrollThread;
	std::atomic_bool m_bIsTerminate;

	//反转变量
	std::atomic_bool m_bReverse;

	//窗口数目计数
	int m_nWndCnt{0};

	//记录窗口宽度
	std::atomic<int> m_nWndHeight{0};
private:
	void InitializeImageDisplay(int count);//init scroll wnd
	ImageDislayWnd*   GetDisplayWnd();
	void ReleaseDisplayWnd(ImageDislayWnd*   wnd);
	void DestroyDisplayWnd();
	//绘制背景颜色
	void DrawBackgroundPicture(CPaintDC &dc);
	void ImageListThreadProcess();

	CRect GetRectByResultType(RESULT_MSG  msg);
	void OnMessageListImage();
	int GetDisplayWndSize();

public:
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PreSubclassWindow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


