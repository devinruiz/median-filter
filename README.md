# median-filter
Image noise reduction realized by median filtering on remote pynq-z2 fpga

Vitis HLS is used to build this project to implement the median filtering algorithm, and perform simulation, synthesis and IP export.
Use Vivado to integrate HLS exported IP and build the simple image noise reduction application using PYNQ.

Through hardware acceleration, the image noise reduction processing rate is nearly seven times faster than the official median filter library.

The src folder includes C-based source files (design specification), testbenchfiles (design test) and related files. 
In the overlay folder is .bit and .hwh file. 
The jupyter folder contains the jupyter notebook and related documents used to display the project. 
Screenshots of some key results are placed in the report folder.
