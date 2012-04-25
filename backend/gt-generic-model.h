/*
 * GTrakt - simple client for trakt.tv
 *
 * Based on:
 * Mex - a media explorer
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

#ifndef __GT_GENERIC_MODEL_H__
#define __GT_GENERIC_MODEL_H__

#include <glib-object.h>

#include <gt/gt-content.h>
#include <gt/gt-model.h>

G_BEGIN_DECLS

#define GT_TYPE_GENERIC_MODEL                   \
  (gt_generic_model_get_type())
#define GT_GENERIC_MODEL(obj)                           \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GT_TYPE_GENERIC_MODEL, GtGenericModel))
#define GT_GENERIC_MODEL_CLASS(klass)                   \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GT_TYPE_GENERIC_MODEL, GtGenericModelClass))
#define GT_IS_GENERIC_MODEL(obj)                       \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GT_TYPE_GENERIC_MODEL))
#define GT_IS_GENERIC_MODEL_CLASS(klass)               \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GT_TYPE_GENERIC_MODEL))
#define GT_GENERIC_MODEL_GET_CLASS(obj)                \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GT_TYPE_GENERIC_MODEL, GtGenericModelClass))

typedef struct _GtGenericModelPrivate GtGenericModelPrivate;
typedef struct _GtGenericModel       GtGenericModel;
typedef struct _GtGenericModelClass  GtGenericModelClass;

struct _GtGenericModel
{
  GObject parent;

  GtGenericModelPrivate *priv;
};

struct _GtGenericModelClass
{
  GObjectClass parent_class;
};

GType gt_generic_model_get_type (void) G_GNUC_CONST;

GtModel *gt_generic_model_new (const gchar *title);

const gchar *gt_generic_model_get_title (GtGenericModel *model);

G_END_DECLS

#endif /* __GT_GENERIC_MODEL_H__ */
