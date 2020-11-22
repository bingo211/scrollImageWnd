/********************************************
��˾��徾���Ϣ
�汾��V1.0
���ߣ�FSB
���ڣ�2016.12.20
���ܣ���������
�������⣺��������С��ʱ��getclientrectȫ��Ϊ0��
���²��ܽ���ͼƬ������
�Ƽ���GetWindowPlacement���Ի�ȡ���ڵ���Ļ����
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
	BOOL CreateScrollWnd(CRect rcRect, CWnd* pParentWnd, UINT nID);//��������

	void PutRecogitionResultToQueue(boost::shared_ptr<BaseRecognitionResult> recogitionResult);

protected:
	DECLARE_MESSAGE_MAP()

private:
	unsigned int m_nBaseWndIDD;
	SyncQueue<boost::shared_ptr<BaseRecognitionResult>> m_SyncQueue;
	SyncQueueEx<ImageDislayWnd*> m_FreeImageList;
	//�����б�
	std::list<ImageDislayWnd*> m_ImageDisplayWndList;
	boost::shared_mutex m_imageDisWndMutex;
	//�����б�
	std::list<ImageDislayWnd*> m_ScrollImageList;
	boost::shared_mutex m_scrollImageMutex;

	boost::thread* m_objScrollThread;
	std::atomic_bool m_bIsTerminate;

	//��ת����
	std::atomic_bool m_bReverse;

	//������Ŀ����
	int m_nWndCnt{0};

	//��¼���ڿ��
	std::atomic<int> m_nWndHeight{0};
private:
	void InitializeImageDisplay(int count);//init scroll wnd
	ImageDislayWnd*   GetDisplayWnd();
	void ReleaseDisplayWnd(ImageDislayWnd*   wnd);
	void DestroyDisplayWnd();
	//���Ʊ�����ɫ
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


