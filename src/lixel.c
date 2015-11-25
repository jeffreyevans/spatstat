#include <R.h>
#include <math.h>

/* 
   lixel.c

   divide a linear network into shorter segments

 */

void Clixellate(ns, fromcoarse, tocoarse, 
		fromfine, tofine, 
		nv, xv, yv, 
		nsplit, 
		np, spcoarse, tpcoarse, 
		spfine, tpfine) 
     /* 
	A linear network with *ns segments and *nv vertices
	is specified by the vectors from, to, xv, yv.

	The i-th segment will be subdivided into nsplit[i] subsegments.

	New data will be added at the end of the vectors 'xv' and 'yv' 
	representing additional vertices in the new network.

	The point pattern data (*np points with local coordinates sp, tp
	in the coarse network) will be mapped to the new 'fine' network. 
	Points are sorted by 'spcoarse' value.
	
	'xv', 'yv' must have space for (nv + sum(nsplit-1)) entries.

	'fromfine', 'tofine' must have length = sum(nsplit).

      */
     int *ns; /* number of segments (input & output) */
     int *fromcoarse, *tocoarse; /* endpoints of each segment (input) */
     int *fromfine, *tofine;  /* endpoints of each segment (output) */
     int *nv; /* number of vertices (input & output) */
     double *xv, *yv; /* cartesian coords of vertices (input & output) */
     int *nsplit; /* number of pieces into which each segment should be split */
     int *np; /* number of data points */
     double *tpcoarse, *tpfine; /* location coordinate */
     int *spcoarse, *spfine; /* segment id coordinate */
{
  int Np, oldNs, oldNv, i, j, k, m, ll;
  int oldfromi, oldtoi, newlines, newNv, newNs, SegmentForData;
  double xstart, xend, ystart, yend, xincr, yincr, tn;

  Np = *np;
  newNv = oldNv = *nv;
  oldNs = *ns;
  newNs = 0;

  /* 
     initialise pointer at start of point pattern
     Determine which segment contains first point
  */
  k = 0;
  SegmentForData = (Np > 0) ? spcoarse[0] : -1;
  
  /* loop over line segments in original network */
  for(i = 0; i < oldNs; i++) {

    newlines = nsplit[i]; 

    if(newlines == 1) {
      /* copy existing segment to new segment list and advance pointer */
      fromfine[newNs] = fromcoarse[i];
      tofine[newNs]   = tocoarse[i];
      ++newNs;
    } else if(newlines > 1) {

      /* split segment into 'newlines' pieces */
      oldfromi = fromcoarse[i];
      oldtoi   = tocoarse[i];

      xstart = xv[oldfromi];
      ystart = yv[oldfromi];

      xend = xv[oldtoi];
      yend = yv[oldtoi];
    
      xincr = (xend-xstart)/newlines;
      yincr = (yend-ystart)/newlines;

      m = newlines - 1;

      for(j = 1; j <= m; j++) {
	/* create new vertex, number 'newNv' */
	xv[newNv] = xstart + j * xincr;
	yv[newNv] = ystart + j * yincr;
	/* create new segment, number 'newNs', ending at new vertex */
	fromfine[newNs] = (j == 1) ? oldfromi : (newNv-1);
	tofine[newNs]   = newNv;
	++newNv;
	++newNs;
      }
      /* create segment from last added vertex to end of old segment */
      fromfine[newNs] = newNv-1;
      tofine[newNs] = oldtoi;
      ++newNs;
    }

    /* handle data points lying on current segment i */
    while(SegmentForData == i) {
      if(newlines == 1) {
	spfine[k] = spcoarse[k];
	tpfine[k] = tpcoarse[k];
      } else {
	tn = tpcoarse[k] * newlines;
	ll = (int) floor(tn); 
	ll = (ll < 0) ? 0 : (ll > newlines) ? newlines: ll;
	tpfine[k] = tn - ll;
	spfine[k] = newNs - newlines + ll;
      }
      ++k;
      SegmentForData = (k < Np) ? spcoarse[k] : -1;
    }
  }
  *nv = newNv;
  *ns = newNs;
}
