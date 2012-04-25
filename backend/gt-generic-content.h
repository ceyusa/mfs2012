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

#ifndef __GT_GENERIC_CONTENT_H__
#define __GT_GENERIC_CONTENT_H__

#include <glib-object.h>

#include <gt/gt-content.h>

G_BEGIN_DECLS

#define GT_TYPE_GENERIC_CONTENT                 \
  (gt_generic_content_get_type())
#define GT_GENERIC_CONTENT(obj)                                         \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GT_TYPE_GENERIC_CONTENT, GtGenericContent))
#define GT_GENERIC_CONTENT_CLASS(klass)                                 \
  (G_TYPE_CHECK_CLASS_CAST ((klass),  GT_TYPE_GENERIC_CONTENT,  GtGenericContentClass))
#define GT_IS_GENERIC_CONTENT(obj)                              \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GT_TYPE_GENERIC_CONTENT))
#define GT_IS_GENERIC_CONTENT_CLASS(klass)                      \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GT_TYPE_GENERIC_CONTENT))
#define GT_GENERIC_CONTENT_GET_CLASS(obj)                               \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GT_TYPE_GENERIC_CONTENT, GtGenericContentClass))

typedef struct _GtGenericContentPrivate GtGenericContentPrivate;
typedef struct _GtGenericContent        GtGenericContent;
typedef struct _GtGenericContentClass   GtGenericContentClass;

struct _GtGenericContent
{
  GInitiallyUnowned parent;

  GtGenericContentPrivate *priv;
};

struct _GtGenericContentClass
{
  GInitiallyUnownedClass parent_class;
};

GType gt_generic_content_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* __GT_GENERIC_CONTENT_H__ */
