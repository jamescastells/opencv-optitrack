#include "opencv2/opencv.hpp"
#include <iostream>
#include <thread>         // std::thread
#include <conio.h>
#include "supportcode.h"       //== Boiler-plate code for application window init ===---
#include "cameralibrary.h"     //== Camera Library header file ======================---
#include <windows.h>

bool	    keys[256];		            // Array Used For Scanning Keyboard

using namespace std;
using namespace cv;
using namespace CameraLibrary;

int capture(int device, int *termino, int  x, int y){

	Camera *camera = CameraManager::X().GetCameraBySerial(device);

	//printf("%d", device);

	if (camera == 0)
	{
		printf("%d: Camara invalida!!!\n",device);
		return 1;
	}

	//std::string s = "D:\\Videos\\" + std::to_string(device) + ".avi";

	std::string s;

	if (x == 0) s = "D:\\Videos\\izquierda.avi";
	else if (x == 700) s= "D:\\Videos\\centro.avi";
	else if (x == 1400) s="D:\\Videos\\derecho.avi";

	int numero;

	if (x == 0) numero = 1;
	if (x == 700) numero = 2;
	if (x == 1400) numero = 3;
	
	char const *pchar = s.c_str();  //use char const* as target type

	cv::namedWindow(pchar);
	cv::moveWindow(pchar, x, y);

	//== MJPEG Mode ==--

	camera->SetVideoType(Core::MJPEGMode);  //== Select MJPEG Video ===================---
	camera->SetAEC(true);             //== Enable Automatic Exposure Control ====---
	camera->SetAGC(true);             //== Enable Automatic Gain Control ========---

	camera->SetNumeric(true, numero);

	//== Start camera output ==--

	camera->Start();

	camera->SetTextOverlay(false);

	int camWidth = camera->Width();
	int camHeight = camera->Height();

	Mat matFrame(Size(camWidth, camHeight), CV_8UC3);
	const int BACKBUFFER_BITSPERPIXEL = 24;

	//namedWindow("Track", CV_WINDOW_AUTOSIZE);
	VideoWriter video(pchar, CV_FOURCC('M', 'J', 'P', 'G'), 60, Size(camWidth, camHeight), true);
	
	while (1)
	{
		Frame *frame = camera->GetFrame();

		if (frame) {

			//printf("%d", matFrame.step);

			frame->Rasterize(camWidth, camHeight, matFrame.step, BACKBUFFER_BITSPERPIXEL, matFrame.data);

			video.write(matFrame);

			//imshow("Track", matFrame);
			imshow(pchar, matFrame);

			int key = waitKey(1);
				
			if (key == 27)
				*termino = 1;


		
			//printf("Grabando camara %d\n",device);

			frame->Release();
		}

		if (*termino == 1){
			camera->Release();
			return 0;
		}

	}
	return 0;
}

int terminar(int *termino){
	while (1){
		if (*termino == 1){
			Sleep(1500);
			return 0;
		}
	}
	return 0;
}

int main(){

	CameraLibrary_EnableDevelopment();

	//== Initialize Camera SDK ==--

	CameraLibrary::CameraManager::X().WaitForInitialization();;

	//== At this point the Camera SDK is actively looking for all connected cameras and will initialize
	//== them on it's own.

	//== Now, lets pop a dialog that will persist until there is at least one camera that is initialized
	//== or until canceled.

	Sleep(1000);

	int termino = 0;

	std::thread first(capture, 198687, &termino, 0, 0);
	std::thread first2(capture, 198685, &termino, 700, 0);
	std::thread first3(capture, 198677, &termino, 1400, 0);

	std::thread second(terminar, &termino);
	
	first.join();
	first2.join();
	first3.join();

	second.join();

	return 0;
}