
<h1>Image processing algorithms collection written in C++</h1>

![image](https://github.com/user-attachments/assets/0b0a9739-f7d8-4509-9d10-4123662c35ba)
^ Otsu with 2 classes visible in the upper image

This repo serves to deepen my understanding of image processing. 
Resources used:

UI
* Dear ImGui: https://github.com/ocornut/imgui
* ImGui file dialog: https://github.com/aiekick/ImGuiFileDialog
* ImPlot: https://github.com/epezent/implot
* stb: https://github.com/nothings/stb

Algorithms
* Moeslund T. B. (2012). *Introduction to Video and Image Processing.* Springer. https://link.springer.com/book/10.1007/978-1-4471-2503-7
* Gonzalez R. C., & Woods R. E. (2009). *Digital Image Processing.* Pearson Education. https://books.google.com/books/about/Digital_Image_Processing.html?id=a62xQ2r_f8wC&source=kp_book_description&redir_esc=y

<h1>In this repo:</h1>

<h2>General</h2>

An ImGui UI for uploading and displaying local images, and applying a wide variety of image processing algorithms on said image (and also displaying the processed image).

<h2>An implementation of a C++ class for storing image data</h2>

* Able to store RGB, grayscale and HSV images
* Conversions between these three image types
* Image color inversion
* Image padding (zero padding, mirror padding)
* Image histogram display

<h2>Algorithms</h2>

* All of these algorithms can be found in the file "Operations.h"

<h3>Pointwise operations</h3>

* Brightness, contrast modification
* Logarithmic operator
* Gamma operator
* Piecewise linear contrast
* Histogram stretching
* Histogram equalization

auxiliary:

* Look up table calculation function

<h3>Thresholding algorithms</h3>


* Mid range thresholding
* Mean thresholding
* Quantile thresholding
* Intermeans (ISODATA) thresholding
* Otsu thresholding
* Otsu thresholding with 2 classes
* Minimum error thresholding
* Maximum entropy thresholding
* Triangle method thresholding

auxiliary:

* thresholding function

<h3>Low pass filters</h3>

* Mean filter
* Gaussian filter

auxiliary

* Image convolution function
* Separable convolution function

<h3>High pass filters</h3>

* Prewitt gradient

<h3>Morphological operations</h3>

* Grayscale erosion
* Grayscale dilation
* Grayscale opening
* Grayscale closing

<h3>Geometric transformations</h3>

* Scaling
