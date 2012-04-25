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

#include "gt-generic-content.h"
#include "gt-content.h"

struct _GtGenericContentPrivate {
  GHashTable *metadata;
};

#define GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GT_TYPE_GENERIC_CONTENT, GtGenericContentPrivate))

static void gt_content_iface_init (GtContentIface *iface);

G_DEFINE_TYPE_WITH_CODE (GtGenericContent, gt_generic_content,
                         G_TYPE_INITIALLY_UNOWNED,
                         G_IMPLEMENT_INTERFACE (GT_TYPE_CONTENT,
                                                gt_content_iface_init));

/*
 * GtContent implementation
 */

static const char *
content_get_metadata (GtContent         *content,
                      GtContentMetadata key)
{
  GtGenericContent *gc = (GtGenericContent *) content;
  GtGenericContentPrivate *priv = gc->priv;

  return (const gchar *) g_hash_table_lookup (priv->metadata,
                                              GUINT_TO_POINTER (key));
}

static void
content_set_metadata (GtContent         *content,
                      GtContentMetadata key,
                      const gchar       *value)
{
  const char *property;
  GtGenericContent *gc = (GtGenericContent *) content;
  GtGenericContentPrivate *priv = gc->priv;

  if (value)
    g_hash_table_insert (priv->metadata, GUINT_TO_POINTER (key),
                         g_strdup (value));
  else
    g_hash_table_remove (priv->metadata, GUINT_TO_POINTER (key));
}

struct MetadataCbStuff {
  GtContentMetadataCb callback;
  gpointer            data;
};

static void
content_foreach_metadata_cb (gpointer key,
                             gpointer value,
                             gpointer data)
{
  struct MetadataCbStuff *stuff = (struct MetadataCbStuff *) data;

  stuff->callback ((GtContentMetadata) key,
                   (const gchar *) value,
                   stuff->data);
}

static void
content_foreach_metadata (GtContent           *content,
                          GtContentMetadataCb  callback,
                          gpointer              data)
{
  GtGenericContent *gc = (GtGenericContent *) content;
  GtGenericContentPrivate *priv = gc->priv;
  struct MetadataCbStuff stuff = {
    .callback = callback,
    .data = data,
  };

  g_hash_table_foreach (priv->metadata,
                        content_foreach_metadata_cb,
                        &stuff);
}

static void
gt_content_iface_init (GtContentIface *iface)
{
  iface->get_metadata     = content_get_metadata;
  iface->set_metadata     = content_set_metadata;
  iface->foreach_metadata = content_foreach_metadata;
}

/*
 * GObject implementation
 */

static void
gt_generic_content_dispose (GObject *object)
{
  GtGenericContent *content = (GtGenericContent *) object;
  GtGenericContentPrivate *priv = content->priv;

  if (priv->metadata) {
    g_hash_table_unref (priv->metadata);
    priv->metadata = NULL;
  }

  G_OBJECT_CLASS (gt_generic_content_parent_class)->dispose (object);
}

static void
gt_generic_content_class_init (GtGenericContentClass *klass)
{
  GObjectClass *o_class = (GObjectClass *) klass;

  o_class->dispose = gt_generic_content_dispose;
  g_type_class_add_private (klass, sizeof (GtGenericContentPrivate));
}

static void
gt_generic_content_init (GtGenericContent *self)
{
  GtGenericContentPrivate *priv;

  self->priv = priv = GET_PRIVATE (self);
  priv->metadata = g_hash_table_new_full (NULL, NULL, NULL, g_free);
}
