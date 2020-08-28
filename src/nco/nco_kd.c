/* $Header$ */

/* K-d tree code originated in OctTools software from UC Berkeley
   It is distributed under the BSD-like license below
   Modifications:
   2018: Obtain source from https://github.com/WyoMurf/kdtree project by Steve Murphy
   201901--201905: Modify, enhance for C99, NCO, and regridding
   201905: Attempts to contact Steve Murphy go unanswered
   201905: None of SM's modifications are necessary/utilized so far as we can tell
   20190614: Rename kd.h, kd.c to nco_kd.h, nco_kd.c */

/*
 * K-d tree geometric data structure
 *
 * David Harrison
 * University of California,  Berkeley
 * 1988
 *
 * This is an implementation of k-d trees as described by Rosenberg
 * in "Geographical Data Structures", IEEE Transactions on CAD, Vol. CAD-4,
 * No. 1, January 1985.  His work is based on that of Jon Bentley in
 * "Multidimensional Binary Search Trees used for Associative Searching",
 * CACM, Vol. 18, No. 9, pp. 509-517, Sept. 1975.
 *
 */

/* Original, unmodified COPYRIGHT: */
/*
 * Oct Tools Distribution 5.1
 *
 * Copyright (c) 1988, 1989, 1990, 1991 Regents of the University of California.
 * All rights reserved.
 *
 * Use and copying of this software and preparation of derivative works
 * based upon this software are permitted.  However, any distribution of
 * this software or derivative works must include the above copyright
 * notice.
 *
 * This software is made available AS IS, and neither the Electronics
 * Research Laboratory or the University of California make any
 * warranty about the software, its performance or its conformity to
 * any specification.
 *
 * Suggestions, comments, or improvements are welcome and should be
 * addressed to:
 *
 *   octtools@ic.berkeley.edu
 *   ..!ucbvax!ic!octtools
 */

/*
 * extensive upgrades, enhancements, fixes, and optimizations made by
 * Steve Murphy. See Documentation in kd.c for information
 * on the routines contained herein:
 * A list of my changes:
 * + build used the nodes son's links to form lists, rather than the list package.
 *   This saves time in that malloc is called much less often.
 * + build uses the geometric mean criteria for finding central nodes, rather than
 *   the centroid of the bounding box. This, on the average, halves the depth of the
 *   tree. Research on random boxes shows that halving the depth of the tree decreases
 *   search traversal 15% Thus are kd trees resilient to degradation.
 * + Added nearest neighbor search routine. TODO: allow the user to pass in pointer
 *   to distance function.
 * + Added rebuild routine. Faster than a build from scratch.
 * + Added node deletion routine. For those purists who hate dead nodes in the tree.
 * + Some routines to give stats on tree health, info about tree, etc.
 * + I may even have inserted some comments to explain some tricky stuff happening
 *   in the code...
 */

#include <stddef.h>
#include "nco_kd.h" /* K-d tree geometric data structure */

static int path_length = 0;
static int path_alloc = 0;
static int path_reset = 1;
static KDElem **path_to_item = (KDElem **) NULL;

static const char *kd_pkg_name = "ncks-kd_tree";


static int kd_data_tries;

static int kd_build_depth = 1000000; /* can you imagine a tree deeper than this? */

static long kddel_number_tried=0;
static long kddel_number_deld=0;

static double kd_tree_badness_factor1 = 0.0;
static double kd_tree_badness_factor2 = 0.0;
static double kd_tree_badness_factor3 = 0.0;  /* count of one-son nodes */
static int kd_tree_max_levels = 0;

/* used in kDist to calculate the appropriate distance metric */
/* nb this is set in Kd_nearest()  */
static poly_typ_enm kd_pl_typ=poly_rll;


/* Forward declarations
void sel_k();
void resolve();
int get_min_max();
void del_elem();
kd_status del_element();

KDElem *load_items();
KDElem *build_node();
KDElem *find_item();
void bounds_update();
*/

/*
 * The following moves the front of `list1' to the front of `list2' and
 * returns `list1'.  This is a destructive operation: both `list1' and
 * `list2' are changed.
*/

#define CMV(list1, list2)					\
  (kd_tmp_ptr=(list1 ? (list1)->sons[0] : (KDElem *) NULL ),	\
    (list1 ? (list1)->sons[0] = (list2) : (KDElem *)NULL),	\
    (list2) = (list1),                                    \
    (list1) = kd_tmp_ptr)

/*
KDElem *cmv (KDElem * list1, KDElem* list2)
{
  KDElem *kd_tmp_ptr;
   kd_tmp_ptr = (list1 ? list1->sons[0] : (KDElem *) NULL );
   list1 ? list1->sons[0] = list2 : (KDElem *)NULL;	
   list2 = list1;                       
   list1 = kd_tmp_ptr;

   return list1;
}

*/


void errRaise(const char *pkg, int code, const char *format)
{

  (void)fprintf(stderr,"%s: %s (%d)\n", pkg, format, code);
  exit(1);

}





int find_min_max_node(int j, KDElem **kd_minval_node, KDElem **kd_minval_nodesdad, int *dir, int *newj);

char *kd_fault(int t)
/* Fatal faults - raises an error using the error handling package */
{
    switch(t)
	{
    case KDF_M:
	errRaise(kd_pkg_name, KDF_M, "out of memory");
	/* NOTREACHED */
	break;
    case KDF_ZEROID:
	errRaise(kd_pkg_name, KDF_ZEROID, "attempt to insert null data");
	/* NOTREACHED */
	break;
    case KDF_MD:
	errRaise(kd_pkg_name, KDF_MD, "bad median");
	/* NOTREACHED */
	break;
    case KDF_F:
	errRaise(kd_pkg_name, KDF_F, "bad father node");
	/* NOTREACHED */
	break;
    case KDF_DUPL:
	errRaise(kd_pkg_name, KDF_DUPL, "attempt to insert duplicate item");
	/* NOTREACHED */
    default:
	errRaise(kd_pkg_name, KDF_UNKNOWN, "unknown fault: %d");
	/* NOTREACHED */
	break;
    }
    return (char *) NULL;
}



KDElem *kd_new_node(kd_generic item, kd_box size, double lomin, double himax, double other, KDElem *loson, KDElem *hison)
// kd_generic item;		/* New node value */
// kd_box size;			/* Size of item   */
// int lomin, himax, other;	/* Bounds info    */
// KDElem *loson, *hison;		/* Sons           */
/* Allocates and initializes a new node element */
{
    KDElem *newElem;

    newElem = (KDElem*)nco_malloc(sizeof (KDElem));
    newElem->item = item;
    newElem->size[0] = size[0];
    newElem->size[1] = size[1];
    newElem->size[2] = size[2];
    newElem->size[3] = size[3];
    newElem->lo_min_bound = lomin;
    newElem->hi_max_bound = himax;
    newElem->other_bound = other;
    newElem->sons[0] = loson;
    newElem->sons[1] = hison;
    return newElem;
}


/*
 * K-d tree creation and deletion.
 */


KDTree * kd_create(void)
/*
 * Creates a new k-d tree and returns its handle.  This handle is
 * used by all other k-d tree operations.  It can be freed using
 * kd_destroy().
 */
{
    KDTree *newTree;
	
    newTree = (KDTree*)nco_malloc( sizeof(KDTree));
    newTree->tree = (KDElem *) NULL;
    newTree->item_count = newTree->dead_count = 0;
    return newTree;
}



/*
 * kd_build() requires a simple linked list.  This page implements
 * that list.

 * One thing that could be done here is get rid of the kd_list and kd_info structs, since
 * they contain nothing that a KDElem doesn't, really. The list could form thru the
 * sons[0] pointer or some such atrocity.
 */




KDElem *kd_tmp_ptr;

/*
#define NIL			(KDElem *) NULL
#define CAR(list)		(list)->item
#define CDR(list)		(list ? (list)->sons[0] : (KDElem *) NULL )
#define CONS(Item, list)						\
(kd_tmp_ptr = Item,  kd_tmp_ptr->sons[0] = (list), kd_tmp_ptr)
*/

/*
 * The following moves the front of `list1' to the front of `list2' and
 * returns `list1'.  This is a destructive operation: both `list1' and
 * `list2' are changed.
 */ 



  
int kd_set_build_depth(int depth)
{
	int retval = kd_build_depth;
	kd_build_depth = depth;
	return retval;
}

KDTree* kd_build(int (*itemfunc)(kd_generic arg, kd_generic *val, kd_box size), kd_generic arg)
// int (*itemfunc)();		/* Returns new items       */
// kd_generic arg;			/* Data to itemfunc        */
/*
 * This routine builds a new, reasonably balanced k-d tree
 * from a list of items.  This list of items is collected
 * by repeatedly calling `itemfunc' using the following
 * arguments:
 *   int itemfunc(arg, val, size)
 *   kd_generic arg;
 *   kd_generic *val;
 *   kd_box size;
 * Each time the itemfunc is called,  it should return the
 * next item to be placed in the tree in `val',  and the size (bounding box)
 * of the item in `size'.  When there are no more items,  the
 * routine should return zero.  `itemfunc' is guaranteed to be
 * called for all items. `arg' is passed as a convenience (usually
 * for state information).
 */
{
    KDTree *newTree = (KDTree *) kd_create();
    KDElem *items, *spares = (KDElem *)NULL;
    kd_box extent;

    int item_count = 0;
    double mean;

	
    /* First build up list of items and their overall extent */
    items = load_items(itemfunc, arg, extent, &item_count, &mean);
    if (!items)
	{
		(void) kd_fault(KDF_ZEROID);
		/* NOTREACHED */
    }

    /* Then recursively fill the tree */
	if( kd_build_depth )
	{
		newTree->tree = build_node(items, item_count, extent, 0, 1,
								   kd_build_depth, &spares,
								   &(newTree->item_count), mean);
		newTree->items_balanced = newTree->item_count;
	}
	else
	{
		extent[KD_LEFT] = extent[KD_BOTTOM] = INT_MAX;
		extent[KD_RIGHT] = extent[KD_TOP] = INT_MIN;
		spares = items;
	}
	
	newTree->extent[0] = extent[0];
	newTree->extent[1] = extent[1];
	newTree->extent[2] = extent[2];
	newTree->extent[3] = extent[3];
	
	while( spares )
	{
	        //KDElem *ptr;
		//ptr = CDR(spares);
		/* count++; */
		/*if( count % 50000 == 0 )
			printf(".%d", count),fflush(stdout);*/
		
		kd_insert(newTree,(kd_generic)spares->item,spares->size,(kd_generic)spares);
		spares = (spares ? spares->sons[0]: (KDElem *)NULL) ;
	}
    return newTree;
}


KDElem *load_items(int (*itemfunc)(kd_generic arg, kd_generic *val, kd_box size), kd_generic arg, kd_box extent, int *length, double *mean)
// int (*itemfunc)();		/* Generate next item       */
// kd_generic arg;			/* State passed to itemfunc */
// kd_box extent;			/* Overall extent           */
// int *length;			/* List length (returned)   */
// double *mean;				/* Geometric mean of Left sides */
/*
 * This routine uses `itemfunc' to generate all of the items
 * that are to be loaded into the new k-d tree and places them
 * in a simple linked list.  This list will be used to build
 * a perfectly balanced initial tree.  The routine also
 * stores the size of all items and the overall size of
 * all items.  The routine will return zero if any of the
 * items returned have no size.
 */
{
    KDElem *new_list = (KDElem *) NULL;
    KDElem  *new_item;
    int add_flag = 1;

    *mean = 0;
    *length = 0;
    extent[KD_LEFT] = extent[KD_BOTTOM] = INT_MAX;
    extent[KD_RIGHT] = extent[KD_TOP] = INT_MIN;
    for (;;)
	{
	        new_item = (KDElem*)nco_malloc(sizeof(KDElem));
		if ((*itemfunc)(arg, &new_item->item, new_item->size))
		{
			if (!new_item->item) add_flag = 0;
			if (add_flag)
			{
				/* Add to list */
				if (new_item->size[KD_LEFT] < extent[KD_LEFT])
					extent[KD_LEFT] = new_item->size[KD_LEFT];
				if (new_item->size[KD_BOTTOM] < extent[KD_BOTTOM])
					extent[KD_BOTTOM] = new_item->size[KD_BOTTOM];
				if (new_item->size[KD_RIGHT] > extent[KD_RIGHT])
					extent[KD_RIGHT] = new_item->size[KD_RIGHT];
				if (new_item->size[KD_TOP] > extent[KD_TOP])
					extent[KD_TOP] = new_item->size[KD_TOP];
				
				//new_list = CONS(new_item, new_list);
				new_list= ( new_item->sons[0]=new_list, new_item);
				(*mean) += new_item->size[KD_LEFT];
				(*length)++;
			}
			else
			  // free(new_item);
			  new_item=(KDElem*)nco_free((void*)new_item);
		}
		else
		{
		        // free(new_item);
		  new_item=(KDElem*)nco_free((void*)new_item);
			break;
		}
    }
    if (!add_flag)
	{
		KDElem *ptr;
		
		/* Destroy list and return */
		/* WHY? why would you, if you got a non-zero result from itemfunc, but no
		   new item, would you destroy the entire list and return nothing? */
		while (new_list)
		{
			ptr = new_list;
			new_list = (new_list ? new_list->sons[0] : (KDElem*)NULL);
			ptr=(KDElem*)nco_free((void*)ptr);
			(*length)--;
		}
    }
	(*mean) /= (*length);
    return new_list;
}



KDElem *build_node(KDElem *items, int num, kd_box extent, int disc, int level, int max_level, KDElem **spares, int *treecount, double mean)
// KDElem *items;			/* Items to insert          */
// int num;			/* Number of items          */
// kd_box extent;			/* Extent of items          */
// int disc;			/* Discriminator            */
// int level;			/* To keep track of which level we're at.  */
// int max_level;		/* To help in limiting in the depth of the
// 					   balanced tree building  */
// KDElem **spares;   /* ptr to a list head to attach spares to */
// int *treecount;     /* keep a record of each node built */
// double mean;          /* the geometric mean of the data under the node based on disc number */
/*
 * This routine builds a new node by finding an approximate median of
 * the items according to the edge given by `disc' and
 * making that the node.  Items less than the median are
 * recursively placed in the lower son,  items greater
 * than the median are recursively placed in the upper son.
 * Bounds information is also updated.  The node is deleted
 * from the list once placed.
 */
{
    KDElem *loson, *hison;
    KDElem *lo, *eq, *hi;
    double lo_min_bound, lo_max_bound, hi_min_bound, hi_max_bound;
    int num_lo, num_hi;
    int hort;
    double tmp, m;
    double lomean, himean;
    long locnt,hicnt;
	
    if (num == 0) return (KDElem *) NULL;

    /* Find (disc)-median of items */
    hort = disc & 0x01;
/*    m = (extent[hort] + extent[hort+2]) >> 1;*/ /* this criteria will
	  use the geographic mean! */
    m = mean;
	
    sel_k(items, m, disc, &lo, &eq, &hi, &lomean, &himean, &locnt, &hicnt);

    /* If more than one median -- try to distinguish them */
    //if (CDR(eq)) resolve(&lo, &eq, &hi, disc,  &lomean, &himean, &locnt, &hicnt);
    if ( eq ? eq->sons[0] : (KDElem*)NULL ) 
       resolve(&lo, &eq, &hi, disc,  &lomean, &himean, &locnt, &hicnt);

    /* Find min-max boundaries based on discriminator */
    RCDR(eq, lo);
    num_lo = get_min_max(eq, disc, &lo_min_bound, &lo_max_bound) - 1;

    RCDR(eq, hi);
    num_hi = get_min_max(eq, disc, &hi_min_bound, &hi_max_bound) - 1;
	
	if( level < max_level )
	{
		if( lomean )
			lomean /= locnt;
		if( himean )
			himean /= hicnt;
		
		tmp = extent[hort+2];  extent[hort+2] = m;
		loson = build_node(lo, num_lo, extent, (disc+1)%4, level+1, max_level, spares, treecount, lomean);
		extent[hort+2] = tmp;

		tmp = extent[hort];    extent[hort] = m;
		hison = build_node(hi, num_hi, extent, (disc+1)%4, level+1, max_level, spares, treecount, himean);
		extent[hort] = tmp;
	}
	else
	{
		/* here, we need to take the lists of unused elements, and pass them back
		   up the calling sequence, or attach them to a global. */
		while( lo )
		{
			CMV(lo,*spares);
		}
		while( hi )
		{
			CMV(hi,*spares);
		}
		hison = loson = (KDElem *)0;
	}
  	
    /* Make new node with appropriate values */
	eq->lo_min_bound = lo_min_bound;
	eq->hi_max_bound = hi_max_bound;
	eq->other_bound = ((disc & 0x2) ? hi_min_bound : lo_max_bound);
	eq->sons[0] = loson;
	eq->sons[1] = hison;
	(*treecount)++;
    return eq;
}





#ifdef OLD_SELECT

void sel_k(items, k, disc, lo, eq, hi)
KDElem *items;			/* Items to examine                 */
int k;				/* Look for item close to `k'       */
int disc;			/* Discriminator                    */
KDElem **lo;			/* Returned items less than `k'th   */
KDElem **eq;			/* Returned items equal to `k'th    */
KDElem **hi;			/* Returned items larger than `k'th */
/*
 * This routine uses a heuristic to attempt to find a rough median
 * using an inline comparison function.  The routine takes a `target'
 * number `k' and places all items that are `disc'-less than `k' in
 * lo, equal in `eq', and greater in `hi'.
 */
{
     KDElem *idx;
     int cmp_val;
     KDElem *median;
    int lo_val;

    idx = items;
    lo_val = INT_MAX;
    /* First find closest to median value */
    while (idx) {
	cmp_val = KD_SIZE(idx)[disc] - k;
	cmp_val = KDABS(cmp_val);
	if (cmp_val < lo_val) {
	    median = idx;
	    lo_val = cmp_val;
	}
	idx = (idx ? (idx)->sons[0] : (KDElem *) NULL );
    }
    /* Now divide based on median */
    *lo = *eq = *hi = (KDElem *)NULL;
    idx = items;
    while (idx) {
	cmp_val = KD_SIZE(idx)[disc] - KD_SIZE(median)[disc];
	if (cmp_val < 0) {
	    CMV(idx, *lo);
	} else if (cmp_val > 0) {
	    CMV(idx, *hi);
	} else {
	    CMV(idx, *eq);
	}
    }
}
#endif


void sel_k(KDElem *items, double k, int disc, KDElem **lo, KDElem **eq, KDElem **hi, double *lomean, double *himean, long *locount, long *hicount)
// KDElem *items;			/* Items to examine                 */
// int k;				/* Look for item close to `k'       */
// int disc;			/* Discriminator                    */
// KDElem **lo;			/* Returned items less than `k'th   */
// KDElem **eq;			/* Returned items equal to `k'th    */
// KDElem **hi;			/* Returned items larger than `k'th */
// double *lomean,*himean;   /* the total values of all the Kj's */
// long *locount,*hicount; /* the counts to get an average     */
/*
 * This routine uses a heuristic to attempt to find a rough median
 * using an inline comparison function.  The routine takes a `target'
 * number `k' and places all items that are `disc'-less than `k' in
 * lo, equal in `eq', and greater in `hi'.
 */
{
      KDElem *idx, *median;
      double cmp_val;
    double lo_val;

    idx = items;
    *lo = *eq = *hi = (KDElem *)NULL;
	*lomean = *himean = 0.0;
	*locount = *hicount = 0;
    lo_val = KD_DBL_MAX;
    
    median = (KDElem *)NULL;
    while (idx)
	{
		/* Check to see if new median */
		cmp_val = KD_SIZE(idx)[disc] - k;
		if (KDABS(cmp_val) < lo_val)
		{
			lo_val = KDABS(cmp_val);
			median = idx;
			while (*eq)
			{
				cmp_val = KD_SIZE(*eq)[disc] - KD_SIZE(median)[disc];
				if (cmp_val < 0.0)
				{
					CMV(*eq, *lo);
					(*lomean) += KD_SIZE(*lo)[(disc+1)%4];
					(*locount)++;
				} else if (cmp_val > 0.0)
				{
					CMV(*eq, *hi);
					(*himean) += KD_SIZE(*hi)[(disc+1)%4];
					(*hicount)++;
				} else
				{
					(void) kd_fault(KDF_MD);
				}}
		}
		/* Place element in list */
		if (median)
		{
			cmp_val = KD_SIZE(idx)[disc] - KD_SIZE(median)[disc];
		}
		if (cmp_val < 0.0)
		{
			CMV(idx, *lo);
			(*lomean) += KD_SIZE(*lo)[(disc+1)%4];
			(*locount)++;
			
		} else if (cmp_val > 0.0)
		{
			CMV(idx, *hi);
			(*himean) += KD_SIZE(*hi)[(disc+1)%4];
			(*hicount)++;
		} else
		{
			CMV(idx, *eq);
		}
	}
}




void resolve(  KDElem **lo,   KDElem **eq,   KDElem **hi, int disc, double *lomean, double *himean, long *locount, long *hicount)
//   KDElem **lo, **eq, **hi; /* Lists for examination */
// int disc;
// double *lomean,*himean;   /* the total values of all the Kj's */
// long *locount,*hicount; /* the counts to get an average     */
/*
 * This routine is called if more than one possible median
 * was found.  The first is chosen as the actual median.
 * The rest are reclassified using cyclical comparison.
 */
/* correction: the rest are put to help balance lo and hi sides. */
{

    int cur_disc=0;
    double val=0.0;
    KDElem *others;
    
    others = (*eq ? (*eq)->sons[0] : (KDElem *) NULL );
    RCDR(*eq, (KDElem *)NULL);
    while (others)
	{
		cur_disc = (disc+1)%4; /* since all the eq's disc val are the same, maybe
									  if we use the next disc, we can get some random
									  numbers to seperate the goats from the sheep. */
		while (cur_disc != disc)
		{
			val = others->size[cur_disc] - (*eq)->size[cur_disc];
			if (val != 0.0) break;
			cur_disc = (cur_disc+1)%4;
			
		}
		if (val < 0.0)
		{
			CMV(others, *lo);
			(*lomean) += KD_SIZE(*lo)[(disc+1)%4];
			(*locount)++;
		} else
		{
			CMV(others, *hi);
			(*himean) += KD_SIZE(*hi)[(disc+1)%4];
			(*hicount)++;
		}
		/* this stuff could be a sensible criterion, but there's no way on earth
		   I can replicate the direction to go when I'm looking for a certain node!
		if( hicount > locount )
		{
			CMV(others, *lo);
			(*lomean) += KD_SIZE(*lo)[NEXTDISC(disc)];
			(*locount)++;
		}
		else
		{
			CMV(others, *hi);
			(*himean) += KD_SIZE(*hi)[NEXTDISC(disc)];
			(*hicount)++;
		} */
	}
}




int get_min_max(KDElem *list, int disc, double *b_min, double *b_max)
// KDElem *list;			/* List to examine */
// int disc;			/* Discriminator   */
// int *b_min;			/* Lower bound     */
// int *b_max;			/* Upper bound     */
/*
 * This routine examines all of the items in `list' and
 * finds the lowest and highest edges based on the discriminator
 * `disc'.  If the discriminator is 0 or 2,  the left and
 * right edges of the boxes are examined.  Otherwise, the
 * top and bottom edges are examined.  Returns the number
 * of items in the list.
 */
{
    KDElem *item;
    int count;

    *b_min = KD_DBL_MAX;
    *b_max = -(KD_DBL_MAX);

    disc = disc & 0x01;		/* zero: horizontal, one: vertical */
    count = 0;
    while (list) {
	item = list;
	if (item->size[disc] < *b_min) *b_min = item->size[disc];
	if (item->size[disc+2] > *b_max) *b_max = item->size[disc+2];
	list = (list ? (list)->sons[0] : (KDElem *) NULL );
	count++;
    }
    return count;
}



void del_elem(KDElem *elem, void (*delfunc)(kd_generic item))
// KDElem *elem;			/* Element to release */
// void (*delfunc)();		/* Free function      */
/*
 * Recursively releases resources associated with `elem'.
 * User data items are freed using `delfunc'.
 */
{
    int i;

    /* If the tree does not exist,  return normally */
    if (!elem) return;

    /* If there are children,  recursively destroy them first */
    for (i = 0;  i < 2;  i++) {
	del_elem(elem->sons[i], delfunc);
    }

    /* Now get rid of the rest of it */
    if (delfunc /* 18.02.98 Liburkin add this terrible:*/ && elem->item)
      (*delfunc)(elem->item);
    
    elem=(KDElem*)nco_free((void*)elem);
}

//void kd_destroy(kd_tree this_one, void (*delfunc)(kd_generic item))
void kd_destroy(KDTree *rootTree , void (*delfunc)(kd_generic item))
// kd_tree this_one;		/* k-d tree to destroy */
// void (*delfunc)();		/* Free function called on user data */
/*
 * This routine frees all resources associated with the
 * specified kd-tree.
 */
{
  if(rootTree->tree)
      del_elem(rootTree->tree, delfunc);
  
  rootTree = (KDTree*)nco_free((void*)rootTree);
  //del_elem(((KDTree *) this_one)->tree, delfunc);
    //this_one=(kd_tree)free(this_one);
    //this_one=(kd_tree)free(this_one);
   return;
}




/*
 * Insertion
 */


void kd_insert(KDTree* realTree, kd_generic data, kd_box size, kd_generic datas_elem)
// kd_tree theTree;		/* k-d tree for insertion */
// kd_generic data;		/* User supplied data     */
// kd_box size;			/* Size of item           */
// kd_generic datas_elem;		/* k-d tree for insertion */
/*
 * Inserts a new data item into the specified k-d tree.  The `data'
 * item cannot be zero.  This value is used internally by the package.
 * Fatal errors:
 *   KDF_ZEROID: attempt to insert an item with a null generic pointer.
 *   KDF_DUPL:   an exact duplicate is already in the tree.
 */
{
  //KDTree *realTree = (KDTree *) theTree;
	KDElem *elem = (KDElem *)datas_elem;
	
    if (!data) (void) kd_fault(KDF_ZEROID);
    if (realTree->tree)
    {
		if (find_item(realTree->tree, 0, data, size, 0, elem))
		{
			realTree->item_count += 1;
			if( size[KD_LEFT] < realTree->extent[KD_LEFT] ) /* the area doesn't contract with deletions,     */
				realTree->extent[KD_LEFT] = size[KD_LEFT];  /* but that's OK, it's theoretically no big deal */
			if( size[KD_RIGHT] > realTree->extent[KD_RIGHT] )
				realTree->extent[KD_RIGHT] = size[KD_RIGHT];
			if( size[KD_TOP] > realTree->extent[KD_TOP] )
				realTree->extent[KD_TOP] = size[KD_TOP];
			if( size[KD_BOTTOM] < realTree->extent[KD_BOTTOM] )
				realTree->extent[KD_BOTTOM] = size[KD_BOTTOM];
		}
		else
		  (void) kd_fault(KDF_DUPL);

    }
   else
   {
		if( elem )
		{
			realTree->tree = elem;
			realTree->tree->item = data;
			realTree->tree->size[0] = size[0];
			realTree->tree->size[1] = size[1];
			realTree->tree->size[2] = size[2];
			realTree->tree->size[3] = size[3];
			realTree->tree->lo_min_bound = size[0];
			realTree->tree->hi_max_bound = size[2];
			realTree->tree->other_bound = size[0];
			realTree->tree->sons[0] = 0;
			realTree->tree->sons[1] = 0;
		}
		else
			realTree->tree = kd_new_node(data, size, size[0], size[2], size[0], (KDElem *) NULL, (KDElem *) NULL);
		realTree->extent[0] = size[0];
		realTree->extent[1] = size[1];
		realTree->extent[2] = size[2];
		realTree->extent[3] = size[3];
		realTree->item_count += 1;
    }
}


/*
 * The find_item() routine optionally produces a path down to the
 * item in the following global dynamic array if search_p is true.
 * find_item() uses NEW_PATH to record the decent down the tree
 * until it finds the object.  It uses LAST_PATH to mark the
 * last item.
 */


void NEW_PATH(KDElem *elem)
{
	if (path_reset)
	{
		path_length = 0;
		path_reset = 0;
	}
	if (path_length >= path_alloc)
	{
		if (path_alloc == 0)
		{
			path_alloc = PATH_INIT;
			path_to_item = (KDElem**)nco_malloc( sizeof(  KDElem *)*path_alloc);
		}
		else
		{
			path_alloc += PATH_INCR;
			//path_to_item = REALLOC(KDElem *, path_to_item, path_alloc);
			path_to_item= (KDElem**)nco_realloc(path_to_item, sizeof(  KDElem *)*path_alloc);

		}
	}
	path_to_item[path_length++] = (elem);
}

/*
  #define NEW_PATH(elem)	\
  if (path_reset) { path_length = 0;  path_reset = 0; } \
  if (path_length >= path_alloc) { \
  if (path_alloc == 0) {path_alloc = PATH_INIT; path_to_item = MULTALLOC(KDElem *,path_alloc); } \
  else { path_alloc += PATH_INCR; \
  path_to_item = REALLOC(KDElem *, path_to_item, path_alloc);} \
  } \
  path_to_item[path_length++] = (elem)
*/


void kd_print_path(void) /* this routine is for debug */
{
	int i;
	for(i=0;i<path_length;i++)
	{
		KDElem *elem;
		elem = path_to_item[i];
		printf("%d: \tElem: %p [%p] lo=%f hi=%f, other=%f, size= \t(%f\t%f\t%f\t%f)  Loson:%p[%p]  HiSon:%p[%p]\n",
			   i,(void*)elem->item, (void*)elem,
			   elem->lo_min_bound, elem->hi_max_bound, elem->other_bound,
			   elem->size[0],elem->size[1],elem->size[2],elem->size[3],
               (void*)elem->sons[0], (void*)(elem->sons[0]?elem->sons[0]->item:NULL),
               (void*)elem->sons[1], (void*)(elem->sons[1]?elem->sons[1]->item:NULL));
	}
}




KDElem *find_item(KDElem *elem, int disc, kd_generic item, kd_box size, int search_p, KDElem *items_elem)
// KDElem *elem;			/* Search location */
// int disc;			/* Discriminator   */
// kd_generic item;		/* Item to insert  */
// kd_box size;			/* geographic Size of item    */
// int search_p;			/* Search or insert */
// KDElem *items_elem;		/* pre-malloc'd container for item */
/*
 * This routine either searches for or inserts `item'
 * into the node `elem'.  The size of `item' is passed
 * in as `size'.  The function for returning the size
 * of an item is `s_func'.  If `search_p' is non-zero,
 * the routine expects to find the item rather than
 * insert it.  The routine returns either the newly
 * created element or the element found (zero if
 * it couldn't be found).
 */
{
 
    int new_disc, vert;
    int ival;
    double val;
    KDElem *result;
    
    /* Compare current element against the one we are looking for */
    if ( !ALLOW_DUPLICATE_ITEM &&  item == elem->item)
	{
		if (search_p)
		{
		        /* global */
		        path_reset = 1;
			if (elem->item)
			   return elem;
			else
			  return (KDElem *) NULL;
		}
		else
			return (KDElem *) NULL;
    }
	else
	{
		/* Determine successor */
		val = size[disc] - elem->size[disc];
		if (val == 0.0)
		{
			/* Cyclical comparison required */
			new_disc = (disc+1)%4;
			while (new_disc != disc)
			{
				val = size[new_disc] - elem->size[new_disc];
				if (val != 0.0) break;
				new_disc = (new_disc+1)%4;
			}
			if (val == 0.0) val = 1.0; /* Force upward if equal */
		}
		ival = (val >= 0.0);
		
		if (elem->sons[ival])
		{
			if (search_p) NEW_PATH(elem);
			result = find_item(elem->sons[ival], (disc+1)%4, item, size, search_p, items_elem);
			/* Bounds update if insert */
			if (!search_p) bounds_update(elem, disc, size);
			/* ^ this is where we jump up the tree after insert and fix the
			   bounds above us in the tree */
			return result;
		}
		else if (search_p)
		{
		        /* global */
		        path_reset = 1;  
			return (KDElem *) NULL;
		}
		else
		{
			/* Insert here */
			vert = (disc+1)%4 & 0x01;
			if( items_elem )
			{
				elem->sons[ival] = items_elem;
				items_elem->size[0] = size[0];
				items_elem->size[1] = size[1];
				items_elem->size[2] = size[2];
				items_elem->size[3] = size[3];
				items_elem->lo_min_bound = size[vert];
				items_elem->hi_max_bound = size[vert + 2];
				items_elem->other_bound = (((disc + 1) % 4 & 0x2) ? size[vert] : size[vert + 2]);
				items_elem->sons[0] = 0;
				items_elem->sons[1] = 0;
				items_elem->item = item;

			}
			else
				elem->sons[ival] = kd_new_node(item, size, size[vert], size[vert+2], (((disc+1)%4 & 0x2) ? size[vert] : size[vert+2]), (KDElem *) NULL, (KDElem *) NULL);
			/* Bounds update */
			bounds_update(elem, disc, size);
			return elem->sons[ival];
		}
	}
}



void bounds_update(KDElem *elem, int disc, kd_box size)
// KDElem *elem;			/* Element to update */
// int disc;			/* Discriminator     */
// kd_box size;			/* Size of new item  */
/*
 * This routine updates the bounds information of `elem'
 * using `disc' and `size'.
 */
{
    int vert;

    vert = disc & 0x01;
    elem->lo_min_bound = MIN(elem->lo_min_bound, size[vert]);
    elem->hi_max_bound = MAX(elem->hi_max_bound, size[vert+2]);
    if (disc & 0x2) {
	/* hi_min_bound */
	elem->other_bound = MIN(elem->other_bound, size[vert]);
    } else {
	/* lo_max_bound */
	elem->other_bound = MAX(elem->other_bound, size[vert+2]);
    }
}



kd_status kd_is_member(KDTree *realTree, kd_generic data, kd_box size)
// kd_tree theTree;		/* Tree to examine  */
// kd_generic data;		/* Item to look for */
// kd_box size;			/* Original size    */
/*
 * Returns KD_OK if `data' is stored in tree `theTree' with
 * the location `size' and KD_NOTFOUND if not.
 */
{
  //KDTree *real_tree = (KDTree *) theTree;
    
    if (find_item(realTree->tree, 0, data, size, 1, 0)) {
	return KD_OK;
    } else {
	return KD_NOTFOUND;
    }
}


/*
 * Deletion
 *
 * Since the preconditions of a k-d tree are moderately complex,  the
 * deletion routine simply marks a node as deleted if it has sons.
 * If it doesn't have sons,  it can be fully deleted.  The routine
 * updates count information in the tree for eventual rebuild once
 * the `badness' of the tree exceeds a threshold.  This is not
 * implemented initially.
 *
 * Items are marked for deletion by setting the item pointer
 * to zero.  This means zero data items are not allowed.
 */

kd_status kd_delete(KDTree* realTree, kd_generic data, kd_box old_size)
// kd_tree theTree;		/* Tree to delete from  */
// kd_generic data;		/* Item to delete       */
// kd_box old_size;		/* Original size        */
/*
 * Deletes the specified item from the specified k-d tree.  `old_size'
 * must be provided to efficiently locate the item.  May return
 * KD_NOTFOUND if the item is not in the tree.
 */
{
  //KDTree *realTree = (KDTree *) theTree;
    KDElem *elem;

    elem = find_item(realTree->tree, 0, data, old_size, 1, 0);
    if (elem) {
	/* Delete element */
	elem->item = (kd_generic) NULL;
	(realTree->dead_count)++;
	return del_element(realTree, elem, path_length);
    } else {
	return KD_NOTFOUND;
    }
}



void kd_delete_stats(int *tries,int *levs)
{
	*tries = kddel_number_tried;
	*levs  = kddel_number_deld;
}

KDElem *kd_do_delete(KDTree *,KDElem *,int);

/* This routine and its subfuncs implement the recursive delete function
   described by JL Bentley on p. 515 of his article in the Comm. of the ACM,
   Sept 1975, Vol 18, No. 9.

   On the average, the comments about it's efficiency are correct. On the average,
   it's darned fast. Very few nodes affected. Searches for replacements are very
   short.

   
   Steve Murphy,  1990
   
   */

kd_status kd_really_delete(KDTree* realTree, kd_generic data, kd_box old_size, int *num_tries, int *num_del)
// kd_tree theTree;		/* Tree to delete from  */
// kd_generic data;		/* Item to delete       */
// kd_box old_size;		/* Original size        */
// int *num_tries, *num_del; /* stats returned about how much work it took */
/*
 * Deletes the specified item from the specified k-d tree.  `old_size'
 * must be provided to efficiently locate the item.  May return
 * KD_NOTFOUND if the item is not in the tree.
 */
{
     // KDTree *realTree = (KDTree *) theTree;
    KDElem *elem,*elemdad,*newelem;
    int j;
    kddel_number_tried = 0;
    kddel_number_deld = 1;
	
    elem = find_item(realTree->tree, 0, data, old_size, 1,0);
    if (elem)
	{
		elemdad = path_to_item[path_length-1];
		/* Delete element */
		j = path_length % 4;
		newelem = kd_do_delete(realTree,elem,j);
		if( elemdad->sons[KD_HISON] == elem )
			elemdad->sons[KD_HISON] = newelem;
		else
			elemdad->sons[KD_LOSON] = newelem;
		
		//FREE(elem);
		elem=(KDElem*)nco_free((void*)elem);
		realTree->item_count--;
	}
	else
	{
		*num_tries = 0;
		*num_del = 0;
		return KD_NOTFOUND;
	}
	*num_tries = kddel_number_tried;
	*num_del = kddel_number_deld;
	return KD_OK;
}

KDElem *kd_do_delete(KDTree *real_tree, KDElem *elem, int j)
// KDTree *real_tree;		/* Tree to delete from  */
// KDElem *elem;           /* element to delete */
// int j;                  /* j is the disc of elem */
/*
 * Deletes the specified item from the specified k-d tree.  `old_size'
 * must be provided to efficiently locate the item.  May return
 * KD_NOTFOUND if the item is not in the tree.
 */
{
	KDElem *Q,*Qdad;
	int Qson;
	static char flip = 0;

	flip = !flip;
	
	/* Delete element */
	if( !elem->sons[KD_HISON] && !elem->sons[KD_LOSON])
	{
		return 0;
	}
	else
	{
		int newj;

		Qdad = elem;
		if( !elem->sons[KD_HISON])
			flip = 0;
		else if( !elem->sons[KD_LOSON] )

		  flip = 1;
		if( !flip ) /* loson */
		{
			Q = elem->sons[KD_LOSON];
			Qson = KD_LOSON;
			newj = (j+1)%4;
			kddel_number_tried += find_min_max_node(j,&Q,&Qdad,&Qson,&newj);
		}
		else  /* hison */
		{
			Q = elem->sons[KD_HISON];
			Qson = KD_HISON;
			newj = (j+1)%4;
			kddel_number_tried += find_min_max_node(j,&Q,&Qdad,&Qson,&newj);
		}
		Qdad->sons[Qson] = kd_do_delete(real_tree, Q, newj);
		kddel_number_deld++;
		Q->sons[KD_LOSON] = elem->sons[KD_LOSON];
		Q->sons[KD_HISON] = elem->sons[KD_HISON];
		Q->lo_min_bound = elem->lo_min_bound; /* you have to inherit the bounds information as well */
		Q->other_bound = elem->other_bound;
		Q->hi_max_bound = elem->hi_max_bound;
		/* fprintf(stderr,"<del=%d>",(int)(Q->item)+1); */
		return Q;
	}
}




kd_status del_element(KDTree *tree, KDElem *elem, int spot)
// KDTree *tree;			/* Tree              */
// KDElem *elem;			/* Item to delete    */
// int spot;			/* Last item in path */
/*
 * This routine deletes `elem' from its tree.  It assumes that
 * the path information down to the element is stored in
 * path_to_item (see find_item() for details).  If the node
 * has no children,  it is deleted and the counts are modified
 * appropriately.  The routine is called recursively on its
 * parent.  If it has children,  it is left and the recursion
 * stops.
 */
{
    if (elem->item == (kd_generic) NULL)
	{
		if (!elem->sons[KD_LOSON] && !elem->sons[KD_HISON])
		{
			if (spot > 0)
			{
				if (path_to_item[spot-1]->sons[KD_LOSON] == elem)
				{
					path_to_item[--spot]->sons[KD_LOSON] = (KDElem *) 0;
				} else if (path_to_item[spot-1]->sons[KD_HISON] == elem)
				{
					path_to_item[--spot]->sons[KD_HISON] = (KDElem *) 0;
				} else
				{
					(void) kd_fault(KDF_F);
				}
				//FREE(elem);
				elem=(KDElem*)nco_free((void*)elem);
				(tree->dead_count)--;
				(tree->item_count)--;
				return del_element(tree, path_to_item[spot], spot);
			} else
			{
				tree->tree = (KDElem *) NULL;
				//FREE(elem);
				elem=(KDElem*)nco_free((void*)elem);
				(tree->dead_count)--;
				(tree->item_count)--;
				return KD_OK;
			}
		}
    }
    /* No work required */
    return KD_OK;
}



/*
 * Generation of items
 */


void kd_push(KDState *gen, KDElem *elem, short disc)
// KDState *gen;			/* Generator       */
// KDElem *elem;			/* Element to push */
// short disc;			/* Discriminator   */
/*
 * This routine pushes a new kd-tree element onto the generation
 * stack.  It also initializes the generation of items in
 * this element.
 */
{
    /* Allocate more space if necessary */
    if (gen->top_index >= gen->stack_size) {
	gen->stack_size += KD_GROWSIZE(gen->stack_size);
	gen->stk = (KDSave*)nco_realloc( gen->stk, sizeof(KDSave)*gen->stack_size);
    }
    gen->stk[gen->top_index].disc = disc;
    gen->stk[gen->top_index].state = KD_THIS_ONE;
    gen->stk[gen->top_index].item = elem;
    gen->top_index += 1;

    return;
}

#define FUNC_kd_pushb 1

#ifdef FUNC_kd_pushb

void kd_pushb(KDState *gen, KDElem* elem, short dk, kd_box Bxn, kd_box Bxp)
{ 
    if ((gen)->top_index >= (gen)->stack_size) {                     
	(gen)->stack_size += KD_GROWSIZE((gen)->stack_size);	     
	(gen)->stk = (KDSave*)nco_realloc( gen->stk, sizeof(KDSave)*gen->stack_size); 
    }
    
    (gen)->stk[(gen)->top_index].disc = (dk);		     	     
    (gen)->stk[(gen)->top_index].state = KD_THIS_ONE;		     
    (gen)->stk[(gen)->top_index].item = (elem);			     
    (gen)->stk[(gen)->top_index].Bn[0] = Bxn[0];			     
    (gen)->stk[(gen)->top_index].Bn[1] = Bxn[1];			     
    (gen)->stk[(gen)->top_index].Bn[2] = Bxn[2];			     
    (gen)->stk[(gen)->top_index].Bn[3] = Bxn[3];			     
    (gen)->stk[(gen)->top_index].Bp[0] = Bxp[0];			     
    (gen)->stk[(gen)->top_index].Bp[1] = Bxp[1];			     
    (gen)->stk[(gen)->top_index].Bp[2] = Bxp[2];			     
    (gen)->stk[(gen)->top_index].Bp[3] = Bxp[3];			     
    (gen)->top_index += 1;

    return;   
}

#else

#define kd_pushb(gen, elem, dk, Bxn, Bxp) \
    if ((gen)->top_index >= (gen)->stack_size) {                     \
	(gen)->stack_size += KD_GROWSIZE((gen)->stack_size);	     \
	(gen)->stk = (KDSave*)nco_realloc( gen->stk, sizeof(KDSave)*gen->stack_size); \
    }								     \
    (gen)->stk[(gen)->top_index].disc = (dk);		     	     \
    (gen)->stk[(gen)->top_index].state = KD_THIS_ONE;		     \
    (gen)->stk[(gen)->top_index].item = (elem);			     \
    (gen)->stk[(gen)->top_index].Bn[0] = Bxn[0];			     \
    (gen)->stk[(gen)->top_index].Bn[1] = Bxn[1];			     \
    (gen)->stk[(gen)->top_index].Bn[2] = Bxn[2];			     \
    (gen)->stk[(gen)->top_index].Bn[3] = Bxn[3];			     \
    (gen)->stk[(gen)->top_index].Bp[0] = Bxp[0];			     \
    (gen)->stk[(gen)->top_index].Bp[1] = Bxp[1];			     \
    (gen)->stk[(gen)->top_index].Bp[2] = Bxp[2];			     \
    (gen)->stk[(gen)->top_index].Bp[3] = Bxp[3];			     \
    (gen)->top_index += 1

#endif





kd_gen kd_start(KDTree*  realTree, kd_box area)
// kd_tree theTree;		/* Tree to generate from */
// kd_box area;			/* Area to search 	 */
/*
 * This routine allocates a generator which can be used
 * to generate all items intersecting `area'.
 * The items are actually returned by kd_next.  Once the
 * sequence is finished,  kd_end should be called.
 */
{
    //KDElem *realTree = ((KDTree *) theTree)->tree;
    KDState *newState;
    int idx;

    newState = (KDState*)nco_malloc(sizeof(KDState));

    kd_data_tries = 0;
    for (idx = 0;  idx < KD_BOX_MAX;  idx++)
      newState->extent[idx] = area[idx];

    newState->stack_size = KD_INIT_STACK;
    newState->top_index = 0;
    newState->stk = (KDSave*)nco_malloc(sizeof(KDSave) *  KD_INIT_STACK);

    /* Initialize search state */
    if (realTree)
	{
		kd_push(newState, realTree->tree, 0);
    }
	else
	{
		newState->top_index = -1;
    }
    return (kd_gen) newState;
}


kd_status kd_next(kd_gen theGen, kd_generic *data, kd_box size)
// kd_gen theGen;			/* Current generator */
// kd_generic *data;		/* Returned data     */
// kd_box size;			/* Optional size     */
/*
 * Returns the next item in the generator sequence.  If
 * `size' is non-zero,  it will be filled with the item's
 * size.  If there are no more items,  it returns KD_NOMORE.
 */
{
      KDState *realGen = (KDState *) theGen;
      KDSave *top_elem;
      KDElem *top_item;
    short hort,m;

    while (realGen->top_index > 0) {
	top_elem = &(realGen->stk[realGen->top_index-1]);
	top_item = top_elem->item;
	hort = top_elem->disc & 0x01;/* the split line is zero: vertical, one: horizontal */
	m = top_elem->disc;
	
	switch (top_elem->state) {
	case KD_THIS_ONE:
		/* Check this one */
		kd_data_tries++;
	
	    if (top_item->item && BOXINTERSECT(realGen->extent, top_item->size)) {
		*data = top_item->item;
		if (size) {
		    size[0] = top_item->size[0];  size[1] = top_item->size[1];
		    size[2] = top_item->size[2];  size[3] = top_item->size[3];
		}
		top_elem->state += 1;
		return KD_OK;
	    } else {
		top_elem->state += 1;
	    }
	    break;
		/* bounds explanation: remember that the bounds info is extents info: Left and Right, top and bottom, are
		   stored in 3 numbers. Why?, well, think about it: consider the case were disc = 0. We have a vertical line
		   splitting the universe, and boxes spread around, SOME OVERLAPPING THE LINE. disc=0 says, all those boxes
		   with their left sides to the left of my line,  will be located in the LOSON subtree. The bounds information
		   is split into 4 numbers: The bounds of the boxes in the LOSON tree and the bounds of the boxes in the HISON
		   tree, min and max on both sides. The right subtree is easy: No box can have a left side overlapping our line.
		   So the min for the right side  is the key[disc] value. The max for the right is the RIGHTmost RIGHT side of all
		   the boxes in the HISON subtree. The LOSON tree is a little trickier. the lominbound is the LEFTMOST left side of
		   all the boxes in that subtree. The low side's max bound is the RIGHTMOST right side of all the boxes in the
		   LOSON subtree -- WHICH could be GREATER than the disc. key! This number is "otherbound", in all cases. */
		/* alternatively, when looking at the HISON, and RIGHT discriminators, the LEFT sides can stretch over the
		   division line, and otherbound ends up being the LEFTmost LEFT side of all the boxes in the subtree. */
	case KD_LOSON:
	    /* See if we push on the loson */
	    if (top_item->sons[KD_LOSON] &&
		((m & 0x02) ?			/* RIGHT or TOP */
		 ((realGen->extent[hort] <= top_item->size[m]) && /* LEFT or BOTTOM of region less thn key (an upper bound for left)*/
		  (realGen->extent[hort+2] >= top_item->lo_min_bound)) /* RIGHT or TOP grthan lominbound */
		 :						/* LEFT or BOTTOM */
		 ((realGen->extent[hort] <= top_item->other_bound) && /* LEFT or BOTTOM of reg lessthan obound */
		  (realGen->extent[hort+2] >= top_item->lo_min_bound)))) /* RIGHT or TOP grthan lominbound */
		{
			top_elem->state += 1;
			kd_push(realGen, top_item->sons[KD_LOSON], (m+1)%4);
	    } else
		{
			top_elem->state += 1;
	    }
	    break;
	case KD_HISON:
	    /* See if we push on the hison */
	    if (top_item->sons[KD_HISON] &&
		((m & 0x02) ?			/* RIGHT or TOP */
		 ((realGen->extent[hort] <= top_item->hi_max_bound) && /* LEFT or BOTTOM of region lessthan himax */
		  (realGen->extent[hort+2] >= top_item->other_bound)) /* RIGHT or TOP grthan obound */
		 :						/* LEFT or BOTTOM */
		 ((realGen->extent[hort] <= top_item->hi_max_bound) && /* LEFT or BOTTOM of region lessthn himax */
		  (realGen->extent[hort+2] >= top_item->size[m])))) /* RIGHT or TOP grthan key (a minimum for the right side*/
		{
			top_elem->state += 1;
			kd_push(realGen, top_item->sons[KD_HISON], (m+1)%4);
	    } else
		{
			top_elem->state += 1;
	    }
	    break;
	default:
	    /* We have exhausted this node -- pop off the next one */
	    realGen->top_index -= 1;
	    break;
	}
    }
    return KD_NOMORE;
}


int kd_finish(kd_gen theGen)
// kd_gen theGen;			/* Generator to destroy */
/*
 * Frees resources consumed by the specified generator.
 * This routine is NOT automatically called at the end
 * of a sequence.  Thus,  the user should ALWAYS calls
 * kd_finish to end a generation sequence.
 */
{
    KDState *realGen = (KDState *) theGen;

    nco_free(realGen->stk);
    nco_free(realGen);
    
   return kd_data_tries;
}



int kd_count
(KDTree* tree)
// kd_tree tree;
/* Returns the number of items in the specified tree */
{
   return (tree->item_count - tree->dead_count);
}




void pr_tree(KDElem *elem, int disc, int depth)
// KDElem *elem;
// int disc;
// int depth;
{
    int i;

    for (i = 0;  i < depth;  i++)
      putchar(' ');
    
    printf("%p: %.14f %.14f %.14f (", (void*)elem->item, elem->lo_min_bound, elem->other_bound, elem->hi_max_bound);
    
    for (i = 0;  i < KD_BOX_MAX;  i++) {
	if (i == disc) putchar('*');
	printf("%.14f ", elem->size[i]);
    }
    
    printf(")\n");
    for (i = 0;  i < 2;  i++)
	{
		if (elem->sons[i])
		  {     printf("%c ", (i ? 'H' : 'L' ));    
			pr_tree (elem->sons[i], (disc+1)%4, depth+3);
		}
    }
}

void kd_print(KDTree * realTree)
/* Attempts to print out the tree assuming 160 characters across */
{
     // KDTree *realTree = (KDTree *) tree;
  if(realTree->tree)   
    pr_tree(realTree->tree, 0, 0);
}




/* ************** kd_tree_badness -- some metrics of tree health   ************************************** */
/* Coded by Steve Murphy,  Sept 1990                                                  */


void kd_tree_badness_level(KDElem *elem, int level)
{
    if( !elem )
	return;
    if( (elem->sons[1] || elem->sons[0]) && !(elem->sons[1] && elem->sons[0]))
	{
		kd_tree_badness_factor3++;
	}
	if( level > kd_tree_max_levels )
		kd_tree_max_levels = level;
	
	if( elem->sons[0] )
		kd_tree_badness_level(elem->sons[0], level+1);
	if( elem->sons[1] )
		kd_tree_badness_level(elem->sons[1], level+1);
}

/* right now, fact1 is not used. fact2 is the ratio of current tree max depth to minimum possible depth with number
   of nodes tree contains. fact3 is the number of nodes with one son. levs is the max depth.*/
void kd_tree_badness(KDTree *tree, double *fact1, double *fact2, double *fact3, int *levs)
{
	double targdepth,log(double),floor(double);
	kd_tree_badness_factor1 = 0.0;
	kd_tree_badness_factor2 = 0.0;
	kd_tree_badness_factor3 = 0.0;
	kd_tree_max_levels = 0;
	targdepth = tree->item_count;
	targdepth = log(targdepth)/log(2.0);
	targdepth = floor(targdepth);
	targdepth++;
	kd_tree_badness_level(tree->tree,1);
	*fact1 = kd_tree_badness_factor1;
	targdepth = (double)kd_tree_max_levels/targdepth;
	*fact2 = targdepth;
	*fact3 = kd_tree_badness_factor3;
	*levs = kd_tree_max_levels;
}

void kd_badness(KDTree *realTree)
{
	int lev=0;
	double a2=0.0,a3=0.0,a4=0.0,a5=0.0;


	a4 = 100.00 * a3 / (double)realTree->item_count ;
	a5 = 100.00 * (double)realTree->dead_count / (double)realTree->item_count;
	
	fprintf(stdout,"balance ratio=%g (the closer to 1.0, the better), #of nodes with only one branch=%g (%g), max depth=%d, dead=%d (%g)\n",
		        a2,a3,a4,lev, realTree->dead_count, a5);
}



/* ************** kd_rebuild -- functions to rebuild a tree       ********************************** */
/* Coded by Steve Murphy, Sept 1990                                                  */
/* NOT TESTED YET!  */

KDTree* kd_rebuild(KDTree* newTree)
{
     // KDTree *newTree= Tree;
	
    KDElem *items =  (KDElem *)NULL;
    KDElem *spares = (KDElem *)NULL;
    kd_box extent;

    long item_count = 0;
    
    double mean=0.0;
	/* rip the tree apart, discarding dead nodes, and rebuild it */

    /* First build up list of items and their overall extent */
    unload_items(newTree, &items, newTree->extent, &item_count, &mean);
	

	/* rebuild the tree */
    if (!items)
	{
		return newTree;
    }

    /* Then recursively fill the tree */
	if( kd_build_depth )
	{
		newTree->tree = build_node(items, item_count, extent, 0, 1, kd_build_depth, &spares, &(newTree->item_count), mean);
		newTree->items_balanced = newTree->item_count;
	}
	else
	{
		spares = items;
	}

	
	while( spares )
	{
		KDElem *ptr;
		ptr = (spares ? (spares)->sons[0] : (KDElem *) NULL );
		/* count++; */
		/*if( count % 50000 == 0 )
			printf(".%d", count),fflush(stdout);*/
		
		kd_insert(newTree,(kd_generic)spares->item,spares->size,(kd_generic)spares);
		spares = ptr;
	}
    return newTree;
}

void unload_items(KDTree* rootTree, KDElem **nodelist, kd_box extent, long *items, double *mean)
{
	/* traverse the tree and collect the nodes bottom-up into a single list; delete
	   dead nodes, freeing them */
        extent[KD_LEFT] =   KD_DBL_MAX;
	extent[KD_BOTTOM] = KD_DBL_MAX;
	extent[KD_RIGHT] =  -KD_DBL_MAX;
	extent[KD_TOP] =    -KD_DBL_MAX;
	
	
	collect_nodes(rootTree, rootTree->tree, nodelist, extent, items, mean);

	*mean /= *items;
}

void collect_nodes(KDTree* tree, KDElem *nodeptr, KDElem **nodelist, kd_box extent, long *items, double *mean)
{
        //KDTree *tree = (KDTree *)atree;
	
	if( nodeptr->sons[KD_LOSON] )
		collect_nodes(tree,(KDElem *)nodeptr->sons[KD_LOSON],nodelist,extent,items,mean);
	if( nodeptr->sons[KD_HISON] )
		collect_nodes(tree,(KDElem *)nodeptr->sons[KD_HISON],nodelist,extent,items,mean);
	/* Here I am at a son with no kids, sort of */
	
	if( ! nodeptr->item ) /* a dead node */
	{
		/* free it and move on */
		free((char *)nodeptr);
		tree->dead_count--;
		tree->item_count--;
	}
	else
	{
		/* add to the list */
		nodeptr->sons[0] = *nodelist;
		*nodelist = nodeptr;
		/* make sure the leftover ptr is nil */
		nodeptr->sons[1] = 0;
		/* the tree is a node smaller */
		tree->item_count--;
		/* recalc the extents using this node */
		if( nodeptr->size[KD_LEFT] < extent[KD_LEFT])
			extent[KD_LEFT] = nodeptr->size[KD_LEFT];
		if (nodeptr->size[KD_BOTTOM] < extent[KD_BOTTOM])
			extent[KD_BOTTOM] = nodeptr->size[KD_BOTTOM];
		if (nodeptr->size[KD_RIGHT] > extent[KD_RIGHT])
			extent[KD_RIGHT] = nodeptr->size[KD_RIGHT];
		if (nodeptr->size[KD_TOP] > extent[KD_TOP])
			extent[KD_TOP] = nodeptr->size[KD_TOP];
		/* calc the passed in values to help rebuild the tree */
		(*items)++;
		(*mean) += nodeptr->size[KD_LEFT];
	}
}


/* ************** find_min_max_node  -- for "real" deletion of a node in a kd-tree ***************** */
/* Coded by Steve Murphy, Sept 1990                                                  */

int nodecmp(KDElem *a, KDElem *b, int disc)
{
         // int ival;
	int new_disc;
	double val;
	
	val = a->size[disc] - b->size[disc];
	if (val == 0.0)
	{
		/* Cyclical comparison required */
		new_disc = (disc+1)%4;
		while (new_disc != disc)
		{
			val = a->size[new_disc] - b->size[new_disc];
			if (val != 0.0) break;
			new_disc = (new_disc+1)%4;
		}
		if (val == 0.0) val = 1.0; /* Force upward if equal */
	}
	//ival = (val >= 0);
	/* val = 0 means a<b, 1= a>b */
	return (val >=0.0);
}

int find_min_max_node(int j, KDElem **kd_minval_node, KDElem **kd_minval_nodesdad, int *dir, int *newj)
// KDElem **kd_minval_node,**kd_minval_nodesdad; /* q is the maximum loson, or the minimum hison, depending on dir, and qdad is q's dad. */
// int j,*dir,*newj; /* j is the discriminator index (0-3), dir is HISON or LOSON (qdad[dir]==q. newj is minval_node's disc*/
{

    KDState *realGen;
	
    realGen = (KDState*)nco_malloc(sizeof(KDState));
	
    kd_data_tries = 0;
	
    realGen->stack_size = KD_INIT_STACK;
    realGen->top_index = 0;
    realGen->stk = (KDSave*)malloc(sizeof(KDSave)* KD_INIT_STACK);

    /* Initialize search state */
    if (*kd_minval_node)
	{
		kd_push(realGen, *kd_minval_node, (j+1)%4);
    }
	else
	{
		realGen->top_index = -1;
    }

	if( *dir == KD_HISON ) /* we are trying to find the smallest value of k[j], in the HISON subtree; */
	{
		  KDSave *top_elem;
		  KDElem *top_item;
		short m;



		while (realGen->top_index > 0)
		{
			top_elem = &(realGen->stk[realGen->top_index-1]);
			top_item = top_elem->item;

			m = top_elem->disc;
				
			switch (top_elem->state)
			{
			case KD_THIS_ONE:
				/* Check this one */
				kd_data_tries++;
				
				if (top_item->item && !nodecmp(top_item,*kd_minval_node,j) && top_item != *kd_minval_node)
				{				/* when items have equal discriminators, choose the deepest to the left */
					*kd_minval_node = top_item;
					*kd_minval_nodesdad = realGen->stk[realGen->top_index-2].item;
					if( *kd_minval_node == (*kd_minval_nodesdad)->sons[KD_LOSON] )
						*dir = KD_LOSON;
					else
						*dir = KD_HISON;
					*newj = m;

					top_elem->state += 1;
				} else
				{
					top_elem->state += 1;
				}
				break;
			case KD_LOSON: /* we would always take the loson, to find a minimum value. */
				/* See if we push on the loson */
				if( top_item->sons[KD_LOSON] )
				{
					top_elem->state += 1;
					kd_push(realGen, top_item->sons[KD_LOSON], (m+1)%4);
				}
				else
					top_elem->state += 1;
				break;
			case KD_HISON: /* we can disqualify the hison iff j == m && top_item->size[m] > (*kd_minval_node)->size[m] */
				/* See if we push on the hison */
				if (j == m && top_item->size[m] > (*kd_minval_node)->size[m])
				{
					top_elem->state += 1;
				}
				else
				{
					if( top_item->sons[KD_HISON] )
					{
						top_elem->state += 1;
						kd_push(realGen, top_item->sons[KD_HISON], (m+1)%4);
					}
					else
						top_elem->state += 1;
				}
				break;
			default:
				/* We have exhausted this node -- pop off the next one */
				realGen->top_index -= 1;
				break;
			}
		}
		nco_free(realGen->stk);
		nco_free(realGen);
		return kd_data_tries;
	}
	else /* we are trying to find the maximal value of k[j], in the LOSON subree. */
	{
		  KDSave *top_elem;
		  KDElem *top_item;
		short m;


		while (realGen->top_index > 0)
		{
			top_elem = &(realGen->stk[realGen->top_index-1]);
			top_item = top_elem->item;

			m = top_elem->disc;
			
			switch (top_elem->state)
			{
			case KD_THIS_ONE:
				/* Check this one */
				kd_data_tries++;
				
				if (top_item->item && nodecmp(top_item,*kd_minval_node,j) && top_item != *kd_minval_node)
				{				/* when items have equal discriminators, choose the deepest to the right */
					*kd_minval_node = top_item;
					*kd_minval_nodesdad = realGen->stk[realGen->top_index-2].item;

					if( *kd_minval_node == (*kd_minval_nodesdad)->sons[KD_LOSON] )
						*dir = KD_LOSON;
					else
						*dir = KD_HISON;
					*newj = m;
					top_elem->state += 1;
				} else
				{
					top_elem->state += 1;
				}
				break;
			case KD_LOSON: /* we can disqualify the loson iff j == m && top_item->size[m] < (*kd_minval_node)->size[m] */
				if (j == m && top_item->size[m] < (*kd_minval_node)->size[m] )
				{
					top_elem->state += 1;
				}
				else
				{
					if( top_item->sons[KD_LOSON] )
					{
						top_elem->state += 1;
						kd_push(realGen, top_item->sons[KD_LOSON], (m+1)%4);
					}
					else
						top_elem->state += 1;
				}
				break;
			case KD_HISON: /* we would always take the hison, to find a maximum value. */
				if( top_item->sons[KD_HISON] )
				{
					top_elem->state += 1;
					kd_push(realGen, top_item->sons[KD_HISON], (m+1)%4);
				}
				else
					top_elem->state += 1;
				break;
			default:
				/* We have exhausted this node -- pop off the next one */
				realGen->top_index -= 1;
				break;
			}
		}
		nco_free(realGen->stk);
		nco_free(realGen);
		return kd_data_tries;
	}
}

/* ************************** Nearest neighbor ***************************** */
/* Coded by Steve Murphy, Sept 1990                          */

/* This is AN implementation of the algorithm described in the article "An Algorithm for finding
   Best Matches in Logarithmic Expected Time", by Jerome H. Friedman, Jon Louis Bentley, and
   Raphael Ari Finkel, SLAC-PUB-1549 (Rev.) STAN-CS-75-482, Feb 1975, Revised Dec. 1975,
   Revised July 1976. It notes that it was submitted to ACM Transactions on Mathematical Software.
   The basic structure of the kd-tree was different than that implemented here, therefore the
   algorithm had to be adapted, and the routines as described had some logical inconsistencies.
   But the basic idea is SLICK, and the heart of the algorithm still beats strong in this code.
   It does quickly pull out the closest things, searching a very small number of nodes.

   The assumption is that the routine will be requested to find a fairly small number of nodes.
   If that assumption is false, a faster sorting algorithm in the Priority list would be necc.

   The basic assumption is that you start with the root node of the tree, and calc it's distance (which,
   by the way, I do by calculating the distance squared as a metric) to the point specified (which, also,
   I do, by taking the centroid of the box of the node, and calculate distances point-to-point).

   This distance forms a "ball" of radius d, which, if subtrees are not overlapped by it, eliminates the
   subtrees from further search. As closer nodes are found, the Ball decreases in size. To make the
   Ball shrink in size as quickly as possible, the depth-first descender will traverse those branches
   first that would lie closest to the point specified. Since we can get the n closest objects,
   The ball is the size the nth-closest object (The one furthest away). Therefore, the fewer the
   objects to collect, the quicker the convergence (on the average). Also, I'm using doubles because
   Ints can easily overflow when you use d-squared type numbers. */

extern double hav(double x1, double x2);
inline double hav(double x1, double x2)
{
  double h=sin((x1-x2)/2.0);
  return h*h;
}

double KDdist(kd_box Xq, KDElem *elem)
{
    double hyp=0.0;

    switch(kd_pl_typ)
	{

	  case poly_none:
	    break;


    	/* use 2D cartesian distance */
	  case poly_crt:
	  case poly_rll:
		hyp=hypot(  Xq[KD_LEFT] - ((poly_sct*)elem->item)->dp_x_ctr, Xq[KD_TOP] -  ((poly_sct*)elem->item)->dp_y_ctr   );

    	break;


	  case poly_sph:
	  {
	  	double x1;
	  	double y1;
	  	double x2;
	  	double y2;


        x1=((poly_sct*)elem->item)->dp_x_ctr *M_PI / 180.0;
        y1=((poly_sct*)elem->item)->dp_y_ctr *M_PI / 180.0;


        /* convert to radians */
        x2=Xq[KD_LEFT] * M_PI/180.0;
        y2=Xq[KD_TOP]  * M_PI/180.0;

        /* orthodromic distance - classic formula */
        //hyp=acos( sin(y1)*sin(y2) +  cos(y1)*cos(y2)*cos( x1-x2 )  );

        /* haversine formula */
        hyp = 2.0* asin( sqrt(  hav(y1,y2)  + cos(y1)*cos(y2)* hav(x1,x2) )  );



	  }
	  break;



	} /* end switch */



   return hyp;




  // double hypot();
	/* The following calcs edge-to-edge distance on bounding boxes. I could
	   imagine that this would not be good enough for stuff like all-angle
	   paths. but for totally orthogonal geoms, this would mostly suffice. */
	if( Xq[KD_LEFT] > elem->size[KD_RIGHT] ) /* off left side */
	{
		if(Xq[KD_TOP] < elem->size[KD_BOTTOM]) /* ob2 is in ul quad */
		{/* dist (Xq[KD_LEFT],Xq[KD_TOP])->(elem->size[KD_BOTTOM,RIGHT])*/
			return hypot((Xq[KD_LEFT]-elem->size[KD_RIGHT]), (Xq[KD_TOP]-elem->size[KD_BOTTOM])); 
		}
		else if(Xq[KD_BOTTOM] > elem->size[KD_TOP]) /* ob2 is in ll quad */
		{
			/* dist (Xq[KD_LEFT],Xq[KD_BOTTOM])->(elem->size[KD_TOP,RIGHT])*/
			return hypot((Xq[KD_LEFT]-elem->size[KD_RIGHT]), (Xq[KD_BOTTOM]-elem->size[KD_TOP])); 
		}
		else  /* shadowed to left */
			return (Xq[KD_LEFT]-elem->size[KD_RIGHT]);
	}
	else if (Xq[KD_RIGHT] < elem->size[KD_LEFT])
	{
		if(Xq[KD_TOP] < elem->size[KD_BOTTOM]) /* ob2 is in ur quad */
		{
			/* dist (Xq[KD_RIGHT],Xq[KD_TOP])->(elem->size[KD_BOTTOM,LEFT])*/
			return hypot((Xq[KD_RIGHT]-elem->size[KD_LEFT]), (Xq[KD_TOP]-elem->size[KD_BOTTOM])); 
		}
		else if(Xq[KD_BOTTOM] > elem->size[KD_TOP]) /* ob2 is in lr quad */
		{
			/* dist (Xq[KD_RIGHT],Xq[KD_BOTTOM])->(elem->size[KD_TOP,LEFT])*/
			return hypot((Xq[KD_RIGHT]-elem->size[KD_LEFT]), (Xq[KD_BOTTOM]-elem->size[KD_TOP])); 
		}
		else  /* shadowed to right */
			return (elem->size[KD_LEFT]-Xq[KD_RIGHT]);
	}
	else if( Xq[KD_TOP] < elem->size[KD_BOTTOM] )
		return (elem->size[KD_BOTTOM]-Xq[KD_TOP]);
	else if( Xq[KD_BOTTOM] > elem->size[KD_TOP] )
		return (Xq[KD_BOTTOM]-elem->size[KD_TOP]);
	else
		return 0.0;
	/*   Well, this is a cheap backup , but pretty simplistic 
	x1 = (Xq[KD_LEFT] + Xq[KD_RIGHT])/2;
	y1 = (Xq[KD_BOTTOM] + Xq[KD_TOP])/2;
	x2 = (elem->size[KD_LEFT] + elem->size[KD_RIGHT])/2;
	y2 = (elem->size[KD_BOTTOM] + elem->size[KD_TOP])/2;
	d = (x2-x1);
	d *= d;
	d2 = (y2-y1);
	d2 *= d2;
	d += d2;
	return d; */
}

double coord_dist(double x, double y)
{
	double d=x-y;
	//d=x-y;
	// d *= d;
	return (d*d);
}


void add_priority(int m, KDPriority *P, kd_box Xq, KDElem *elem)
{
	int x;
	double d;
	d = KDdist(Xq,elem);
	for(x=m-1;x>=0;x--)
	{
		if( d < P[x].dist )
		{
			if(x != m-1 )
			{
				P[x+1] = P[x];
			}
			P[x].dist = d;
			P[x].elem = elem;
		}
		else
			break;
	}
	return;
}



void kd_print_nearest(KDTree* tree, double x, double y, poly_typ_enm pl_typ, int m)
{
	KDPriority *list=NULL;
	int xz,i;
	
	xz = kd_nearest(tree, x, y, pl_typ,  m, list);
	fprintf(stdout,"Nearest Search: visited %d nodes to find the %d closest objects.\n", xz, m);
	for(i=0;i<m;i++)
	{
	  if(list[i].elem != NULL)
	      (void)fprintf(stdout,"Nearest Neighbor: dist to center: %f units. elem=%p. item=%p. x(%.14f,%.14f) y(%.14f,%.14f)\n",
				list[i].dist,
				(void*)list[i].elem,
				(void*)list[i].elem->item,
				list[i].elem->size[KD_LEFT],
				list[i].elem->size[KD_RIGHT],
				list[i].elem->size[KD_BOTTOM],
				list[i].elem->size[KD_TOP]);
	}
	nco_free(list);
}


int bounds_intersect(kd_box Xq, kd_box Bp, kd_box Bn)
{

  kd_box Xn;

  Xn[KD_LEFT]=Bn[0];
  Xn[KD_BOTTOM]=Bn[1];
    
  Xn[KD_RIGHT]=Bp[0];
  Xn[KD_TOP]=Bp[1]; 
    
    
  if(BOXINTERSECT(Xq,Xn))
       return 1;

  return 0;

     
}





int bounds_overlap_ball(kd_box Xq, kd_box Bp, kd_box Bn, int m, KDPriority *list)
{
	int idx;
	double sum=0.0;

	int dbg_flg=0;

	if(dbg_flg)
	  printf("ball: Bp(%.14f, %.14f) Bn(%.14f, %.14f) list[m-1].dist=%g ",  Bp[0], Bp[1], Bn[0], Bn[1], list[m-1].dist);


	for(idx = 0; idx < 2; idx++)
	{
		if( Xq[idx] < Bn[idx] )
		{
			sum += coord_dist(Xq[idx],Bn[idx]);
			if( sum > list[m-1].dist )
			  return 0;
		}
		else if( Xq[idx] > Bp[idx] )
		{
			sum += coord_dist(Xq[idx],Bp[idx]);
			if( sum > list[m-1].dist )
  			  return 0;
		}
	}

	if(dbg_flg)
		printf(" ret=1\n");

	return 1;
}

int  kd_neighbour(KDElem *node, kd_box Xq, int nbr_list, KDPriority *list, kd_box Bp, kd_box Bn)
{
    int d;
    short hort,vert;
    double p;
    
    KDState *realGen;
    KDSave *top_elem;
    KDElem *top_item;

    
    realGen = (KDState*)nco_malloc( sizeof(KDState));
	
    kd_data_tries = 0;
	
    realGen->stack_size = KD_INIT_STACK;
    realGen->top_index = 0;
    realGen->stk = (KDSave*)nco_malloc(sizeof(KDSave)* KD_INIT_STACK);

    /* Initialize search state */
    if (node)
	{
		kd_pushb(realGen, node, 0,Bn,Bp);
    }
	else
	{
		realGen->top_index = -1;
    }

	while (realGen->top_index > 0)
	{

		
		top_elem = &(realGen->stk[realGen->top_index-1]);
		top_item = top_elem->item;
		d = top_elem->disc;
		p = top_item->size[d];
		hort = d & 1;
		vert = d & 2;

		// printf("kd_neighbour(), state=%d p=%g disc=%d kd_data_tries=%d\n", top_elem->state,p,d,kd_data_tries );

		
		switch (top_elem->state)
		{
		case KD_THIS_ONE:
			/* Check this one */
			kd_data_tries++;
			if( top_item->item ) /* really shouldn't add dead nodes to the list! */
				add_priority(nbr_list,list,Xq,top_item);
			top_elem->state += 1;
			break;
		case KD_LOSON:
			/* calc bounds */
			/* See if we push on the loson */
			if( Xq[d] <= p )
			{
				if( top_item->sons[KD_LOSON])
				{
					if (vert)
					{
						top_elem->Bp[hort] = top_item->size[d];
						top_elem->Bn[hort] = top_item->lo_min_bound;
					}
					else
					{
						top_elem->Bp[hort] = top_item->other_bound;
						top_elem->Bn[hort] = top_item->lo_min_bound;
					}
					if( bounds_overlap_ball(Xq,top_elem->Bp,top_elem->Bn,nbr_list,list))
					{
						top_elem->state += 1;
						kd_pushb(realGen, top_item->sons[KD_LOSON], (d+1)%4,top_elem->Bn,top_elem->Bp);
					}
					else
						top_elem->state++;
				}
				else
					top_elem->state += 1;
			}
			else
			{
				if( top_item->sons[KD_HISON] )
				{
					if (vert)
					{
						top_elem->Bp[hort] = top_item->hi_max_bound;
						top_elem->Bn[hort] = top_item->other_bound;
					}
					else
					{
						top_elem->Bp[hort] = top_item->hi_max_bound;
						top_elem->Bn[hort] = top_item->size[d];
					}
					if( bounds_overlap_ball(Xq,top_elem->Bp,top_elem->Bn,nbr_list,list))
					{
						top_elem->state += 1;
						kd_pushb(realGen, top_item->sons[KD_HISON], (d+1)%4,top_elem->Bn,top_elem->Bp);
					}
					else
						top_elem->state++;
				}
				else
					top_elem->state += 1;
			}
			break;
		case KD_HISON:
			/* See if we push on the hison */
			if( Xq[d] <= p )
			{
				if( top_item->sons[KD_HISON] )
				{
					if (vert)
					{
						top_elem->Bp[hort] = top_item->hi_max_bound;
						top_elem->Bn[hort] = top_item->other_bound;
					}
					else
					{
						top_elem->Bp[hort] = top_item->hi_max_bound;
						top_elem->Bn[hort] = top_item->size[d];
					}
					if( bounds_overlap_ball(Xq,top_elem->Bp,top_elem->Bn,nbr_list,list))
					{
						top_elem->state += 1;
						kd_pushb(realGen, top_item->sons[KD_HISON], (d+1)%4,top_elem->Bn,top_elem->Bp);
					}
					else
						top_elem->state++;
				}
				else
					top_elem->state += 1;
			}
			else
			{
				if( top_item->sons[KD_LOSON] )
				{
					if (vert)
					{
						top_elem->Bp[hort] = top_item->size[d];
						top_elem->Bn[hort] = top_item->lo_min_bound;
					}
					else
					{
						top_elem->Bp[hort] = top_item->other_bound;
						top_elem->Bn[hort] = top_item->lo_min_bound;
					}
					if( bounds_overlap_ball(Xq,top_elem->Bp,top_elem->Bn,nbr_list,list))
					{
						top_elem->state += 1;
						kd_pushb(realGen, top_item->sons[KD_LOSON], (d+1)%4,top_elem->Bn,top_elem->Bp);
					}
					else
						top_elem->state++;
				}
				else
					top_elem->state += 1;
			}
			break;
		default:
			/* We have exhausted this node -- pop off the next one */
			realGen->top_index -= 1;
			break;
		}
	}
	nco_free(realGen->stk);
	nco_free(realGen);

	/* we have to change KDElem * to kd_generic and give the user something back
	 he can 'identify' with. */

	/* this is rubbish   
	for(p=0;p<m;p++)
	{
		list[p].elem = (KDElem *)list[p].elem->item;
	}
        */

	return kd_data_tries;
}




int kd_neighbour_intersect(KDElem *node, kd_box Xq, int m, KDPriority *list, kd_box Bp, kd_box Bn)
{
  int idx;
  int iret;

  if( BOXINTERSECT(Xq,node->size))
  {
    for(idx=0 ; idx<m ;idx++)
    {
      if(list[idx].elem == (KDElem*)NULL)
	{
	  list[idx].elem=node;
	  list[idx].dist=1.1;
	  break;
        }	  

    }
    if(idx==m)
      return 0;

    
  }  
  
  if( node->sons[0] )
  {  
    iret= kd_neighbour_intersect(node->sons[0], Xq,  m,  list,  Bp,  Bn);
    if(iret==0) return iret;
      
  }
     
  if( node->sons[1] )
  {  
     kd_neighbour_intersect(node->sons[1], Xq,  m,  list,  Bp,  Bn);
     if(iret==0) return iret;
       
  }
     
  return 1;

}

int kd_neighbour_intersect2(KDElem *node, int disc, kd_box Xq, int m, KDPriority *list)
{
  int idx;
  int iret;


  
  /* horizonal */
  if( disc == 0 || disc==2 )
  {
    /* out of bounds no overlap */ 
     iret=!( node->lo_min_bound > Xq[KD_RIGHT] ||  node->hi_max_bound < Xq[KD_LEFT] );

     if(iret==0) return 1;


     /* check vertical */
     //if(  node->size[KD_BOTTOM] >= Xq[KD_BOTTOM] && node->size[KD_BOTTOM] <= Xq[KD_TOP] )
     if( BOXINTERSECT(node->size, Xq) )
     { 
           for(idx=0 ; idx<m ;idx++)
             if(!list[idx].elem)
	     {
	        list[idx].elem=node;
	        list[idx].dist=1.1;
	        break;
             }	  

	   if(idx==m)
                 return 0;
     }
   
  }
  else
    /* vertical */  
  {    

    /* out of bounds - no overlap */
    iret=!( node->lo_min_bound >  Xq[KD_TOP] ||  node->hi_max_bound < Xq[KD_BOTTOM] );
     if(iret==0) return 1;


     // if( node->size[KD_LEFT] >= Xq[KD_LEFT] && node->size[KD_LEFT] <= Xq[KD_RIGHT] )
     if( BOXINTERSECT(node->size, Xq) )
       { 
           for(idx=0 ; idx<m ;idx++)
             if(!list[idx].elem)
	     {
	       list[idx].elem=node;
	       list[idx].dist=1.1;
	       break;
             }
	   
	   if(idx==m)
	     return 0;

     }



  }


  if( node->sons[0] )
  {
    iret= kd_neighbour_intersect2(node->sons[0], (disc+1)%4, Xq,  m,  list);
    if(iret==0) return iret;
      
  }
     
  if( node->sons[1] )
  {  
    kd_neighbour_intersect2(node->sons[1], (disc+1)%4,  Xq,  m,  list);
    if(iret==0) return iret;
       
  }


  return 1;
  
}

//int kd_neighbour_intersect3(KDElem *node, int disc, kd_box Xq, KDPriority **list_head , KDPriority *list_end, int stateH, int stateV )
int kd_neighbour_intersect3(KDElem *node, int disc, kd_box Xq, omp_mem_sct *omp_mem, int stateH, int stateV )
{
  
  int iret;

  nco_bool bAddPnt=False;




  char fnc_nm[]="kd_neighbour_intersect3";



  /* horizonal */
  if( stateH <2  &&   (disc == 0 || disc==2 ) )
  {

    if( node->lo_min_bound > Xq[KD_RIGHT] ||  node->hi_max_bound < Xq[KD_LEFT] )
	 return 1;

    if( node->lo_min_bound >= Xq[KD_LEFT] &&  node->hi_max_bound <= Xq[KD_RIGHT] )
	 stateH=2;
    else
      stateH=1;
    
   
  }
    /* vertical */  
  else if(stateV <2 && (disc == 1 || disc ==3) )
  {    

    /* out of bounds - no overlap */
    if( node->lo_min_bound >  Xq[KD_TOP] ||  node->hi_max_bound < Xq[KD_BOTTOM] )
	 return 1;

    if( node->lo_min_bound >=  Xq[KD_BOTTOM] &&   node->hi_max_bound <= Xq[KD_TOP] )
	 stateV=2;
	else
     stateV=1;
    


  }

  if(stateH == 2 && stateV == 2) bAddPnt=True;
  else if(stateH == 2 && stateV == 1 && node->size[KD_TOP] >= Xq[KD_BOTTOM] && node->size[KD_BOTTOM] <= Xq[KD_TOP]) bAddPnt=True;
  else if(stateH == 1 && stateV == 2 && node->size[KD_RIGHT] >= Xq[KD_LEFT] && node->size[KD_LEFT] <= Xq[KD_RIGHT]) bAddPnt=True;
  else if(BOXINTERSECT(node->size,Xq)) bAddPnt=True;

  /* add node as necessary */
  if(bAddPnt){ 
    if(omp_mem->kd_blk_nbr*NCO_VRL_BLOCKSIZE < omp_mem->kd_cnt+1){
      /* fxm */
      (void)fprintf(stderr,"%s: ERROR %s reports that the kd-tree overlap buffer (size=%d) is now full and currenly cannot be dynamically expanded. We are trying to fix this. In the meanwhile consider increasing NCO_VRL_BLOCKSIZE in nco_kd.h, and then re-compiling.\n",nco_prg_nm_get(),fnc_nm,NCO_VRL_BLOCKSIZE);
      nco_exit(EXIT_FAILURE);

      /*
        omp_mem->kd_list=(KDPriority*)nco_realloc(omp_mem->kd_list,++omp_mem->kd_blk_nbr*(NCO_VRL_BLOCKSIZE*sizeof(KDPriority)));
        (void)fprintf(stderr,"%s: Increasing block size to %ld kd_cnt=%ld omp_mem=%p\n",fnc_nm,omp_mem->kd_blk_nbr,omp_mem->kd_cnt,(void*)omp_mem->kd_list);
      */
    } /* !omp_mem */
    omp_mem->kd_list[omp_mem->kd_cnt].elem = node;
    omp_mem->kd_list[omp_mem->kd_cnt].dist = 1.0;
    omp_mem->kd_list[omp_mem->kd_cnt++].area = -1.0;
  } /* !baddPnt */

  if(node->sons[0]){
    //iret= kd_neighbour_intersect3(node->sons[0], (disc+1)%4, Xq, list_head,  list_end, stateH, stateV);
    iret= kd_neighbour_intersect3(node->sons[0], (disc+1)%4, Xq, omp_mem, stateH, stateV);
    if(iret==0) return iret;
  }
     
  if(node->sons[1]){  
    //iret=kd_neighbour_intersect3(node->sons[1], (disc+1)%4,  Xq, list_head,  list_end, stateH, stateV);
    iret=kd_neighbour_intersect3(node->sons[1], (disc+1)%4,  Xq, omp_mem, stateH, stateV);
    if(iret==0) return iret;
  }

  return 1;
}


/* sort by distance */
int kd_priority_cmp_dist( const void *vp1, const void *vp2)
{

  const KDPriority * const kd1=((const KDPriority * const )vp1);
  const KDPriority * const kd2=((const KDPriority * const )vp2);

  //ptrdiff_t df= (char*)kd1->elem->item - (char*)kd2->elem->item;
  double  df= kd1->dist - kd2->dist;

  return ( df < 0.0 ? -1 :  df >0.0 ? 1 : 0   );
}


int kd_priority_cmp( const void *vp1, const void *vp2)
{

  const KDPriority * const kd1=((const KDPriority * const )vp1);
  const KDPriority * const kd2=((const KDPriority * const )vp2);

  //ptrdiff_t df= (char*)kd1->elem->item - (char*)kd2->elem->item;
  ptrdiff_t df= kd1->elem->item - kd2->elem->item;

  return ( df < 0 ? -1 :  df >0 ? 1 : 0   );
}

/* Sorts input list
 * if duplicates then copy new list over to old and True returned
 * no duplicates then sorted list remains and False is returned
 */
nco_bool kd_priority_list_sort(KDPriority *list, int nbr_lst, int fll_nbr, int *out_fll_nbr) {


  int idx;
  int nw_fll_nbr;

  nco_bool bret=False;

  KDPriority *lcl_list=NULL_CEWI;

  lcl_list=(KDPriority*)nco_calloc(nbr_lst, sizeof(KDPriority));


  /* use stdlib sort  */
  qsort((void *)list, fll_nbr, sizeof(KDPriority), kd_priority_cmp);

  lcl_list[0]=list[0];
  nw_fll_nbr=1;

  for (idx = 1; idx < fll_nbr; idx++)
    if(list[idx].elem->item != list[idx - 1].elem->item )
        lcl_list[nw_fll_nbr++] = list[idx];




  /* deal with duplicates */
  if(  nw_fll_nbr < fll_nbr  ){
     /* copy new local list over to old one */
     memcpy(list, lcl_list, sizeof(KDPriority)*nbr_lst );
     bret=True;

  } else
     bret=False;

  *out_fll_nbr=nw_fll_nbr;

  lcl_list=(KDPriority*)nco_free(lcl_list);


  return bret;

}



int kd_nearest (KDTree* realTree, double x, double y, poly_typ_enm pl_typ, int m, KDPriority *alist)
{
	int idx;
	kd_box Bp,Bn,Xq;

	Xq[KD_LEFT] = x;
	Xq[KD_BOTTOM] = y;
	Xq[KD_RIGHT] = x;
	Xq[KD_TOP] = y;

	/* bit of  hack - set the static here -
	 * nb is picked up in kDist() */
	kd_pl_typ=pl_typ;

	//*alist = (KDPriority *)nco_calloc(sizeof(KDPriority),m);
	for(idx=0;idx<m;idx++)
	{
	  alist[idx].dist = KD_DBL_MAX;
	  //alist[idx].elem = (KDElem*)NULL;
	  //alist[idx].area = 0.0;
	}


	Bp[0]=realTree->extent[0];
	Bp[1]=realTree->extent[1];

	Bn[0]=realTree->extent[2];
	Bn[1]=realTree->extent[3];


	return kd_neighbour(realTree->tree,Xq,m,alist,Bp,Bn);
}

int kd_nearest_intersect_wrp(KDTree **rTree, int nbr_tr, kd_box Xq, kd_box Xr, omp_mem_sct *omp_mem)
{
  /* count duplicates for dbg */
  int ret_cnt_nw=0;



  nco_bool bSort=False;

  //  const char fnc_nm[]="kd_nearest_intersect_wrp():";
  // (void)fprintf(stderr,"%s:%s: just entered function\n", nco_prg_nm_get(),fnc_nm );


  (void)kd_nearest_intersect(rTree, nbr_tr,Xq, omp_mem, bSort);

  (void)kd_nearest_intersect(rTree,nbr_tr, Xr, omp_mem, bSort);


  if( omp_mem->kd_cnt >1  &&  kd_priority_list_sort(omp_mem->kd_list,  omp_mem->kd_blk_nbr* NCO_VRL_BLOCKSIZE, omp_mem->kd_cnt , &ret_cnt_nw  )  )
    omp_mem->kd_cnt=ret_cnt_nw;

  return omp_mem->kd_cnt;
}

//int kd_nearest_intersect(KDTree* realTree, kd_box Xq, int m, KDPriority *list, int bSort)
int kd_nearest_intersect(KDTree** rTree, int nbr_tr, kd_box Xq, omp_mem_sct *omp_mem, int bSort)
{
  int idx;
  int nw_lcl_cnt=0;
  int ret_cnt=0;
  
  //const char fnc_nm[]="kd_nearest_intersect()";

   //(void)fprintf(stderr,"%s:%s: just entered function\n", nco_prg_nm_get(),fnc_nm );
    /*
	list_srt= omp_mem->kd_list+ (size_t)omp_mem->kd_cnt;
	list_end= omp_mem->kd_list;
	m=omp_mem->kd_blk_nbr * NCO_VRL_BLOCKSIZE;
	list_end+=m;
   */
   //node_cnt= kd_neighbour_intersect3(realTree->tree,0,Xq, &list_srt ,list_end,0,0);

   for(idx=0;idx < nbr_tr;idx++)
     (void)kd_neighbour_intersect3(rTree[idx]->tree,0,Xq, omp_mem,0,0);

    /*
    ret_cnt=( list_srt - omp_mem->kd_list)+ omp_mem->kd_cnt;
    if(ret_cnt > 1 && bSort && kd_priority_list_sort(omp_mem->kd_list, m, ret_cnt, &nw_lcl_cnt)) ret_cnt=nw_lcl_cnt;
    */
    ret_cnt=omp_mem->kd_cnt;
    if(omp_mem->kd_cnt &&
       bSort &&
       kd_priority_list_sort(omp_mem->kd_list,omp_mem->kd_blk_nbr*NCO_VRL_BLOCKSIZE,omp_mem->kd_cnt,&nw_lcl_cnt)
       ){
      ret_cnt=nw_lcl_cnt;
      omp_mem->kd_cnt=nw_lcl_cnt;
    }

    if(0 && nco_dbg_lvl_get() >= nco_dbg_dev)
    {
      KDPriority *list;

      list=omp_mem->kd_list;
      (void)fprintf(stderr,"ret_cnt=%d\n", ret_cnt);

      for(idx=0;idx<ret_cnt;idx++)
      {
        (void)fprintf(stderr," dist to center: %f units. elem=%p item=%p. x(%.14f,%.14f) y(%.14f,%.14f)\n",
                      list[idx].dist,
                      (void*)list[idx].elem,
                      list[idx].elem->item,
                      list[idx].elem->size[KD_LEFT],
                      list[idx].elem->size[KD_RIGHT],
                      list[idx].elem->size[KD_BOTTOM],
                      list[idx].elem->size[KD_TOP]);
      }
    }
  return ret_cnt;
}
