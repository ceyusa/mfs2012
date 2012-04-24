/*
 * GTrakt - simple client for trakt.tv
 *
 * Based on:
 * Gt - a media explorer
 *
 * Copyright © 2010, 2011 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>
 */


#ifndef __GT_MODEL_H__
#define __GT_MODEL_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GT_TYPE_MODEL            \
  (gt_model_get_type ())
#define GT_MODEL(obj)            \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GT_TYPE_MODEL, GtModel))
#define GT_IS_MODEL(obj)         \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GT_TYPE_MODEL))
#define GT_MODEL_IFACE(iface)    \
  (G_TYPE_CHECK_CLASS_CAST ((iface), GT_TYPE_MODEL, GtModelIface))
#define GT_IS_MODEL_IFACE(iface) \
  (G_TYPE_CHECK_CLASS_TYPE ((iface), GT_TYPE_MODEL))
#define GT_MODEL_GET_IFACE(obj)  \
  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GT_TYPE_MODEL, GtModelIface))

typedef struct _GtModel      GtModel;
typedef struct _GtModelIface GtModelIface;

#include "gt-content.h"

struct _GtModelIface
{
  GTypeInterface g_iface;

  GtContent *(*get_content) (GtModel *model,
                             guint    index_);
  void (*add_content) (GtModel   *model,
                       GtContent *content);
  void (*add) (GtModel *model,
               GList    *content_list);
  void (*remove_content) (GtModel   *model,
                          GtContent *content);
  void (*clear) (GtModel *model);
  guint (*get_length) (GtModel *model);
};

GType gt_model_get_type (void) G_GNUC_CONST;

GtContent *gt_model_get_content (GtModel *model,
                                 guint   index_);
void gt_model_add (GtModel *model,
                   GList   *content);
void gt_model_add_content (GtModel   *model,
                           GtContent *content);
void gt_model_remove_content (GtModel   *model,
                              GtContent *content);
void gt_model_clear (GtModel *model);
guint gt_model_get_length (GtModel *model);

gchar *gt_model_to_string (GtModel *model);

G_END_DECLS

#endif /* __GT_MODEL_H__ */
