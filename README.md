#packCircles

version 1.0

Author:  
Peter Menzel <pmenzel@gmail.com>   


##About
packCircles arranges a list of circles, which are denoted by their radii,
by consecutively placing each circle externally tangent to two previously placed
circles avoiding overlaps.
The output is an SVG file.

The program implements the algorithm described in the paper:
"Visualization of large hierarchical data by circle packing" 
by Weixin Wang, Hui Wang, Guozhong Dai, and Hongan Wang
in [Proceedings of the SIGCHI Conference on Human Factors in Computing Systems, 2006, pp. 517-520](https://dl.acm.org/citation.cfm?id=1124851).

Source code is partially based on a implementation of this algorithm
in the [ProtoVis javascript library](http://mbostock.github.io/protovis/).

![Example](https://pmenzel.github.com/packCircles/example.png)

##Usage 
Download:
```
git clone https://github.com/pmenzel/packCircles.git
```
Compile:
```
gcc -O3 -o packCircles packCircles.c -lm
```
Run:
```
./packCircles -i INPUTFILE > output.svg
```

The input file has at least one unsigned long number per line that denotes the
area of the circle.  Two additional tab-separated columns can be used for the
fill color and the name of each circle, which will be used in the title element
for each circle.  Colors need to be valid CSS color names, for example: `red`,
`#12AB56`, or `rgb(123,89,12)`.



###License

Copyright (c) 2016 Peter Menzel 

The FreeBSD License applies for packCircles.
See file LICENSE.

