#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include "bgapi2_genicam/bgapi2_genicam.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "BaumerCamera.h"

using namespace BGAPI2;

void BaumerCamera::CameraStart()
{
	try
	{
		SystemList *systemList = SystemList::GetInstance();
		systemList->Refresh();

		//OPEN THE FIRST SYSTEM IN THE LIST WITH A CAMERA CONNECTED
		for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++)
		{
			sysIterator->second->Open();
			sSystemID = sysIterator->first;

			interfaceList = sysIterator->second->GetInterfaces();
			interfaceList->Refresh(100); // timeout of 100 msec

			//OPEN THE NEXT INTERFACE IN THE LIST
			for (InterfaceList::iterator ifIterator = interfaceList->begin(); ifIterator != interfaceList->end(); ifIterator++)
			{
				ifIterator->second->Open();
				//search for any camera is connetced to this interface
				deviceList = ifIterator->second->GetDevices();
				deviceList->Refresh(100);
				if (deviceList->size() == 0)
				{
					ifIterator->second->Close();
				}
				else
				{
					sInterfaceID = ifIterator->first;
					break;
				}

			}
			//if a camera is connected to the system interface then leave the system loop
			if (sInterfaceID != "")
			{
				break;
			}
		}

		if (sSystemID == "")
		{
			std::cout << " No System found " << std::endl;
			BGAPI2::SystemList::ReleaseInstance();
			return;
		}
		else
		{
			pSystem = (*systemList)[sSystemID];
		}


		if (sInterfaceID == "")
		{
			std::cout << " No camera found " << sInterfaceID << std::endl;
			pSystem->Close();
			BGAPI2::SystemList::ReleaseInstance();
			return;
		}
		else
		{
			pInterface = (*interfaceList)[sInterfaceID];
		}

		//COUNTING AVAILABLE CAMERAS
		deviceList = pInterface->GetDevices();
		deviceList->Refresh(100);

		//OPEN THE FIRST CAMERA IN THE LIST

		DeviceList::iterator devIterator = deviceList->begin();
		devIterator->second->Open();
		sDeviceID = devIterator->first;

		if (sDeviceID == "")
		{
			std::cout << " No Device found " << sDeviceID << std::endl;
			pInterface->Close();
			pSystem->Close();
			BGAPI2::SystemList::ReleaseInstance();
			return;
		}
		else
		{
			pDevice = (*deviceList)[sDeviceID];
		}

		//SET TRIGGER MODE OFF (FreeRun)
		pDevice->GetRemoteNode("TriggerMode")->SetString("Off");
		//pDevice->GetRemoteNode("PixelFormat")->SetString("BGR8");

		//COUNTING AVAILABLE DATASTREAMS
		datastreamList = pDevice->GetDataStreams();
		datastreamList->Refresh();

		//OPEN THE FIRST DATASTREAM IN THE LIST
		DataStreamList::iterator dstIterator = datastreamList->begin();
		dstIterator->second->Open();
		sDataStreamID = dstIterator->first;

		if (sDataStreamID == "")
		{
			std::cout << " No DataStream found " << sDataStreamID << std::endl;
			pDevice->Close();
			pInterface->Close();
			pSystem->Close();
			BGAPI2::SystemList::ReleaseInstance();
			return;
		}
		else
		{
			pDataStream = (*datastreamList)[sDataStreamID];
		}

		//BufferList
		bufferList = pDataStream->GetBufferList();

		// 4 buffers using internal buffer mode
		for (int i = 0; i < 4; i++)
		{
			pBuffer = new BGAPI2::Buffer();
			bufferList->Add(pBuffer);
		}

		for (BufferList::iterator bufIterator = bufferList->begin(); bufIterator != bufferList->end(); bufIterator++)
		{
			bufIterator->second->QueueBuffer();
		}

		//START DataStream acquisition
		pDataStream->StartAcquisitionContinuous();

		//START CAMERA
		pDevice->GetRemoteNode("AcquisitionStart")->Execute();
	}
	catch (BGAPI2::Exceptions::IException& ex)
	{
		std::cout << "Error in function: " << ex.GetFunctionName() << std::endl << "Error description: " << ex.GetErrorDescription() << std::endl << std::endl;
	}
}

//cv::Mat BaumerCamera::GetImageFromCamera()
void BaumerCamera::GetImageFromCamera()
{
	BGAPI2::Buffer * pBufferFilled = NULL;
	cv::Mat imClone;
	try
	{
		pBufferFilled = pDataStream->GetFilledBuffer(1000);
		if (pBufferFilled == NULL) { std::cout << "Error: Buffer Timeout after 1000 msec" << std::endl; }
		else if (pBufferFilled->GetIsIncomplete() == true) { std::cout << "Error: Image is incomplete" << std::endl; pBufferFilled->QueueBuffer(); }
		else
		{
			std::cout << " Image " << pBufferFilled->GetFrameID() << " received. " << std::endl;

			std::cout << pBufferFilled->GetPixelFormat() << std::endl;
			if (pBufferFilled->GetPixelFormat() == "BGR8")
			{
				cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_8UC3, (char *)pBufferFilled->GetMemPtr());
				imClone = imOriginal->clone();
				delete imOriginal;
			}
			else if (pBufferFilled->GetPixelFormat() == "BayerRG8")
			{
				cv::Mat* imOriginal = new cv::Mat((int)pBufferFilled->GetHeight(), (int)pBufferFilled->GetWidth(), CV_8UC1, (char *)pBufferFilled->GetMemPtr());
				cv::Mat* imTransformBGR8 = new cv::Mat((int)pBufferFilled->GetHeight(),	(int)pBufferFilled->GetWidth(),	CV_8UC3);
				cv::cvtColor(*imOriginal, *imTransformBGR8, CV_BayerBG2BGR); //to BGR
				imClone = imTransformBGR8->clone();
				delete imOriginal;
				delete imTransformBGR8;
			}

			pBufferFilled->QueueBuffer();
		}
	}
	catch (BGAPI2::Exceptions::IException& ex)
	{
		std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
		std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
		std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
	}

	cv::namedWindow("res", cv::WINDOW_AUTOSIZE);
	cv::imshow("res", imClone);
	cv::waitKey(30);
	return;
	//return imClone;
}

void BaumerCamera::CameraStop()
{
	//STOP CAMERA
	try
	{
		//SEARCH FOR 'AcquisitionAbort'
		if (pDevice->GetRemoteNodeList()->GetNodePresent("AcquisitionAbort"))
		{
			pDevice->GetRemoteNode("AcquisitionAbort")->Execute();
		}

		pDevice->GetRemoteNode("AcquisitionStop")->Execute();

		BGAPI2::String sExposureNodeName = "";
		if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTime")) {
			sExposureNodeName = "ExposureTime";
		}
		else if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTimeAbs")) {
			sExposureNodeName = "ExposureTimeAbs";
		}

		//STOP DataStream acquisition
		pDataStream->StopAcquisition();
		bufferList->DiscardAllBuffers();

		//Release buffers
		while (bufferList->size() > 0)
		{
			pBuffer = bufferList->begin()->second;
			bufferList->RevokeBuffer(pBuffer);
			delete pBuffer;
		}

		pDataStream->Close();
		pDevice->Close();
		pInterface->Close();
		pSystem->Close();
		BGAPI2::SystemList::ReleaseInstance();
	}
	catch (BGAPI2::Exceptions::IException& ex)
	{
		std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
		std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
		std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
	}
}