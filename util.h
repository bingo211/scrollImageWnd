#pragma once
#include "stdafx.h"
#include <string>
using namespace std;

class Utility
{
public:
	Utility(void){;}
	~Utility(void){;}
	//��ȡ����·��
	static bool GetProcessPath(wchar_t* pszPath,int nBufLen)
	{
		if (pszPath == 0)
			return false;

		if(GetModuleFileName(0, pszPath, nBufLen) == 0)
			return false;

		wchar_t* pszFind = wcsrchr(pszPath, L'\\');
		*pszFind = 0;
		lstrcat(pszPath, L"\\");
		return true;
	}
	//���ͼ��·��
	static bool GetImageFilePath(wchar_t* pszImageFilePath)
	{
		if(!GetProcessPath(pszImageFilePath, 256))
			return false;
		lstrcat(pszImageFilePath, L"image\\");
		return true;
	}

	static bool GetConfigFilePath(wchar_t* pszImageFilePath)
	{
		if(!GetProcessPath(pszImageFilePath, MAX_PATH))
			return false;
		lstrcat(pszImageFilePath, L"config.txt");
		return true;
	}
};
namespace VRVIEW
{
	//��������
#define CREATE_WINDOW(wndPointer, wndClass, wndID)	if((wndPointer)==nullptr)  \
	{                                                                           \
	(wndPointer)=new (wndClass);	                                    \
	if ((wndPointer) != nullptr)											\
	{																	\
	if ((wndPointer)->Create(CRect(0,0,0,0), this, wndID))			\
	{																\
	wndPointer->SetWindowShow(FALSE);	                            \
}                                                               \
}                                                                   \
}

	//���ٴ���
#define DESTROY_WINDOW(x)												\
	{																		\
	if((x) != NULL)													\
	{																	\
	(x)->DestroyWindow();											\
	delete (x);														\
	(x) = NULL;														\
}																	\
}
}