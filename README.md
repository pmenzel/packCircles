# packCircles

version 1.0

Author: Peter Menzel <pmenzel@gmail.com>


## About
packCircles arranges a list of circles, which are denoted by their area,
by consecutively placing each circle externally tangent to two previously placed
circles at the point closest to the midpoint of the plot.
The output is an SVG file.

The program implements the algorithm described in the paper:
"Visualization of large hierarchical data by circle packing"
by Weixin Wang, Hui Wang, Guozhong Dai, and Hongan Wang
in [Proceedings of the SIGCHI Conference on Human Factors in Computing Systems, 2006, pp. 517-520](https://dl.acm.org/citation.cfm?id=1124851).

Source code is partially based on a implementation of this algorithm
in the [ProtoVis javascript library](http://mbostock.github.io/protovis/).

![Example](https://pmenzel.github.com/packCircles/example.gif)

## Usage
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

The option `-c` can be used to generate colors programmatically for circles with
no assigned colors in the input file. Colors are selected from the HSV color space
by applying [this method](http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/) for iteratively
choosing new values for H using the Golden Ratio, and using fixed values for S (0.5) and V (0.9).

## R package

packCircles is also included in this [R package](https://github.com/mbedward/packcircles) by mbedward.

## License

Copyright (c) 2016 Peter Menzel

The FreeBSD License applies for packCircles.

See the file LICENSE.

