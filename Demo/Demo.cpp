// Demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "opencv2/opencv.hpp"
#include "windows.h"
#include "dshow.h"
#include <iostream>
#include <locale>
#include <vector>
#include <Ks.h>             // Required by KsMedia.h
#include <KsMedia.h>        // For KSPROPERTY_CAMERACONTROL_FLAGS_*

#pragma comment(lib, "Strmiids.lib")

using namespace cv;
using namespace std;

#define BLUE    0x0001
#define GREEN   0x0002
#define RED     0x0004
#define GRAY    0x0007

//设置字体颜色
static void setcolor(unsigned int color)
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon, color | FOREGROUND_INTENSITY);
}

struct ControlInfo {
	long min;
	long max;
	long step;
	long def;
	long flags;
};

/*
 * Print information about a control in an easily readable fashion.
 */
void print_control_info(ControlInfo* info)
{
	char flags[32] = "";

	if (info->flags & KSPROPERTY_CAMERACONTROL_FLAGS_AUTO)
	{
		strcat_s(flags, sizeof(flags), "AUTO | ");
	}
	else if (info->flags & KSPROPERTY_CAMERACONTROL_FLAGS_MANUAL)
	{
		strcat_s(flags, sizeof(flags), "MANUAL | ");
	}

	if (info->flags & KSPROPERTY_CAMERACONTROL_FLAGS_RELATIVE)
	{
		strcat_s(flags, sizeof(flags), "RELATIVE");
	}
	else
	{
		strcat_s(flags, sizeof(flags), "ABSOLUTE");
	}

	printf(
		"        min:   %d\n"
		"        max:   %d\n"
		"        step:  %d\n"
		"        def:   %d\n"
		"        flags: 0x%08X (%s)\n",
		info->min, info->max, info->step, info->def, info->flags, flags
	);
}

/*
 * get a camera's properties
 *
 * See also:
 *
 * IAMCameraControl Interface
 *     http://msdn2.microsoft.com/en-us/library/ms783833.aspx
 * PROPSETID_VIDCAP_CAMERACONTROL
 *     http://msdn2.microsoft.com/en-us/library/aa510754.aspx
 */
HRESULT GetCameraRange(IBaseFilter* pBaseFilter,tagCameraControlProperty CameraControl, tagVideoProcAmpProperty VideoProcAmp,
	bool Use_IAMCameraControl = true, bool Use_IAMVideoProcAmp = true)
{
	HRESULT hr = 0;
	IAMCameraControl* pCameraControl=NULL;
	IAMVideoProcAmp* pVideoProcAmp=NULL;
	ControlInfo CameraControlInfo = { 0 };
	ControlInfo VideoProcAmpInfo = { 0 };
	long value = 0, flags = 0;

	//setcolor(RED);
	printf("Get a Camera's Properties\n");
	if (Use_IAMCameraControl)
	{
		// Get a pointer to the IAMCameraControl interface used to control the camera
		hr = pBaseFilter->QueryInterface(IID_IAMCameraControl, (void**)&pCameraControl);
		if (hr != S_OK)
		{
			fprintf(stderr, "ERROR: Unable to access IAMCameraControl interface.\n");
			return hr;
		}

		// Retrieve information about the Exposure and Focus controls 
		hr = pCameraControl->GetRange(CameraControl, &CameraControlInfo.min, &CameraControlInfo.max, &CameraControlInfo.step, &CameraControlInfo.def, &CameraControlInfo.flags);
		if (hr != S_OK)
		{
			fprintf(stderr, "ERROR: Unable to retrieve CameraControl property information.\n");
			return hr;
		}
		printf("       %d control:\n", CameraControl);
		print_control_info(&CameraControlInfo);

	}

	if (Use_IAMVideoProcAmp)
	{
		// Get a pointer to the IAMVideoProcAmp interface used to control the camera
		hr = pBaseFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVideoProcAmp);
		if (hr != S_OK)
		{
			fprintf(stderr, "ERROR: Unable to access IAMVideoProcAmp interface.\n");
			return hr;
		}

		// Retrieve information about the Brightness controls 
		hr = pVideoProcAmp->GetRange(VideoProcAmp, &VideoProcAmpInfo.min, &VideoProcAmpInfo.max, &VideoProcAmpInfo.step, &VideoProcAmpInfo.def, &VideoProcAmpInfo.flags);
		if (hr != S_OK)
		{
			fprintf(stderr, "ERROR: Unable to retrieve VideoProcAmp property information.\n");
			return hr;
		}
		printf("       %d control:\n", VideoProcAmp);
		print_control_info(&VideoProcAmpInfo);
	}
	
	return S_OK;
}

void _FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		// pUnk should not be used.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

//Gets camera support resolution
vector<SIZE> CamCaps(IBaseFilter* pBaseFilter)
{
	vector<SIZE> resolutions;
	//vector<SIZE> modes;
	HRESULT hr = 0;
	vector<IPin*> pins;
	IEnumPins* EnumPins;
	pBaseFilter->EnumPins(&EnumPins);
	pins.clear();
	for (;;)
	{
		IPin* pin;
		hr = EnumPins->Next(1, &pin, NULL);
		if (hr != S_OK) { break; }
		pins.push_back(pin);
		pin->Release();
	}
	EnumPins->Release();

	printf("Device pins number: %zd\n", pins.size());

	PIN_INFO pInfo;
	for (int i = 0; i < pins.size(); i++)
	{
		pins[i]->QueryPinInfo(&pInfo);

		setcolor(RED);

		if (pInfo.dir == 0)
		{
			wprintf(L"Pin name: %s \n", pInfo.achName);
		}

		if (pInfo.dir == 1)
		{
			wprintf(L"Pin name: %s \n", pInfo.achName);
		}

		IEnumMediaTypes* emt = NULL;
		pins[i]->EnumMediaTypes(&emt);

		AM_MEDIA_TYPE* pmt;

		setcolor(GRAY);
		wprintf(L"Avialable resolutions.\n", pInfo.achName);
		for (;;)
		{
			hr = emt->Next(1, &pmt, NULL);
			if (hr != S_OK) { break; }

			if ((pmt->formattype == FORMAT_VideoInfo) &&
				//(pmt->subtype == MEDIASUBTYPE_RGB24) &&
				(pmt->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
				(pmt->pbFormat != NULL))
			{
				VIDEOINFOHEADER* pVIH = (VIDEOINFOHEADER*)pmt->pbFormat;
				SIZE s;
				// Get frame size
				s.cy = pVIH->bmiHeader.biHeight;
				s.cx = pVIH->bmiHeader.biWidth;
				// BitRate
				unsigned int bitrate = pVIH->dwBitRate;
				// Bits per pixel
				unsigned int bitcount = pVIH->bmiHeader.biBitCount;
				REFERENCE_TIME t = pVIH->AvgTimePerFrame; // blocks (100ns) per frame
				int FPS = floor(10000000.0 / static_cast<double>(t));
				//printf("Size: x=%d\ty=%d\tFPS: %d\t bitrate: %ld\tbit/pixel:%ld\n", s.cx, s.cy, FPS, bitrate, bitcount);

				//Get support for 24-bit depth resolution
				if (bitcount == 24)
				{
					printf("Size: x=%d\ty=%d\tFPS: %d\t \n", s.cx, s.cy, FPS);
					resolutions.push_back(s);
				}
			}
			_FreeMediaType(*pmt);
		}

		//modes.clear();
		emt->Release();
	}

	pins.clear();

	return resolutions;
}

/*
* Do something with the filter. In this sample we just test the pan/tilt properties.
*/
vector<SIZE> process_filter(IBaseFilter* pBaseFilter)
{
	return CamCaps(pBaseFilter);
}

//Enumeration camera devices
IEnumMoniker* Enum_Devices()
{
	ICreateDevEnum* pDevEnum = NULL;
	IEnumMoniker* pEnum = NULL;
	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
		reinterpret_cast<void**>(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the video capture category.
		hr = pDevEnum->CreateClassEnumerator(
			CLSID_VideoInputDeviceCategory,
			&pEnum, 0);
		if (hr == S_OK)
		{
			pDevEnum->Release();
			pDevEnum = NULL;
			return pEnum;
		}
		pDevEnum->Release();
		pDevEnum = NULL;
	}
}

//Gets the device Filter interface
vector<IBaseFilter*> Get_Camera_Filters()
{
	vector<IBaseFilter*> filters;
	IEnumMoniker* pEnum = Enum_Devices();
	IMoniker* pMoniker = NULL;
	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		// To create an instance of the filter, do the following:
		IBaseFilter* pFilter;
		HRESULT hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
			(void**)&pFilter);

		if (FAILED(hr)) {
			pMoniker->Release();
			continue;  // Skip this one, maybe the next one will work.
		}
		filters.push_back(pFilter);

		pMoniker->Release();
		pMoniker = NULL;
	}
	pEnum->Release();
	pEnum = NULL;

	return filters;
}

//Gets the camera name and resolution
map<string, vector<SIZE>> GetCameras()
{
	std::map<string, vector<SIZE>> list;
	int deviceCounter = 0;
	HRESULT hr;

	IEnumMoniker* pEnum = Enum_Devices();
	if (pEnum==NULL)
	{
		printf("SETUP: Capture Devices Not Found\n");
		return list;
	}
	printf("SETUP: Looking For Capture Devices\n");
	IMoniker* pMoniker = NULL;

	while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {

		IPropertyBag* pPropBag;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
			(void**)(&pPropBag));

		if (FAILED(hr)) {
			pMoniker->Release();
			continue;  // Skip this one, maybe the next one will work.
		}
		// Find the description or friendly name.
		VARIANT varName;
		VariantInit(&varName);
		hr = pPropBag->Read(L"Description", &varName, 0);

		if (FAILED(hr)) hr = pPropBag->Read(L"FriendlyName", &varName, 0);

		if (SUCCEEDED(hr))
		{
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);

			int count = 0;
			char tmp[255] = { 0 };
			while (varName.bstrVal[count] != 0x00 && count < 255)
			{
				tmp[count] = (char)varName.bstrVal[count];
				count++;
			}

			// To create an instance of the filter, do the following:
			IBaseFilter* pFilter;
			hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter,
				(void**)&pFilter);

			if (FAILED(hr)) {
				pMoniker->Release();
				continue;  // Skip this one, maybe the next one will work.
			}

			//Gets the camera property range
			//GetCameraRange(pFilter);

			//Get camera resolutions
			vector<SIZE> resolutions = process_filter(pFilter);

			//Make sure the camera name key is unique
			if (list.count(tmp) == 1)
			{
				std::string tmps = tmp;
				string tmp_1 = tmps.append("_1").c_str();
				if (list.count(tmp_1) == 1)
				{
					string tmp_2 = tmps.append("_2").c_str();
					list.insert(std::pair <string, vector<SIZE>>(tmp_2, resolutions));
				}
				else
				{
					list.insert(std::pair <string, vector<SIZE>>(tmp_1, resolutions));
				}
			}
			else
			{
				list.insert(std::pair <string, vector<SIZE>>(tmp, resolutions));
			}
		}
		pPropBag->Release();
		pPropBag = NULL;

		pMoniker->Release();
		pMoniker = NULL;

		deviceCounter++;
	}

	pEnum->Release();
	pEnum = NULL;

	return list;
}

int main()
{
	map<string, vector<SIZE>> cameras_resolutions = GetCameras();
	CoUninitialize();

	vector<string> cameras;
	map<string, vector<SIZE>>::iterator it;
	for (it = cameras_resolutions.begin(); it != cameras_resolutions.end(); it++) {
		cameras.push_back(it->first);
	}

	//Gets the specified camera name index
	int index = -1;
	for (int i = 0; i < cameras.size(); i++)
	{
		cameras[i].erase(0, cameras[i].find_first_not_of(" "));
		cameras[i].erase(cameras[i].find_last_not_of(" ") + 1);
		if (cameras[i] == "cvgc_camera")
		{
			index = i;
			break;
		}
	}
	if (index==-1)
	{
		printf("The Specified Device Was Not Found\n");
		return -1;
	}	

	vector<IBaseFilter*> filters = Get_Camera_Filters();
	//GetCameraRange(filters[index], CameraControl_Focus, VideoProcAmp_Brightness,true,true);
	GetCameraRange(filters[index], CameraControl_Focus, VideoProcAmpProperty(NULL),true,false);

	//Get the video stream
	namedWindow("Win10x64", WINDOW_NORMAL);
	VideoCapture capture;
	//Use Directshow to open the first camera device.    
	//capture.open(0, CAP_DSHOW);
	capture.open(index, CAP_DSHOW);

	//Resolution setting
	bool a = capture.set(CAP_PROP_FRAME_WIDTH, 1920);
	bool b = capture.set(CAP_PROP_FRAME_HEIGHT, 1080);

	//Camera focus Settings
	bool c = capture.set(CAP_PROP_FOCUS, 100);
	bool d = capture.set(CAP_PROP_AUTOFOCUS, 1);

	//Coding format
	bool e = capture.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));

	if (!capture.isOpened())
	{
		return -1;
	}

	//Record video
	VideoWriter writer("1.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30.0, Size(1920, 1080));

	int count = 0;

	while (true)
	{
		Mat camera;
		//Read a frame of image
		capture >> camera;
		//capture.read(camera);
		writer << camera;
		if (camera.empty())
		{
			continue;
		}
		imshow("Win10x64", camera);

		count++;

		if (count == 600)
		{
			//Gets 600 frames to close recording
			writer.release();
		}
	
		//Press the Esc key to exit the program
		if (waitKey(10) == 27)  
		{
			break;
		}
		//Press the a key
		if (waitKey(10) == 97)
		{
			//Bring up the Directshow camera properties bar
			capture.set(CAP_PROP_SETTINGS, 0);
		}
	}

	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
