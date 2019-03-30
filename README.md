# fast-mser


## Algorithms and Codes 
All codes are in the directory of basicimg.

OpenCV MSER (CV-MSER): img_opencv3_mser (original implementation https://github.com/opencv/opencv)

CV-MSER+: img_linked_linear_mser

VLFeat MSER (VF-MSER): img_vlfeat_mser (original implementation http://www.vlfeat.org/)

Idiap MSER (ID-MSER): img_idiap_mser (original implementation https://github.com/idiap/mser)

MS-MSER: img_ms_mser

PPMS-MSER: img_ppms_mser

Fast MSER: img_fast_mser

Note that for channel parallel algorithms (CPCV-MSER, CPCV-MSER+, CPVF-MSER, CPID-MSER and CPMS-MSER), we can use the following codes:

img_mser_base* mser = new img_idiap_mser(); // or other mser algorithms

#pragma omp parallel for num_threads(4)

for (i32 i = 0; i < 4; ++i) {

  &nbsp img_multi_msers mser_res;

  &nbsp mser->extract(mser_res, srcs[i]);

}

## Build

Requires:
OpenCV 3.1

Visual Studio 2012 (Release configuration, X64 platform)


If you have any questions about how to build this project, please tell me in 'Issues'.
Thank you very much!

