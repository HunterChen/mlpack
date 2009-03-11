// Copyright 2007 Georgia Institute of Technology. All rights reserved.
// ABSOLUTELY NOT FOR DISTRIBUTION
/**
 * @file tree/kdtree.h
 *
 * Tools for kd-trees.
 *
 * Eventually we hope to support KD trees with non-L2 (Euclidean)
 * metrics, like Manhattan distance.
 *
 * @experimental
 */

#ifndef TREE_GEN_METRIC_TREE_H
#define TREE_GEN_METRIC_TREE_H

#include "general_spacetree.h"

#include "fastlib/base/common.h"
#include "fastlib/col/arraylist.h"
#include "fastlib/fx/fx.h"

#include "gen_metric_tree_impl.h"

/**
 * Regular pointer-style trees (as opposed to THOR trees).
 */
namespace proximity {

  /**
   * Creates a ball tree from data.
   *
   * @experimental
   *
   * This requires you to pass in two unitialized ArrayLists which will contain
   * index mappings so you can account for the re-ordering of the matrix.
   * (By unitialized I mean don't call Init on it)
   *
   * @param matrix data where each column is a point, WHICH WILL BE RE-ORDERED
   * @param leaf_size the maximum points in a leaf
   * @param old_from_new pointer to an unitialized arraylist; it will map
   *        new indices to original
   * @param new_from_old pointer to an unitialized arraylist; it will map
   *        original indexes to new indices
   */
  template<typename TMetricTree>
  TMetricTree *MakeGenMetricTree(Matrix& matrix, index_t leaf_size,
				 ArrayList<index_t> *old_from_new = NULL,
				 ArrayList<index_t> *new_from_old = NULL) {

    TMetricTree *node = new TMetricTree();
    index_t *old_from_new_ptr;

    if (old_from_new) {
      old_from_new->Init(matrix.n_cols());
      
      for (index_t i = 0; i < matrix.n_cols(); i++) {
        (*old_from_new)[i] = i;
      }
      
      old_from_new_ptr = old_from_new->begin();
    } else {
      old_from_new_ptr = NULL;
    }

    node->Init(0, matrix.n_cols());
    node->bound().center().Init(matrix.n_rows());
    tree_gen_metric_tree_private::SplitGenMetricTree<TMetricTree>
      (matrix, node, leaf_size, old_from_new_ptr);
    
    if (new_from_old) {
      new_from_old->Init(matrix.n_cols());
      for (index_t i = 0; i < matrix.n_cols(); i++) {
        (*new_from_old)[(*old_from_new)[i]] = i;
      }
    }
    
    return node;
  }

};

#endif
