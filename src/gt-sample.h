/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Contact: mfs-dm-2011@igalia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef _GT_SAMPLE
#define _GT_SAMPLE

#include <glib-object.h>

G_BEGIN_DECLS

#define GT_TYPE_SAMPLE gt_sample_get_type()
#define GT_SAMPLE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GT_TYPE_SAMPLE, GtSample))
#define GT_SAMPLE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GT_TYPE_SAMPLE, GtSampleClass))
#define GT_IS_SAMPLE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GT_TYPE_SAMPLE))
#define GT_IS_SAMPLE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GT_TYPE_SAMPLE))
#define GT_SAMPLE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GT_TYPE_SAMPLE, GtSampleClass))

typedef struct _GtSamplePrivate GtSamplePrivate;

typedef struct {
        GObject parent;

        /* <private> */
        GtSamplePrivate *priv;
} GtSample;

typedef struct {
        GObjectClass parent_class;
} GtSampleClass;

GType gt_sample_get_type (void);
GtSample* gt_sample_new (void);

void gt_sample_set_name (GtSample *self, const gchar *name);
void gt_sample_say_hello (GtSample *self);

G_END_DECLS

#endif /* _GT_SAMPLE */
