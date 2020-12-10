# PCCamera_Demo 
Windows with UVC camera, you can preview, change resolution, get and change camera parameters

# Environment
Windows + VS2019
# Install dependency libraries
Add OpenCV4.1.0 library
# Use
In the project property page,Add the "include" folder in the opencv library to the include directory of its VC + + column,the "VC15/lib" folder in opencv library is added to the Library Directory of its VC + + column。And add "opencv_world410d.lib"(If it's in Release mode,add "opencv_world410.lib" ) add the additional dependencies entered in the property page linker bar.Finally, the "opencv_world410d.dll" (If it's in Release mode,add "opencv_world410.dll" ) under the folder "vc15\bin" in the OpenCV library is placed in the generated directory(Debug Directory or Release Directory).
# API
|  Interface | Function  |
| :------------: | :------------: |
| Enum_Devices()  |  Enumeration camera devices |
|  Get_Cameras() | Gets the cameras name and resolutions  |
| Get_Camera_Filters  | Gets the device Filter interface|
| Get_Camera_Resolutions(IBaseFilter* pBaseFilter)  |  Gets camera support resolutions   |
| Get_Camera_Range(IBaseFilter* pBaseFilter,tagCameraControlProperty CameraControl, tagVideoProcAmpProperty VideoProcAmp,bool Use_IAMCameraControl = true, bool Use_IAMVideoProcAmp = true  |  Gets camera's properties |
| VideoCapture::open(int index, int apiPreference = CAP_ANY)  |  Opens the capture device for the specified index |
| VideoCapture::isOpened() |  Returns true if video capturing has been initialized already. |
| VideoCapture::grab()  |  Grabs the next frame from video file or capturing device. |
| VideoCapture::retrieve(Mat& image, int channel=0) |  Decodes and returns the grabbed video frame. |
| VideoCapture::read(Mat& image)  |  Grabs, decodes and returns the next video frame. |
| VideoCapture::get(int propId)  | Returns the specified VideoCapture property  |
| VideoCapture::set(int propId, double value) | Sets a property in the VideoCapture |
| imread(const string& filename, int flags=1 )  | Loads an image from a file.  |
|  mwrite(const string& filename, InputArray image, const vector<int>& params=vector<int>() ) | Saves an image to a specified file.  |
|  VideoCapture::release() | Closes video file or capturing device.  |
# OpenCV支持相机控制项
|  propId | propName  |  Function |
| :------------: | :------------: | :------------: |
| 3 |  CAP_PROP_FRAME_WIDTH  | Width of the frames in the video stream. |
| 4 |  CAP_PROP_FRAME_Height |  Height of the frames in the video stream. |
| 5 |  CAP_PROP_FPS     | Frame rate.  |
| 6 | CAP_PROP_FOURCC  |  4-character code of codec. see VideoWriter::fourcc  |
| 10 | CAP_PROP_BRIGHTNESS  |  Brightness of the image (only for those cameras that support) |
| 11 |  CAP_PROP_CONTRAST  |  Contrast of the image (only for cameras) |
| 12 | CAP_PROP_SATURATION  |   Saturation of the image (only for cameras) |
| 13 | CAP_PROP_HUE      | Hue of the image (only for cameras)  |
| 14 |  CAP_PROP_GAIN   |  Gain of the image (only for those cameras that support) |
| 15 |   CAP_PROP_EXPOSURE |  Exposure (only for those cameras that support) |
| 20 | CAP_PROP_SHARPNESS  |  Sharpness of the image (only for cameras)  |
| 21 | CAP_PROP_AUTO_EXPOSURE  | DC1394: exposure control done by camera, user can adjust reference level using this feature  |
| 22 | CAP_PROP_GAMMA  |  Gamma of the image (only for cameras)   |
| 27 | CAP_PROP_ZOOM  | Zoom  |
| 28 |  CAP_PROP_FOCUS | Focus (only for those cameras that support)  |
| 33 |  CAP_PROP_PAN  | Pan  |
| 34 |  CAP_PROP_TILT    |  Title |
| 35 | CAP_PROP_ROLL      |  Roll |
| 36 | CAP_PROP_IRIS     | Iris  |
| 37 |  CAP_PROP_SETTINGS  | Pop up video/camera filter dialog (note: only supported by DSHOW backend currently. The property value is ignored  |
| 39 |  CAP_PROP_AUTOFOCUS |  Auto Focus |
| 44 | CAP_PROP_AUTO_WB  | white-balance color temperature  |
  
**Note**: When querying a property that is not supported by the backend used by the VideoCapture class, value 0 is returned.
# Reference
This example uses the OpenCV library to call the camera, this call part of the OpenCV API,details please visit https://docs.opencv.org /4.1.0 .
# Contact us
Email:

software@cvgc.cn

The official website:

http://www.cvgc.cn/

http://www.tipscope.com/

https://www.tinyscope.com/

