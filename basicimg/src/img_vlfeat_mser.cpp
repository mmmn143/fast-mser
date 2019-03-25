#include "stdafx.h"
#include "img_vlfeat_mser.h"

/** @brief Maximum value

**

** Maximum value of the integer type ::unsigned char.

**/

#define MSER_PIX_MAXVAL 256





/** @brief MSER Filter

**

** The MSER filter computes the Maximally Stable Extremal Regions of

** an image.

**

** @sa @ref mser

**/

typedef struct _MserFilt MserFilt;



/** @brief MSER filter statistics */

typedef struct _MserStats MserStats;



/** @brief MSER filter statistics definition */

struct _MserStats {

    int num_extremal;           /**< number of extremal regions                                */

    int num_unstable;           /**< number of unstable extremal regions                       */

    int num_abs_unstable;       /**< number of regions that failed the absolute stability test */

    int num_too_big;            /**< number of regions that failed the maximum size test       */

    int num_too_small;          /**< number of regions that failed the minimum size test       */

    int num_duplicates;         /**< number of regions that failed the duplicate test          */

};





/** @name Construction and Destruction

** @{

**/

void mser_new(void*& f, int ndims, int const *dims);





void mser_delete(MserFilt *f);





/** @} */





/** @name Processing

** @{

**/

void mser_process(MserFilt *f,

                  unsigned char const *im);





void mser_ell_fit(MserFilt *f);





/** @} */





/** @name Retrieving data

** @{

**/

unsigned int mser_get_regions_num(MserFilt const *f);





unsigned int const *mser_get_regions(MserFilt const *f);





float const *mser_get_ell(MserFilt const *f);





unsigned int mser_get_ell_num(MserFilt const *f);





unsigned int mser_get_ell_dof(MserFilt const *f);





MserStats const *mser_get_stats(MserFilt const *f);





/** @} */





/** @name Retrieving parameters

** @{

**/

unsigned char mser_get_delta(MserFilt const *f);





float mser_get_min_area(MserFilt const *f);





float mser_get_max_area(MserFilt const *f);





float mser_get_max_variation(MserFilt const *f);





float mser_get_min_diversity(MserFilt const *f);





/** @} */





/** @name Setting parameters

** @{

**/

void mser_set_delta(MserFilt *f, unsigned char x);





void mser_set_min_area(MserFilt *f, float x);





void mser_set_max_area(MserFilt *f, float x);





void mser_set_max_variation(MserFilt *f, float x);





void mser_set_min_diversity(MserFilt *f, float x);





/** @} */





/* ====================================================================

*                                                   INLINE DEFINITIONS

* ================================================================== */





/** @internal

** @brief MSER accumulator data type

**

** This is a large integer type. It should be large enough to contain

** a number equal to the area (volume) of the image by the image

** width by the image height (for instance, if the image is a square

** of side 256, the maximum value is 256 x 256 x 256).

**/

typedef float mser_acc;



/** @internal @brief Basic region flag: null region */

#ifdef COMPILER_MSC

#define MSER_VOID_NODE ( (1ui64 << 32) - 1)

#else

#define MSER_VOID_NODE ( (1ULL << 32) - 1)

#endif



/* ----------------------------------------------------------------- */





/** @internal

** @brief MSER: basic region (declaration)

**

** Extremal regions and maximally stable extremal regions are

** instances of image regions.

**

** There is an image region for each pixel of the image. Each region

** is represented by an instance of this structure.  Regions are

** stored into an array in pixel order.

**

** Regions are arranged into a forest. MserReg::parent points to

** the parent node, or to the node itself if the node is a root.

** MserReg::parent is the index of the node in the node array

** (which therefore is also the index of the corresponding

** pixel). MserReg::height is the distance of the fartest leaf. If

** the node itself is a leaf, then MserReg::height is zero.

**

** MserReg::area is the area of the image region corresponding to

** this node.

**

** MserReg::region is the extremal region identifier. Not all

** regions are extremal regions however; if the region is NOT

** extremal, this field is set to ....

**/

struct _MserReg {

    unsigned int parent;         /**< points to the parent region.            */

    unsigned int shortcut;       /**< points to a region closer to a root.    */

    unsigned int height;         /**< region height in the forest.            */

    unsigned int area;           /**< area of the region.                     */

};



/** @internal @brief MSER: basic region */

typedef struct _MserReg MserReg;



/* ----------------------------------------------------------------- */





/** @internal

** @brief MSER: extremal region (declaration)

**

** Extremal regions (ER) are extracted from the region forest. Each

** region is represented by an instance of this structure. The

** structures are stored into an array, in arbitrary order.

**

** ER are arranged into a tree. @a parent points to the parent ER, or

** to itself if the ER is the root.

**

** An instance of the structure represents the extremal region of the

** level set of intensity MserExtrReg::value and containing the

** pixel MserExtReg::index.

**

** MserExtrReg::area is the area of the extremal region and

** MserExtrReg::area_top is the area of the extremal region

** containing this region in the level set of intensity

** MserExtrReg::area + @c delta.

**

** MserExtrReg::variation is the relative area variation @c

** (area_top-area)/area.

**

** MserExtrReg::max_stable is a flag signaling whether this extremal

** region is also maximally stable.

**/

struct _MserExtrReg {

    int parent;         /**< index of the parent region                   */

    int index;          /**< index of pivot pixel                         */

    unsigned char value;          /**< value of pivot pixel                         */

    unsigned int shortcut;       /**< shortcut used when building a tree           */

    unsigned int area;           /**< area of the region                           */

    float variation;      /**< rel. area variation                          */

    unsigned int max_stable;     /**< max stable number (=0 if not maxstable)      */

};





/** @internal

** @brief MSER: extremal region */

typedef struct _MserExtrReg MserExtrReg;



/* ----------------------------------------------------------------- */





/** @internal

** @brief MSER filter

** @see @ref mser

**/

struct _MserFilt {

    /** @name Image data and meta data @internal */

    /*@{*/

    int ndims;          /**< number of dimensions                    */

    int *dims;          /**< dimensions                              */

    int nel;            /**< number of image elements (pixels)       */

    int *subs;          /**< N-dimensional subscript                 */

    int *dsubs;         /**< another subscript                       */

    int *strides;       /**< strides to move in image data           */

    /*@}*/



    unsigned int *perm;  /**< pixel ordering                          */

    unsigned int *joins; /**< sequence of join ops                    */

    int njoins; /**< number of join ops                      */



    /** @name Regions */

    /*@{*/

    MserReg *r;     /**< basic regions                           */

    MserExtrReg *er;    /**< extremal tree                           */

    unsigned int *mer;   /**< maximally stable extremal regions       */

    int ner;    /**< number of extremal regions              */

    int nmer;   /**< number of maximally stable extr. reg.   */

    int rer;    /**< size of er buffer                       */

    int rmer;   /**< size of mer buffer                      */

    /*@}*/



    /** @name Ellipsoids fitting */

    /*@{*/

    float *acc;           /**< moment accumulator.                    */

    float *ell;           /**< ellipsoids list.                       */

    int rell;           /**< size of ell buffer                     */

    int nell;           /**< number of ellipsoids extracted         */

    int dof;            /**< number of dof of ellipsoids.           */



    /*@}*/



    /** @name Configuration */

    /*@{*/

    int verbose;        /**< be verbose                             */

    int delta;          /**< delta filter parameter                 */

    float max_area;       /**< badness test parameter                 */

    float min_area;       /**< badness test parameter                 */

    float max_variation;  /**< badness test parameter                 */

    float min_diversity;  /**< minimum diversity                      */

	f32 nms_similarity;

	b8 from_min_max[2];

	u8 gray_mask;

	img_Connected_Type connect_type;

    /*@}*/



    MserStats stats;        /** run statistic                           */

};



/* ----------------------------------------------------------------- */





/** @brief Get delta

** @param f MSER filter.

** @return value of @c delta.

**/

unsigned char

mser_get_delta(MserFilt const *f) {

    return (f->delta);

}





/** @brief Set delta

** @param f MSER filter.

** @param x value of @c delta.

**/

void

mser_set_delta(MserFilt *f, unsigned char x) {

    f->delta = x;

}





/* ----------------------------------------------------------------- */





/** @brief Get minimum diversity

** @param  f MSER filter.

** @return value of @c minimum diversity.

**/

float

mser_get_min_diversity(MserFilt const *f) {

    return (f->min_diversity);

}





/** @brief Set minimum diversity

** @param f MSER filter.

** @param x value of @c minimum diversity.

**/

void

mser_set_min_diversity(MserFilt *f, float x) {

    f->min_diversity = x;

}





/* ----------------------------------------------------------------- */





/** @brief Get statistics

** @param f MSER filter.

** @return statistics.

**/

MserStats const *

mser_get_stats(MserFilt const *f) {

    return (&f->stats);

}





/* ----------------------------------------------------------------- */





/** @brief Get maximum region area

** @param f MSER filter.

** @return maximum region area.

**/

float

mser_get_max_area(MserFilt const *f) {

    return (f->max_area);

}





/** @brief Set maximum region area

** @param f MSER filter.

** @param x maximum region area.

**/

void

mser_set_max_area(MserFilt *f, float x) {

    f->max_area = x;

}





/* ----------------------------------------------------------------- */





/** @brief Get minimum region area

** @param f MSER filter.

** @return minimum region area.

**/

float

mser_get_min_area(MserFilt const *f) {

    return (f->min_area);

}





/** @brief Set minimum region area

** @param f MSER filter.

** @param x minimum region area.

**/

void

mser_set_min_area(MserFilt *f, float x) {

    f->min_area = x;

}





/* ----------------------------------------------------------------- */





/** @brief Get maximum region variation

** @param f MSER filter.

** @return maximum region variation.

**/

float

mser_get_max_variation(MserFilt const *f) {

    return (f->max_variation);

}





/** @brief Set maximum region variation

** @param f MSER filter.

** @param x maximum region variation.

**/

void

mser_set_max_variation(MserFilt *f, float x) {

    f->max_variation = x;

}





/* ----------------------------------------------------------------- */





/** @brief Get maximally stable extremal regions

** @param f MSER filter.

** @return array of MSER pivots.

**/

unsigned int const *

mser_get_regions(MserFilt const *f) {

    return (f->mer);

}





/** @brief Get number of maximally stable extremal regions

** @param f MSER filter.

** @return number of MSERs.

**/

unsigned int

mser_get_regions_num(MserFilt const *f) {

    return (f->nmer);

}





/* ----------------------------------------------------------------- */





/** @brief Get ellipsoids

** @param f MSER filter.

** @return ellipsoids.

**/

float const *

mser_get_ell(MserFilt const *f) {

    return (f->ell);

}





/** @brief Get number of degrees of freedom of ellipsoids

** @param f MSER filter.

** @return number of degrees of freedom.

**/

unsigned int

mser_get_ell_dof(MserFilt const *f) {

    return (f->dof);

}





/** @brief Get number of ellipsoids

** @param f MSER filter.

** @return number of ellipsoids

**/

unsigned int

mser_get_ell_num(MserFilt const *f) {

    return (f->nell);

}





/*MSER */





/** -------------------------------------------------------------------

** @brief Advance N-dimensional subscript

**

** The function increments by one the subscript @a subs indexing an

** array the @a ndims dimensions @a dims.

**

** @param ndims number of dimensions.

** @param dims dimensions.

** @param subs subscript to advance.

**/



void adv(int ndims, int const *dims, int *subs) {

    int d = 0;

    while (d < ndims) {

        if (++subs[d] < dims[d])

            return;

        subs[d++] = 0;

    }

}





/** -------------------------------------------------------------------

** @brief Climb the region forest to reach aa root

**

** The function climbs the regions forest @a r starting from the node

** @a idx to the corresponding root.

**

** To speed-up the operation, the function uses the

** MserReg::shortcut field to quickly jump to the root. After the

** root is reached, all the used shortcut are updated.

**

** @param r regions' forest.

** @param idx stating node.

** @return index of the reached root.

**/



unsigned int climb(MserReg *r, unsigned int idx) {

    unsigned int prev_idx = idx;

    unsigned int next_idx;

    unsigned int root_idx;



    /* move towards root to find it */

    while (1) {

        /* next jump to the root */

        next_idx = r[idx].shortcut;



        /* recycle shortcut to remember how we came here */

        r[idx].shortcut = prev_idx;



        /* stop if the root is found */

        if (next_idx == idx)

            break;



        /* next guy */

        prev_idx = idx;

        idx = next_idx;

    }



    root_idx = idx;



    /* move backward to update shortcuts */

    while (1) {

        /* get previously visited one */

        prev_idx = r[idx].shortcut;



        /* update shortcut to point to the new root */

        r[idx].shortcut = root_idx;



        /* stop if the first visited node is reached */

        if (prev_idx == idx)

            break;



        /* next guy */

        idx = prev_idx;

    }



    return (root_idx);

}





/** -------------------------------------------------------------------

** @brief Create a new MSER filter

**

** Initializes a new MSER filter for images of the specified

** dimensions. Images are @a ndims -dimensional arrays of dimensions

** @a dims.

**

** @param ndims number of dimensions.

** @param dims  dimensions.

**/





void mser_new(void *&fv, int ndims, int const *dims) {
	MserFilt* f = (MserFilt*)fv;

	b8 need_rellocate = sys_false;

	if (f != NULL && f->ndims == ndims) {
		b8 difference = sys_false;

		for (i32 i = 0; i < ndims; ++i) {
			if (f->dims[i] != dims[i]) {
				difference = sys_true;
				break;
			}
		}

		if (difference) {
			need_rellocate = sys_true;
		}
	} else {
		need_rellocate = sys_true;
	}

	if (need_rellocate) {
		f = (MserFilt *) calloc(sizeof(MserFilt), 1);
		fv = f;

		f->ndims = ndims;

		f->dims = (int *) malloc(sizeof(int) * ndims);

		f->subs = (int *) malloc(sizeof(int) * ndims);

		f->dsubs = (int *) malloc(sizeof(int) * ndims);

		f->strides = (int *) malloc(sizeof(int) * ndims);

		/* shortcuts */

		if (f->dims != NULL && f->subs != NULL && f->dsubs != NULL && f->strides != NULL) {

			int k = 0;



			/* copy dims to f->dims */

			memcpy(f->dims, dims, sizeof(int) * ndims);



			/* compute strides to move into the N-dimensional image array */

			f->strides[0] = 1;

			for (k = 1; k < ndims; ++k) {

				f->strides[k] = f->strides[k - 1] * dims[k - 1];

			}



			/* total number of pixels */

			f->nel = f->strides[ndims - 1] * dims[ndims - 1];



			/* dof of ellipsoids */

			f->dof = ndims * (ndims + 1) / 2 + ndims;



			/* more buffers */

			f->perm = (unsigned int *) malloc(sizeof(unsigned int) * f->nel);

			f->joins = (unsigned int *) malloc(sizeof(unsigned int) * f->nel);

			f->r = (MserReg *) malloc(sizeof(MserReg) * f->nel);

			i32 memory_cost = sizeof(unsigned int) * f->nel + sizeof(unsigned int) * f->nel + sizeof(MserReg) * f->nel;

			basiclog_info2(sys_strcombine()<<L"linear memory cost "<< memory_cost / 1024.0 / 1024.0 <<L"MB");
		}
	}

	f->er = 0;

	f->rer = 0;

	f->mer = 0;

	f->rmer = 0;

	f->ell = 0;

	f->rell = 0;
}





/** -------------------------------------------------------------------

** @brief Delete MSER filter

**

** The function releases the MSER filter @a f and all its resources.

**

** @param f MSER filter to be deleted.

**/



void

mser_delete(MserFilt *f) {

    if (f) {

        if (f->acc)

            free(f->acc);

        if (f->ell)

            free(f->ell);



        if (f->er)

            free(f->er);

        if (f->r)

            free(f->r);

        if (f->joins)

            free(f->joins);

        if (f->perm)

            free(f->perm);



        if (f->strides)

            free(f->strides);

        if (f->dsubs)

            free(f->dsubs);

        if (f->subs)

            free(f->subs);

        if (f->dims)

            free(f->dims);



        if (f->mer)

            free(f->mer);

        free(f);

    }

}


/** -------------------------------------------------------------------

** @brief Process image

**

** The functions calculates the Maximally Stable Extremal Regions

** (MSERs) of image @a im using the MSER filter @a f.

**

** The filter @a f must have been initialized to be compatible with

** the dimensions of @a im.

**

** @param f MSER filter.

** @param im image data.

**/



void

mser_process(MserFilt *f, unsigned char const *im) {

    /* shortcuts */

    unsigned int nel = f->nel;

    unsigned int *perm = f->perm;

    unsigned int *joins = f->joins;

    int ndims = f->ndims;

    int *dims = f->dims;

    int *subs = f->subs;

    int *dsubs = f->dsubs;

    int *strides = f->strides;

    MserReg *r = f->r;

    MserExtrReg *er = f->er;

    unsigned int *mer = f->mer;

    int delta = f->delta;



    int njoins = 0;

    int ner = 0;

    int nmer = 0;

#define get_gray(gray)	((gray) ^ (f->gray_mask))

    int i, k;



    /* delete any previosuly computed ellipsoid */

    f->nell = 0;





    /* -----------------------------------------------------------------

    *                                          Sort pixels by intensity

    * -------------------------------------------------------------- */



    {
		if (f->gray_mask == 0 || !f->from_min_max[0]) {
			unsigned int buckets[MSER_PIX_MAXVAL];



			/* clear buckets */

			memset(buckets, 0, sizeof(unsigned int) * MSER_PIX_MAXVAL);





			/* compute bucket size (how many pixels for each intensity

			* value) */

			for (i = 0; i < (int) nel; ++i) {
				++buckets[get_gray(im[i])];
			}



			/* cumulatively add bucket sizes */

			for (i = 1; i < MSER_PIX_MAXVAL; ++i) {

				buckets[i] += buckets[i - 1];

			}



			/* empty buckets computing pixel ordering */

			for (i = nel; i >= 1;) {
				u32 j = --buckets[get_gray(im[--i])];
				perm[j] = i;

			}
		} else {
			i32 half = nel / 2;
			u32 temp_u;
			for (i32 i = 0; i < half; ++i) {
				temp_u = perm[i];
				perm[i] = perm[nel - 1 - i];
				perm[nel - 1 - i] = temp_u;
			}
		}
    }



    /* initialize the forest with all void nodes */

    for (i = 0; i < (int) nel; ++i) {

        r[i].parent = MSER_VOID_NODE;

    }





    /* -----------------------------------------------------------------

    *                        Compute regions and count extremal regions

    * -------------------------------------------------------------- */





    /*

    * In the following:

    * idx    : index of the current pixel

    * val    : intensity of the current pixel

    * r_idx  : index of the root of the current pixel

    * n_idx  : index of the neighbors of the current pixel

    * nr_idx : index of the root of the neighbor of the current pixel

    */



    /* process each pixel by increasing intensity */

    for (i = 0; i < (int) nel; ++i) {

        /* pop next node xi */

        unsigned int idx = perm[i];

        u8 val = get_gray(im[idx]);

        unsigned int r_idx;



        /* add the pixel to the forest as a root for now */

        r[idx].parent = idx;

        r[idx].shortcut = idx;

        r[idx].area = 1;

        r[idx].height = 1;



        r_idx = idx;





        /* convert the index IDX into the subscript SUBS; also initialize

        * DSUBS to (-1,-1,...,-1) */

        {

            unsigned int temp = idx;

            for (k = ndims - 1; k >= 0; --k) {

                dsubs[k] = -1;

                subs[k] = temp / strides[k];

                temp = temp % strides[k];

            }

        }



        /* examine the neighbors of the current pixel */

        while (1) {

            unsigned int n_idx = 0;

            int good = 1;





            /*

            * Compute the neighbor subscript as NSUBS+SUB, the

            * corresponding neighbor index NINDEX and check that the

            * neighbor is within the image domain.

            */

            for (k = 0; k < ndims && good; ++k) {

                int temp = dsubs[k] + subs[k];

                good &= (0 <= temp) && (temp < dims[k]);

                n_idx += temp * strides[k];

            }

			b8 valid_neibour = sys_true;

			if (f->connect_type == img_Connected_Type_4_Neibour) {
				i32 n_offset = abs((i32)n_idx - (i32)idx);
				if (n_offset != 1 && n_offset % dims[0] != 0) {
					valid_neibour = sys_false;
				}
			}

            /*

            * The neighbor should be processed if the following conditions

            * are met:

            * 1. The neighbor is within image boundaries.

            * 2. The neighbor is indeed different from the current node

            * (the opposite happens when DSUB=(0,0,...,0)).

            * 3. The neighbor is already in the forest, meaning that it has

            * already been processed.

            */

			if (valid_neibour) {
				if (good &&

					n_idx != idx &&

					r[n_idx].parent != MSER_VOID_NODE) {

						unsigned char nr_val = 0;

						unsigned int nr_idx = 0;

						int hgt = r[r_idx].height;

						int n_hgt = r[nr_idx].height;





						/*

						* Now we join the two subtrees rooted at

						* R_IDX = ROOT(  IDX)

						* NR_IDX = ROOT(N_IDX).

						* Note that R_IDX = ROOT(IDX) might change as we process more

						* neighbors, so we need keep updating it.

						*/



						r_idx = climb(r, idx);

						nr_idx = climb(r, n_idx);





						/*

						* At this point we have three possibilities:

						* (A) ROOT(IDX) == ROOT(NR_IDX). In this case the two trees

						* have already been joined and we do not do anything.

						* (B) I(ROOT(IDX)) == I(ROOT(NR_IDX)). In this case the pixel

						* IDX is extending an extremal region with the same

						* intensity value. Since ROOT(NR_IDX) will NOT be an

						* extremal region of the full image, ROOT(IDX) can be

						* safely added as children of ROOT(NR_IDX) if this

						* reduces the height according to the union rank

						* heuristic.

						* (C) I(ROOT(IDX)) > I(ROOT(NR_IDX)). In this case the pixel

						* IDX is starting a new extremal region. Thus ROOT(NR_IDX)

						* WILL be an extremal region of the final image and the

						* only possibility is to add ROOT(NR_IDX) as children of

						* ROOT(IDX), which becomes parent.

						*/



						if (r_idx != nr_idx) /* skip if (A) */



						{

							nr_val = get_gray(im[nr_idx]);



							if (nr_val == val && hgt < n_hgt) {

								/* ROOT(IDX) becomes the child */

								r[r_idx].parent = nr_idx;

								r[r_idx].shortcut = nr_idx;

								r[nr_idx].area += r[r_idx].area;

								r[nr_idx].height = max(n_hgt, hgt + 1);



								joins[njoins++] = r_idx;

							} else {

								/* cases ROOT(IDX) becomes the parent */

								r[nr_idx].parent = r_idx;

								r[nr_idx].shortcut = r_idx;

								r[r_idx].area += r[nr_idx].area;

								r[r_idx].height = max(hgt, n_hgt + 1);



								joins[njoins++] = nr_idx;



								/* count if extremal */

								if (nr_val != val)

									++ner;

							}       /* check b vs c */

						}               /* check a vs b or c */

				}                       /* neighbor done */
			}

            



            /* move to next neighbor */

            k = 0;

            while (++dsubs[k] > 1) {

                dsubs[k++] = -1;

                if (k == ndims)

                    goto done_all_neighbors;

            }

        } /* next neighbor */

        done_all_neighbors:;

    }        /* next pixel */



    /* the last root is extremal too */

    ++ner;



    /* save back */

    f->njoins = njoins;



    f->stats.num_extremal = ner;





    /* -----------------------------------------------------------------

    *                                          Extract extremal regions

    * -------------------------------------------------------------- */





    /*

    * Extremal regions are extracted and stored into the array ER.  The

    * structure R is also updated so that .SHORTCUT indexes the

    * corresponding extremal region if any (otherwise it is set to

    * VOID).

    */



    /* make room */

    if (f->rer < ner) {

        if (er)

            free(er);

        f->er = er = (MserExtrReg *) malloc(sizeof(MserExtrReg) * ner);

		i32 memory_cost = sizeof(MserExtrReg) * ner;

		basiclog_info2(sys_strcombine()<<L"linear memory cost "<< memory_cost / 1024.0 / 1024.0 <<L"MB");

        f->rer = ner;

    };



    /* save back */

    f->nmer = ner;



    /* count again */

    ner = 0;



    /* scan all regions Xi */

    if (er != NULL) {

        for (i = 0; i < (int) nel; ++i) {

            /* pop next node xi */

            unsigned int idx = perm[i];



            unsigned char val = get_gray(im[idx]);

            unsigned int p_idx = r[idx].parent;

            unsigned char p_val = get_gray(im[p_idx]);



            /* is extremal ? */

            int is_extr = (p_val > val) || idx == p_idx;



            if (is_extr) {

                /* if so, add it */

                er[ner].index = idx;

                er[ner].parent = ner;

                er[ner].value = get_gray(im[idx]);

                er[ner].area = r[idx].area;



                /* link this region to this extremal region */

                r[idx].shortcut = ner;



                /* increase count */

                ++ner;

            } else {

                /* link this region to void */

                r[idx].shortcut = MSER_VOID_NODE;

            }

        }

    }





    /* -----------------------------------------------------------------

    *                                   Link extremal regions in a tree

    * -------------------------------------------------------------- */



    for (i = 0; i < ner; ++i) {

        unsigned int idx = er[i].index;



        do {

            idx = r[idx].parent;

        } while (r[idx].shortcut == MSER_VOID_NODE);



        er[i].parent = r[idx].shortcut;

        er[i].shortcut = i;

    }

	f->ner = ner;
}

void mser_recogniztion(MserFilt* f) {
	unsigned int nel = f->nel;

	unsigned int *perm = f->perm;

	unsigned int *joins = f->joins;

	int ndims = f->ndims;

	int *dims = f->dims;

	int *subs = f->subs;

	int *dsubs = f->dsubs;

	int *strides = f->strides;

	MserReg *r = f->r;

	MserExtrReg *er = f->er;

	unsigned int *mer = f->mer;

	int delta = f->delta;

	int njoins = f->njoins;

	int ner = f->ner;

	int nmer = f->nmer;

	int nbig = 0;

	int nsmall = 0;

	int nbad = 0;

	int ndup = 0;

	int i, j;

	/* -----------------------------------------------------------------

    *                            Compute variability of +DELTA branches

    * -------------------------------------------------------------- */





    /* For each extremal region Xi of value VAL we look for the biggest

    * parent that has value not greater than VAL+DELTA. This is dubbed

    * `top parent'. */



    for (i = 0; i < ner; ++i) {

        /* Xj is the current region the region and Xj are the parents */

        int top_val = er[i].value + delta;

        int top = er[i].shortcut;



        /* examine all parents */

        while (1) {

            int next = er[top].parent;

            int next_val = er[next].value;

            /* Break if:

            * - there is no node above the top or

            * - the next node is above the top value.

            */

            if (next == top || next_val > top_val)

                break;



            /* so next could be the top */

            top = next;

        }



        /* calculate branch variation */
		// has problem of parent region NULL
		if (f->delta > 0)
        {

            int area = er[i].area;

            int area_top = er[top].area;

            er[i].variation = (float) (area_top - area) / area;

            er[i].max_stable = 1;

        } else {
			er[i].variation = -1;
			er[i].max_stable = 1;
		}


        /* Optimization: since extremal regions are processed by

        * increasing intensity, all next extremal regions being processed

        * have value at least equal to the one of Xi. If any of them has

        * parent the parent of Xi (this comprises the parent itself), we

        * can safely skip most intermediate node along the branch and

        * skip directly to the top to start our search. */

        {

            int parent = er[i].parent;

            int curr = er[parent].shortcut;

            er[parent].shortcut = max(top, curr);

        }

    }





    /* -----------------------------------------------------------------

    *                                  Select maximally stable branches

    * -------------------------------------------------------------- */



    nmer = ner;

	if (f->nms_similarity >= 0) {
		for (i = 0; i < ner; ++i) {

			unsigned int parent = er[i].parent;

			unsigned char val = er[i].value;

			float var = er[i].variation;

			unsigned char p_val = er[parent].value;

			float p_var = er[parent].variation;

			i32 loser = -1;





			/*

			* Notice that R_parent = R_{l+1} only if p_val = val + 1. If not,

			* this and the parent region coincide and there is nothing to do.

			*/
			if (p_val > val + 1)

				continue;



			/* decide which one to keep and put that in loser */

			if (var - p_var < -f->nms_similarity) {
				loser = parent;
			} else if (p_var - var < -f->nms_similarity) {
				loser = i;
			}

			if (loser != -1) {
				/* make loser NON maximally stable */

				if (er[loser].max_stable) {

					--nmer;

					er[loser].max_stable = 0;

				}
			}
		}
	}

    



    f->stats.num_unstable = ner - nmer;





    /* -----------------------------------------------------------------

    *                                                 Further filtering

    * -------------------------------------------------------------- */





    /* It is critical for correct duplicate detection to remove regions

    * from the bottom (smallest one first).                          */

    {

        float max_area = (float) f->max_area * nel;

        float min_area = (float) f->min_area * nel;

        float max_var = (float) f->max_variation;

        float min_div = (float) f->min_diversity;



        /* scan all extremal regions (intensity value order) */

        for (i = ner - 1; i >= 0L; --i) {

            /* process only maximally stable extremal regions */

            if (!er[i].max_stable)

                continue;



            if (er[i].variation >= max_var) {

                ++nbad;

                goto remove;

            }

            if (er[i].area > max_area) {

                ++nbig;

                goto remove;

            }

            if (er[i].area < min_area) {

                ++nsmall;

                goto remove;

            }





            /*

            * Remove duplicates

            */

            if (min_div < 1.0 && min_div > 0) {

                unsigned int parent = er[i].parent;

                int area, p_area;

                float div;



                /* check all but the root mser */

                if ((int) parent != i) {

                    /* search for the maximally stable parent region */

                    while (!er[parent].max_stable) {

                        unsigned int next = er[parent].parent;

                        if (next == parent)

                            break;

                        parent = next;

                    }





                    /* Compare with the parent region; if the current and parent

                    * regions are too similar, keep only the parent. */

                    area = er[i].area;

                    p_area = er[parent].area;

                    div = (float) (p_area - area) / (float) p_area;



                    if (div < min_div) {

                        ++ndup;

                        goto remove;

                    }

                } /* remove dups end */

            }

            continue;

            remove:

            er[i].max_stable = 0;

            --nmer;

        }         /* check next region */



        f->stats.num_abs_unstable = nbad;

        f->stats.num_too_big = nbig;

        f->stats.num_too_small = nsmall;

        f->stats.num_duplicates = ndup;

    }





    /* -----------------------------------------------------------------

    *                                                   Save the result

    * -------------------------------------------------------------- */



    /* make room */

    if (f->rmer < nmer) {

        if (mer)

            free(mer);

        f->mer = mer = (unsigned int *) malloc(sizeof(unsigned int) * nmer);

        f->rmer = nmer;
    }

    /* save back */

    f->nmer = nmer;



    j = 0;

    if (er != NULL && mer != NULL) {

        for (i = 0; i < ner; ++i) {

            if (er[i].max_stable)

                mer[j++] = er[i].index;

        }

    }
}

void img_vlfeat_mser::allocate_memory(const mt_mat& src, const img_mask_info<u8>& mask) {
	if (!src.is_continuous()) {
		m_src.copy_from(src);
	} else {
		m_src = src;
	}

	i32 dims[2] = {m_src.size()[1], m_src.size()[0]};

	mser_new(m_vl_feat_instance, m_src.dim(), dims);

	MserFilt* f = (MserFilt*)m_vl_feat_instance;
	f->delta = m_delta;

	f->max_area = m_max_point_ratio;

	f->min_area = m_min_point / (f32)f->nel;

	f->max_variation = m_stable_variation;

	f->min_diversity = m_duplicated_variation;
	
	f->nms_similarity = m_nms_similarity;

	f->from_min_max[0] = m_from_min_max[0];
	f->from_min_max[1] = m_from_min_max[1];

	f->connect_type = m_connected_type;
}

void img_vlfeat_mser::clear_memory_cache() {
	m_src = mt_mat();
	mser_delete((MserFilt*)m_vl_feat_instance);
	m_vl_feat_instance = NULL;
}

void img_vlfeat_mser::build_tree(const mt_mat& src, const img_mask_info<u8>& mask, u8 gray_mask) {
	MserFilt* f = (MserFilt*)m_vl_feat_instance;
	f->gray_mask = gray_mask;

	mser_process(f, m_src.data());
}

void img_vlfeat_mser::recognize_mser() {
	MserFilt* f = (MserFilt*)m_vl_feat_instance;
	mser_recogniztion(f);
}

void img_vlfeat_mser::extract_pixel(img_multi_msers& msers, u8 gray_mask) {
	vector<img_mser>& t_mser = msers.m_msers[(gray_mask == 0) ? 0 : 1];
	MserFilt* f = (MserFilt*)m_vl_feat_instance;

	m_channel_total_pixel_number = 0;
	t_mser.resize(f->nmer);

	for (i32 i = 0; i < f->nmer; ++i) {
		u32 idx = f->mer[i];
		u32 r_shortcut = f->r[idx].shortcut;
		t_mser[i].m_gray_level = f->er[r_shortcut].value ^ gray_mask;
		t_mser[i].m_size = f->er[r_shortcut].area;

		m_channel_total_pixel_number += t_mser[i].m_size;

		//basiclog_info2(sys_strcombine()<<L"gray "<<t_mser[i].m_gray_level <<L" area "<<t_mser[i].m_size);
	}
}