# AirCode Decoding 

## Demo

To run the demo, open MATLAB, change to the decode folder. Run the file demo_decode.m.

On my iMac, the whole running time is ~10 seconds.


## MATLAB version

We only tested with Matlab R2016b on macOS and Windows, with image processing toolbox, computer vision toolbox, and other MATLAB toolboxes installed.


## External Library

The ellipse detector is downloaded from [Jean-Nicolas Ouellet's webpage](http://vision.gel.ulaval.ca/~jouellet/code/DualEllipse/de.html).

Bilateral image filtering is from [Douglas Lanman's MathWorks page](https://www.mathworks.com/matlabcentral/fileexchange/12191-bilateral-filtering?focused=5077384&tab=function).

## Future Improvement

- automatic ROI selection
- more robust image cleaning process