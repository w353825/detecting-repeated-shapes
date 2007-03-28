/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#ifndef SEGMENT_IMAGE
#define SEGMENT_IMAGE

#include <cstdlib>
#include <image.h>
#include <misc.h>
#include <filter.h>
#include "segment-graph.h"

#define RANDOM_COLOR 0
#define AVERAGE_COLOR 1
#define GREYSCALE_COLOR 2

// random color
rgb random_rgb(){ 
  rgb c;
  double r;
  
  c.r = (uchar)random();
  c.g = (uchar)random();
  c.b = (uchar)random();

  return c;
}

// dissimilarity measure between pixels
static inline float diff(image<float> *r, image<float> *g, image<float> *b,
			 int x1, int y1, int x2, int y2) {
  return sqrt(square(imRef(r, x1, y1)-imRef(r, x2, y2)) +
	      square(imRef(g, x1, y1)-imRef(g, x2, y2)) +
	      square(imRef(b, x1, y1)-imRef(b, x2, y2)));
}

/*
 * Segment an image
 *
 * Returns a color image representing the segmentation.
 *
 * im: image to segment.
 * sigma: to smooth the image.
 * c: constant for treshold function.
 * min_size: minimum component size (enforced by post-processing stage).
 * num_ccs: number of connected components in the segmentation.
 */
image<rgb> *segment_image(image<rgb> *im, float sigma, float c, int min_size, 
    float ck, float cutoff, float proximity, float size, float color, 
    float difference, float eccentricity, float theta, float mean_dist, 
    int output_type, int *num_ccs) {
  int width = im->width();
  int height = im->height();

  image<float> *r = new image<float>(width, height);
  image<float> *g = new image<float>(width, height);
  image<float> *b = new image<float>(width, height);

  // smooth each color channel  
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      imRef(r, x, y) = imRef(im, x, y).r;
      imRef(g, x, y) = imRef(im, x, y).g;
      imRef(b, x, y) = imRef(im, x, y).b;
    }
  }
  image<float> *smooth_r = smooth(r, sigma);
  image<float> *smooth_g = smooth(g, sigma);
  image<float> *smooth_b = smooth(b, sigma);
  delete r;
  delete g;
  delete b;
 
  // build graph
  edge *edges = new edge[width*height*4];
  int num = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (x < width-1) {
	edges[num].a = y * width + x;
	edges[num].b = y * width + (x+1);
	edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y);
	num++;
      }

      if (y < height-1) {
	edges[num].a = y * width + x;
	edges[num].b = (y+1) * width + x;
	edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x, y+1);
	num++;
      }

      if ((x < width-1) && (y < height-1)) {
	edges[num].a = y * width + x;
	edges[num].b = (y+1) * width + (x+1);
	edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y+1);
	num++;
      }

      if ((x < width-1) && (y > 0)) {
	edges[num].a = y * width + x;
	edges[num].b = (y-1) * width + (x+1);
	edges[num].w = diff(smooth_r, smooth_g, smooth_b, x, y, x+1, y-1);
	num++;
      }
    }
  }
  delete smooth_r;
  delete smooth_g;
  delete smooth_b;

  // segment
  universe *u = segment_graph(width*height, num, edges, c);
  
  // post process small components
  for (int i = 0; i < num; i++) {
    int a = u->find(edges[i].a);
    int b = u->find(edges[i].b);
    if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))
      u->join(a, b);
  }
  delete [] edges;
  *num_ccs = u->num_sets();

  image<rgb> *output = new image<rgb>(width, height);

  // pick random colors for each component
  rgb *colors = new rgb[width*height];
  for (int i = 0; i < width*height; i++) {
	  colors[i] = random_rgb();
  }


  // vars for AVERAGE_COLOR
	int *pixel_id = new int[width*height];
  int *pixel_id_to_comp = new int[width*height];
  int *comp_color_r = new int[width*height];
  int *comp_color_g = new int[width*height];
  int *comp_color_b = new int[width*height];
  int *comp_size = new int[width*height];
  // end vars for AVERAGE_COLOR
  
	switch(output_type) {
	  case GREYSCALE_COLOR:
		  for (int y = 0; y < height; y++) {
			    for (int x = 0; x < width; x++) {
			    	rgb this_pixel = imRef(im, x, y);
			    	int grey_pixel = 
			    	 (this_pixel.r + this_pixel.g + this_pixel.b) / 3;
			      imRef(output, x, y).r = grey_pixel;
			      imRef(output, x, y).g = grey_pixel;
			      imRef(output, x, y).b = grey_pixel;
			    }
			}
		  break;
	  case AVERAGE_COLOR:
		  // create a map from pixel to map.
		  for(int i = 0; i < width*height; i++) {
		  	pixel_id[i] = width*height;
		  	comp_color_r[i] = 0;
		  	comp_color_g[i] = 0;
		  	comp_color_b[i] = 0;
		  	comp_size[i] = 0;
		  }
		  
		  for (int y = 0; y < height; y++) {
		    for (int x = 0; x < width; x++) {
		      int comp = u->find(y * width + x);
		      pixel_id_to_comp[width*height] =  pixel_id[comp];
		    }
		  }  
		  // map complete 
		  // now we can reference comp_(prop)[pixel_id_to_comp[i]]
		  // to set comp_(prop)
		  for (int y = 0; y < height; y++) {
		    for (int x = 0; x < width; x++) {
		    	int comp = pixel_id_to_comp[x*y];
					comp_color_r[comp] += imRef(im, x, y).r;
					comp_color_g[comp] += imRef(im, x, y).g;
					comp_color_b[comp] += imRef(im, x, y).b;
					comp_size[comp]++;
		    }
		  }  
		  
		  // finally, we can set the output colors
		  for (int y = 0; y < height; y++) {
		    for (int x = 0; x < width; x++) {
		    	int comp = pixel_id_to_comp[x*y];
					imRef(output, x, y).r = (uchar) comp_color_r[comp] / comp_size[comp];
					imRef(output, x, y).g = (uchar) comp_color_g[comp] / comp_size[comp];
					imRef(output, x, y).b = (uchar) comp_color_b[comp] / comp_size[comp];
		    }
		  }
		  break;
	case RANDOM_COLOR:
	default:
			for (int y = 0; y < height; y++) {
			    for (int x = 0; x < width; x++) {
			      int comp = u->find(y * width + x);
			      imRef(output, x, y) = colors[comp];
			    }
			}
			break;
	}


  // vars for AVERAGE_COLOR
  delete [] pixel_id;
  delete [] pixel_id_to_comp;
  delete [] comp_color_r;
  delete [] comp_color_g;
  delete [] comp_color_b;
  delete [] comp_size;
  // end vars for AVERAGE_COLOR
  
  delete [] colors;  
  delete u;

  return output;
}

#endif
