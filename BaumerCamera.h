#pragma once

#include "bgapi2_genicam/bgapi2_genicam.hpp"
#include <opencv2/opencv.hpp>

using namespace BGAPI2;

class BaumerCamera
{
private:
	BGAPI2::SystemList *systemList = NULL;
	BGAPI2::System * pSystem = NULL;
	BGAPI2::String sSystemID;

	BGAPI2::InterfaceList *interfaceList = NULL;
	BGAPI2::Interface * pInterface = NULL;
	BGAPI2::String sInterfaceID;

	BGAPI2::DeviceList *deviceList = NULL;
	BGAPI2::Device * pDevice = NULL;
	BGAPI2::String sDeviceID;

	BGAPI2::DataStreamList *datastreamList = NULL;
	BGAPI2::DataStream * pDataStream = NULL;
	BGAPI2::String sDataStreamID;

	BGAPI2::BufferList *bufferList = NULL;
	BGAPI2::Buffer * pBuffer = NULL;
	BGAPI2::String sBufferID;

public:
	void CameraStart();
	//cv::Mat GetImageFromCamera();
	void GetImageFromCamera();
	void CameraStop();

};