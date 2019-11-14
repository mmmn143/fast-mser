# Fast MSER


## Algorithms and Codes 
All codes are in the directory of basicimg.

OpenCV MSER (CV-MSER): img_opencv3_mser (original implementation https://github.com/opencv/opencv)

CV-MSER+: img_linked_linear_mser

VLFeat MSER (VF-MSER): img_vlfeat_mser (original implementation http://www.vlfeat.org/)

Idiap MSER (ID-MSER): img_idiap_mser (original implementation https://github.com/idiap/mser)

Fast-MSER V1: img_fast_mser_v1

Fast MSER V2: img_fast_mser_v2

Note that for channel parallel algorithms (CPCV-MSER, CPCV-MSER+, CPVF-MSER, CPID-MSER), we can use the following codes:

img_mser_base* mser = new img_idiap_mser(); // or other mser algorithms

#pragma omp parallel for num_threads(4) // 4 denotes the number of threads

for (i32 i = 0; i < 4; ++i) {

  &nbsp;&nbsp;&nbsp;&nbsp;img_multi_msers mser_res;

  &nbsp;&nbsp;&nbsp;&nbsp;mser->extract(mser_res, srcs[i]);

}

## Build

### Requires:

OpenCV 3.41

We have included the head files and the static library of OpenCV 3.41 in basic_thrid_libs/lib_opencv/341.
The dynamic library of OpenCV 3.41 is in bin/x64/vc11/opencv_world341.zip. You can unzip opencv_world341.zip to get opencv_world341.dll. Note that opencv_world341.dll must be placed under bin/x64/vc11/.

### Development environment:
Windows

Visual Studio 2012 (Release configuration, X64 platform)

Open all_projects.sln and build it, then you can test the comparison MSER algorithms.

## FAQ
If you have any questions about how to build this project, please tell me in 'Issues'.
Thank you very much!

