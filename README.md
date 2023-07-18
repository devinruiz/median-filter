# median-filter
Image noise reduction realized by median filtering on remote pynq-z2 fpga

Vitis HLS is used to build this project to implement the median filtering algorithm, and perform simulation, synthesis and IP export.
Use Vivado to integrate HLS exported IP and build the simple image noise reduction application using PYNQ.

Through hardware acceleration, the image noise reduction processing rate is nearly seven times faster than the official median filter library.
