This program attempts to detect repeated shapes in images, using segmentation and hierarchical agglomerative clustering of image segments. The program was developed in the hope that this would be a useful adjunct to developing content based image recognition.

As a more practical matter, the code can create attractive image transformations.

More information on the original project, including samples, is located at
[my website](http://www.tobydietrich.com/research/detecting-repeated-shapes-in-images/).

The abstract follows:

An important visual capability is the ability to quickly detect the presence of multiple similar shapes in an image. We seek a fast method to either determine that no repeated objects are in a image, or to determine that there are repeated objects and identify them. We term this the perceptual clustering property. We segment an image and present a heuristic for the similarity between image segments, and then cluster segments. For segmentation we use the fast MST-based image segmentation presented by [Felzenszwalb & Huttenlocher](http://people.cs.uchicago.edu/~pff/segment/). We define a metric space for segments based on simple per-segment measures including color, size, rough shape etc. Over this space we cluster using a modified hierarchical agglomerative clustering (HAC) algorithm based on shortest distance between clusters. The running time is dominated by the segmentation algorithm, which is itself quite fast; run times are on the order of one second for 240×320 images. We find that our clustering does indeed capture the perceptual clustering property for a wide variety of images.

The code is in C. Originally written in 2004, I am updating the code to be based on Felzenszwalb's new code release of December 2006. The initial repository tag is of Felzenszwalb's segmentation code.