/*
 * Copyright (c) 2013 embedded brains GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _LINUX_RBTREE_H
#define _LINUX_RBTREE_H

#include <rtems/score/rbtree.h>

struct rb_node {
  struct rb_node *rb_left;
  struct rb_node *rb_right;
  struct rb_node *rb_parent;
  int rb_color;
};

RTEMS_STATIC_ASSERT(
  sizeof( struct rb_node ) == sizeof( RBTree_Node ),
  rb_node_size
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_node, rb_left ) == offsetof( RBTree_Node, Node.rbe_left ),
  rb_node_left
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_node, rb_right ) == offsetof( RBTree_Node, Node.rbe_right ),
  rb_node_right
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_node, rb_parent ) == offsetof( RBTree_Node, Node.rbe_parent ),
  rb_node_parent
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_node, rb_color ) == offsetof( RBTree_Node, Node.rbe_color ),
  rb_node_color
);

struct rb_root {
  struct rb_node *rb_node;
};

RTEMS_STATIC_ASSERT(
  sizeof( struct rb_root ) == sizeof( RBTree_Control ),
  rb_root_size
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_root, rb_node ) == offsetof( RBTree_Control, rbh_root ),
  rb_root_node
);

#undef RB_ROOT
#define RB_ROOT ( (struct rb_root) { NULL } )

#define rb_entry( p, container, field ) RTEMS_CONTAINER_OF( p, container, field )

static inline void rb_insert_color( struct rb_node *node, struct rb_root *root)
{
  _RBTree_Insert_color( (RBTree_Control *) root, (RBTree_Node *) node );
}

static inline void rb_erase( struct rb_node *node, struct rb_root *root )
{
  _RBTree_Extract( (RBTree_Control *) root, (RBTree_Node *) node );
}

static inline struct rb_node *rb_next( struct rb_node *node )
{
  return (struct rb_node *) _RBTree_Successor( (RBTree_Node *) node );
}

static inline struct rb_node *rb_prev( struct rb_node *node )
{
  return (struct rb_node *) _RBTree_Predecessor( (RBTree_Node *) node );
}

static inline struct rb_node *rb_first( struct rb_root *root )
{
  return (struct rb_node *) _RBTree_Minimum( (RBTree_Control *) root );
}

static inline struct rb_node *rb_last( struct rb_root *root )
{
  return (struct rb_node *) _RBTree_Maximum( (RBTree_Control *) root );
}

static inline void rb_replace_node(
  struct rb_node *victim,
  struct rb_node *replacement, 
  struct rb_root *root
)
{
  _RBTree_Replace_node(
    (RBTree_Control *) root,
    (RBTree_Node *) victim,
    (RBTree_Node *) replacement
  );
}

static inline void rb_link_node(
  struct rb_node *node,
  struct rb_node *parent,
  struct rb_node **link
)
{
  _RBTree_Initialize_node( (RBTree_Node *) node );
  _RBTree_Add_child(
    (RBTree_Node *) node,
    (RBTree_Node *) parent,
    (RBTree_Node **) link
  );
}

static inline struct rb_node *rb_parent( struct rb_node *node )
{
  return (struct rb_node *) _RBTree_Parent( (RBTree_Node *) node );
}

#endif /* _LINUX_RBTREE_H */
