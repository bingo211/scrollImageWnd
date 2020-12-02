// ScrollImageWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Intelligent.h"
#include "ScrollImageWnd.h"
#include <boost/lexical_cast.hpp>  

//默认步长
#define DEFAULT_WDN_SHIFLT_STEP 10

#define TIMER_INTERVAL_ID 2345
#define TIMER_INTERVAL_SPEED 300

// ScrollImageWnd
IMPLEMENT_DYNAMIC(ScrollImageWnd, CStatic)

ScrollImageWnd::ScrollImageWnd()
{
	m_nBaseWndIDD = 5000;
	m_bIsTerminate = false;
	m_bReverse = true;
}

ScrollImageWnd::~ScrollImageWnd()
{
}


BEGIN_MESSAGE_MAP(ScrollImageWnd, CStatic)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// ScrollImageWnd 消息处理程序
BOOL ScrollImageWnd::CreateScrollWnd(CRect rcRect, CWnd* pParentWnd, UINT nID)
{
	if (!Create(NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_CENTERIMAGE | SS_CENTER, rcRect, pParentWnd, nID))
		return FALSE;
	ModifyStyleEx(0, WS_EX_TRANSPARENT);
	ShowWindow(TRUE);
	//初始化显示窗口数目为20
	InitializeImageDisplay(20);
	return TRUE;
}

void ScrollImageWnd::InitializeImageDisplay(int count)
{
	for (int i = 0; i < count; i++)
	{
		ImageDislayWnd *pWnd = new ImageDislayWnd();
		if (pWnd != nullptr)
		{
			pWnd->CreateImageWnd(CRect(0, 0, 0, 0), this, m_nBaseWndIDD);
			m_nBaseWndIDD++;
			m_ImageDisplayWndList.push_back(pWnd);
		}
	}
	m_nWndCnt = count;
	m_objScrollThread = new boost::thread(boost::bind(&ScrollImageWnd::ImageListThreadProcess, this));
	//启动定时器
	SetTimer(TIMER_INTERVAL_ID, TIMER_INTERVAL_SPEED,NULL);
}

ImageDislayWnd* ScrollImageWnd::GetDisplayWnd()
{
	ImageDislayWnd* pWnd = nullptr;
	do 
	{
		boost::shared_lock<boost::shared_mutex> readLock(m_imageDisWndMutex);
		if (m_ImageDisplayWndList.empty())
		{
			pWnd = new ImageDislayWnd;
			if (pWnd != nullptr)
			{
				pWnd->CreateImageWnd(CRect(0, 0, 0, 0), this, m_nBaseWndIDD);
				m_nBaseWndIDD++;
				return pWnd;
			}
			return nullptr;
		}
	} while (0);
	boost::unique_lock<boost::shared_mutex> lock(m_imageDisWndMutex);
	pWnd = m_ImageDisplayWndList.front();
	m_ImageDisplayWndList.pop_front();
	return pWnd;
}

//reuse window
void ScrollImageWnd::ReleaseDisplayWnd(ImageDislayWnd* wnd)
{
	boost::unique_lock<boost::shared_mutex> lock(m_imageDisWndMutex);
	m_ImageDisplayWndList.push_back(wnd);
}

void ScrollImageWnd::DestroyDisplayWnd()
{
	boost::unique_lock<boost::shared_mutex> lock(m_imageDisWndMutex);
	for (auto pWnd : m_ImageDisplayWndList)
	{
		pWnd->DestroyWindow();
		delete pWnd;
	}
	m_ImageDisplayWndList.clear();
}

//获取显示窗口的大小
int ScrollImageWnd::GetDisplayWndSize()
{
	boost::shared_lock<boost::shared_mutex> readLock(m_imageDisWndMutex);
	return m_ImageDisplayWndList.size();
}

void ScrollImageWnd::OnDestroy()
{
	//关闭定时器
	KillTimer(TIMER_INTERVAL_ID);
	//关闭线程
	m_bIsTerminate = true;
	if (m_objScrollThread != nullptr)
	{
		m_objScrollThread->timed_join(boost::posix_time::seconds(1));
		delete m_objScrollThread;
		m_objScrollThread = nullptr;
	}
	DestroyDisplayWnd();

	CStatic::OnDestroy();
}

void ScrollImageWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	DrawBackgroundPicture(dc);
}

void ScrollImageWnd::DrawBackgroundPicture(CPaintDC &dc)
{
	CRect rcClient;
	GetClientRect(rcClient);
	CBrush brush(RGB(19,46,73));
	dc.FillRect(rcClient, &brush);
	brush.DeleteObject();
}

void ScrollImageWnd::PutRecogitionResultToQueue(boost::shared_ptr<BaseRecognitionResult> recogitionResult)
{
	m_SyncQueue.push(recogitionResult);
}

void ScrollImageWnd::ImageListThreadProcess()
{
	while (!m_bIsTerminate)
	{
		ImageDislayWnd* pImageWnd = nullptr;
		int nFreeLen = m_FreeImageList.getSize();
		int nScrollLen = 0;
		do 
		{
			boost::unique_lock<boost::shared_mutex> WriteLock(m_scrollImageMutex);
			nScrollLen = m_ScrollImageList.size();
			if (nFreeLen)
			{
				if (nScrollLen > 1)
				{
					auto imageListIter = --m_ScrollImageList.end();
					--imageListIter;
					CRect& rect = (*imageListIter)->m_nImageWndRect;
					if (rect.left > 0)
					{
						m_FreeImageList.pull(pImageWnd);
						m_ScrollImageList.push_back(pImageWnd);
					}
				}
				else
				{
					m_FreeImageList.pull(pImageWnd);
					m_ScrollImageList.push_back(pImageWnd);
				}
			}
		} while (0);

		if (nScrollLen + nFreeLen < m_nWndCnt)
		{
			boost::shared_ptr<BaseRecognitionResult> recogitionResult;
			bool bRet = m_SyncQueue.pull(recogitionResult); 
			if (bRet)
			{
				RESULT_MSG type = recogitionResult->m_nType;
				CRect rect = GetRectByResultType(type);
				pImageWnd = GetDisplayWnd();
				pImageWnd->SetBackgroudPicture(std::move(recogitionResult));
				//pImageWnd->ShowWindow(TRUE);
				pImageWnd->m_nImageWndRect = rect;
				m_FreeImageList.push(pImageWnd);
			}
		}
		boost::this_thread::sleep(boost::posix_time::millisec(10));
	}
}

CRect ScrollImageWnd::GetRectByResultType(RESULT_MSG  msg)
{
	CRect  rect;
	CRect rcClient;
	GetClientRect(rcClient);
    int nHeight = rcClient.bottom - rcClient.top;
// 	static std::once_flag once_flag;
// 	std::call_once(once_flag, [&]{
// 		m_nWndHeight.store(nHeight);
// 	});
	int nHalfHight = nHeight / 2;
	switch (msg)
	{
	case VEHICLE_RESULT_MSG:
	{
		if (m_bReverse)
			rect = CRect(std::floor<int>(-nHalfHight*1.4), 10, 0, nHalfHight - 5 );
		else
			rect = CRect(std::floor<int>(-nHalfHight * 1.4), nHalfHight + 5, 0, nHeight - 10);
	}
	break;
	case PERSON_RESULT_MSG:
	default:
	{
		if (m_bReverse)
			rect = CRect(std::floor<int>(-nHalfHight * 1.3), 10, 0, nHalfHight - 5);
		else
			rect = CRect(std::floor<int>(-nHalfHight*1.3), nHalfHight + 5, 0, nHeight-10);
     }
	break;
	}
	m_bReverse = !m_bReverse;
	return rect;
}

BOOL ScrollImageWnd::OnEraseBkgnd(CDC* pDC)
{
	//return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}


void ScrollImageWnd::PreSubclassWindow()
{
	DWORD dwStyle = GetStyle();
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | WS_CHILD | WS_CLIPSIBLINGS | SS_OWNERDRAW);
	CStatic::PreSubclassWindow();
}

//定时器设置为300毫秒
void ScrollImageWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_INTERVAL_ID == nIDEvent)
	{
		OnMessageListImage();
	}
	CStatic::OnTimer(nIDEvent);
}

void ScrollImageWnd::OnMessageListImage()
{
	int nRecogImageSize = m_SyncQueue.getSize();
	boost::unique_lock<boost::shared_mutex> WriteLock(m_scrollImageMutex);
	for (auto imageListIter = m_ScrollImageList.begin(); imageListIter != m_ScrollImageList.end();)
	{
		CRect rcClient;
		GetClientRect(rcClient);
		CRect& rect = (*imageListIter)->m_nImageWndRect;
		(*imageListIter)->ShowWindow(TRUE);
		if (rect.left >= rcClient.right)
		{
			(*imageListIter)->ShowWindow(FALSE);
			ReleaseDisplayWnd((*imageListIter));
			imageListIter = m_ScrollImageList.erase(imageListIter);
		}
		//两行进行显示时应该使用right来进行判断，如果使用一行进行显示时，应该使用left进行判断
		else if (nRecogImageSize == 0 && m_FreeImageList.getSize()==0 && (*--m_ScrollImageList.end())->m_nImageWndRect.left > 0)
		{
			return;
		}
		else
		{
			int nscale = nRecogImageSize / 3 + 1;
			nscale = std::min<int>(nscale , 20);
			if (nscale)
			{
				int nStep = nscale* DEFAULT_WDN_SHIFLT_STEP;
				(*imageListIter)->MoveWindow(rect);
				rect.left += nStep;
				rect.right += nStep;
				++imageListIter;
			}
		}
	}
}

void ScrollImageWnd::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	//zoom show window by adjust window size 
	CRect rcClient;
	GetClientRect(rcClient);
	int nWndHeight = rcClient.bottom - rcClient.top;
	double nWndZoomRatio = (double)nWndHeight / (m_nWndHeight.load()+0.001);
	m_nWndHeight.store(nWndHeight);
	do 
	{
		boost::unique_lock<boost::shared_mutex> WriteLock(m_scrollImageMutex);
		for (auto imageListIter : m_ScrollImageList)
		{
			CRect& rect = imageListIter->m_nImageWndRect;
			rect = CRect(rect.left* nWndZoomRatio, rect.top*nWndZoomRatio, rect.right * nWndZoomRatio, rect.bottom* nWndZoomRatio);
		}
	} while (0);

	m_FreeImageList.NotifyWndRadio(nWndZoomRatio);
}
