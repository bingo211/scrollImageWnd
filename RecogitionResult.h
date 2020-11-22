/*********************************
��˾��徾���Ϣ
�汾��V1.0
���ߣ�FSB
���ڣ�2016.12.20
���ܣ�
**********************************/
#pragma once
#include <string.h>

/***************************************
* �˵�״̬�����г������۾�������
*����״̬����ȫ������ʻԱ������ʻ
**************************************/
enum  RESULT_MSG:int
{
	VEHICLE_RESULT_MSG = 1,
	PERSON_RESULT_MSG,
	NONE_RESULT_MSG
};
//���Ҽ�����Ϊ32
#define MAX_PENDANT_NUM  32

typedef struct tagPictureData
{
	char* buf;
	int nWidth;
	int nHeight;
	int len;
	struct tagPictureData()
	{
		buf = nullptr;
		len = 0;
		nWidth = 0;
		nHeight = 0;
	}
}pictureData,*lpPictureData;

class BaseRecognitionResult
{
public:
	BaseRecognitionResult(int nMsgType, lpPictureData _imageData)
	{
		m_nType = (RESULT_MSG)nMsgType;

		if (_imageData != nullptr)
		{
			ImageData.buf = new char[_imageData->len];
			memcpy(ImageData.buf, _imageData->buf, _imageData->len); 
			ImageData.len = _imageData->len;
			ImageData.nHeight = _imageData->nHeight;
			ImageData.nWidth = _imageData->nWidth;
		}
	}
	virtual ~BaseRecognitionResult()
	{
		FreeBuffer();
	}
	pictureData ImageData;
	RESULT_MSG m_nType;
private:
	void FreeBuffer()
	{
		if (ImageData.len)
		{
			delete[] ImageData.buf;
			ImageData.buf = nullptr;
		}
	}
};

//��ȫ������ʻԱ������ʻλ
class VehicleResultInfo :public BaseRecognitionResult
{
public:
	VehicleResultInfo(lpPictureData imageData, const char* vehicleName, const char* vehicleNumber, unsigned int nResultInfo) :
		BaseRecognitionResult(RESULT_MSG::VEHICLE_RESULT_MSG, imageData)
	{
		memset(szVehicleName, 0, sizeof(szVehicleName));
		memset(szVehcileNumber,0,12);//��ʼ��

		memcpy(szVehicleName, vehicleName, strlen(vehicleName));
		memcpy(szVehcileNumber, vehicleNumber, strlen(vehicleNumber));
		m_nRecogitionResultBits = nResultInfo;
	}
	virtual ~VehicleResultInfo()
	{
	}
public:
	char szVehicleName[20];

	char szVehcileNumber[12];//���ƺţ���/����  ���ƺţ�

	unsigned int m_nRecogitionResultBits;//������ڵ���չ
};
//�۾������г�������
class PersonResultInfo :public BaseRecognitionResult
{
public:
	PersonResultInfo(lpPictureData imageData, bool bSex, const char* szSexResult, unsigned int nResultInfo) :
		BaseRecognitionResult(RESULT_MSG::PERSON_RESULT_MSG, imageData)
	{
		bPersonSex = bSex;//�Ա�
		memset(szPersonSexResult,0,10);
		memcpy(szPersonSexResult, szSexResult, strlen(szSexResult));
		m_nRecogitionResultBits = nResultInfo;
	}
	virtual ~PersonResultInfo()
	{
	}
public:
	bool bPersonSex;//0 Ϊ��ʿ  1 ΪŮʿ
	char szPersonSexResult[10];
	unsigned int m_nRecogitionResultBits;
};