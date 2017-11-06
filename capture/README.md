# AirCode Capturing 

## Build Requirements
This program has only been tested in the following configuration.

Softwares:
- Windows 10
- Visual Studio 2013 Professional
- OpenCV 2.4.13.0
- FlyCapture2 2.10.3.169 for Point Grey Linear Cameras

Hardwares:
- Mitsubishi PK20 Projector [[link](https://www.amazon.com/Mitsubishi-PK20-PocketProjector/dp/B000I28EGC)]
- PointGrey GrassHopper3 Monochrome Camera [[link](https://www.ptgrey.com/grasshopper3-32-mp-mono-usb3-vision-sony-pregius-imx252)]

For Compiling, first you need to setup the corresponding link library and 
include search path for OpenCV and FlyCapture2. After all the paths have been
correctly specified, the VS project should build. 

The capturing code essentially implements the following paper --
[Fast Separation of Direct and Global Components of a Scene using High Frequency Illumination](http://www1.cs.columbia.edu/CAVE/publications/pdfs/Krishnan_TOG06.pdf),
SIGGRAPH 2006. 
With the following tweaks in the software/hardware setup:
- Instead of regular black and white checkerboard, we use red to replace white.
- Cross polarizers are used to reduce the specular highlight impacts.


## Capture Example

The following animation shows a typical capturing session.

![](https://media.giphy.com/media/xUOxfdA76vkje4UqcM/giphy.gif)


