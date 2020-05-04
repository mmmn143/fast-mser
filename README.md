# Fast MSER 

## Future Work
- Experiments w.r.t. different threads (1, 2, 4, 8, 16, 32 threads). We have a computer with 32-cores. The OS is Linux. Thus, we need to support our algorithms on Linux;

- Improve the speed-up in MSER recognition, and further reduce the running memory of V1 and V2;

- Compare Fast MSER to several deep learning based scene text methods in execution time aspect.

## News (2020/05/02)
- We support our algorithms on MacOS (however, OpenMP is not supported well in MacOS);

- Fast MSER V1 supports 1, 2, 4, 8, 16 and 32 threads. Fast MSER V2 supports 1, 4 and 32 threads.

## Algorithms and Codes 
All codes are in the directory of basicimg.

OpenCV MSER (CV-MSER): img_opencv3_mser (original implementation https://github.com/opencv/opencv)

CV-MSER+: img_linked_linear_mser

VLFeat MSER (VF-MSER): img_vlfeat_mser (original implementation http://www.vlfeat.org/)

Idiap MSER (ID-MSER): img_idiap_mser (original implementation https://github.com/idiap/mser)

Fast-MSER V1: img_fast_mser_v1 (supporting 1-threads, 2-threads, 4-threads, 8-threads, 16-threads and 32-threads)

Fast MSER V2: img_fast_mser_v2 (supporting 1-threads, 4-threads and 32-threads)

Note that for channel parallel algorithms (CPCV-MSER, CPCV-MSER+, CPVF-MSER, CPID-MSER), we can use the following codes:

img_mser_base* mser = new img_idiap_mser(); // or other mser algorithms

#pragma omp parallel for num_threads(4) // 4 denotes the number of threads

for (i32 i = 0; i < 4; ++i) {

  &nbsp;&nbsp;&nbsp;&nbsp;img_multi_msers mser_res;

  &nbsp;&nbsp;&nbsp;&nbsp;mser->extract(mser_res, srcs[i]);

}

## Examples of Runing an MSER algorithm
You can find the examples of how to run a MSER algorithm in the code: basicimg/test/img_mser_test.cpp 

## Performance w.r.t. Different deltas

Compared to CV-MSER+, the speed-ups of Fast MSER V1 w.r.t. different deltas (from 1 to 5) are 3.5, 3.3, 3.2, 3.1, 3.1, respectively.
Compared to CV-MSER+, the speed-ups of Fast MSER V2 w.r.t. different deltas (from 1 to 5) are 3.2, 3.2, 3.2, 3.1, 3.0, respectively.
Thus, the larger delta, the smaller speed-up.
![image](https://github.com/mmmn143/fast-mser/blob/master/images/delta_text_detection_icdar.png)

## Build

### Windows

#### OpenCV 3.41

We have included the head files and the static library of OpenCV 3.41 in basic_thrid_libs/lib_opencv/341.
The dynamic library of OpenCV 3.41 is in bin/x64/vc11/opencv_world341.zip. You can unzip opencv_world341.zip to get opencv_world341.dll. Note that opencv_world341.dll must be placed under bin/x64/vc11/.

#### Visual Studio 2012 (Release configuration, X64 platform)

Open all_projects.sln and build it, then you can test the comparison MSER algorithms.

### MacOS

#### OpenCV 3

Please do not use OpenCV 4.

#### CLion

Please use CMakeLists.txt.

## FAQ
If you have any questions about how to build this project, please tell me in 'Issues'.
Thank you very much!

