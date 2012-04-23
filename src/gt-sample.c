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

#include "gt-sample.h"

G_DEFINE_TYPE (GtSample, gt_sample, G_TYPE_OBJECT)

enum {
        PROP_0,
        PROP_NAME,
};

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GT_TYPE_SAMPLE, GtSamplePrivate))

struct _GtSamplePrivate {
        gchar *name;
};

static void
gt_sample_get_property (GObject *object, guint property_id,
                         GValue *value, GParamSpec *pspec)
{
        GtSample *self = GT_SAMPLE (object);

        switch (property_id) {
        case PROP_NAME:
                g_value_set_string (value, self->priv->name);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        }
}

static void
gt_sample_set_property (GObject *object, guint property_id,
                         const GValue *value, GParamSpec *pspec)
{
        GtSample *self = GT_SAMPLE (object);

        switch (property_id) {
        case PROP_NAME:
                gt_sample_set_name (self, g_value_get_string (value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        }
}

static void
gt_sample_finalize (GObject *object)
{
        GtSample *self = GT_SAMPLE (object);
        g_free (self->priv->name);

        G_OBJECT_CLASS (gt_sample_parent_class)->finalize (object);
}

static void
gt_sample_class_init (GtSampleClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (klass, sizeof (GtSamplePrivate));

        object_class->get_property = gt_sample_get_property;
        object_class->set_property = gt_sample_set_property;
        object_class->finalize = gt_sample_finalize;

        g_object_class_install_property
                (object_class, PROP_NAME,
                 g_param_spec_string ("name", "Your name", "The greeter",
                                      NULL,
                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
gt_sample_init (GtSample *self)
{
        self->priv = GET_PRIVATE (self);
        self->priv->name = NULL;
}

GtSample*
gt_sample_new (void)
{
        return g_object_new (GT_TYPE_SAMPLE, NULL);
}

void
gt_sample_set_name (GtSample *self, const gchar *name)
{
        if (!self->priv->name)
                g_free (self->priv->name);

        self->priv->name = g_strdup (name);
}

void
gt_sample_say_hello (GtSample *self)
{
        const gchar *name = self->priv->name ? self->priv->name : "mundo";
        g_print ("Hola %s\n", name);
}
