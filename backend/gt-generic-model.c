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

#include <glib.h>

#include <gt/gt-model.h>
#include <gt/gt-generic-model.h>
#include <gt/gt-model-manager.h>

enum {
  PROP_TITLE = 1,
  PROP_LENGTH,
};

struct _GtGenericModelPrivate {
  GArray *items;
  gchar *title;
};

#define GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GT_TYPE_GENERIC_MODEL, GtGenericModelPrivate))

static void gt_model_iface_init (GtModelIface *iface);

G_DEFINE_TYPE_WITH_CODE (GtGenericModel, gt_generic_model, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GT_TYPE_MODEL, gt_model_iface_init));

static GtContent *
get_content_internal (GtGenericModel *model,
                      guint          idx)
{
  GtGenericModelPrivate *priv = model->priv;

  if (idx >= priv->items->len) {
    return NULL;
  } else {
    return g_array_index (priv->items, GtContent *, idx);
  }
}

/*
 * GtModel implementation
 */
static void
gt_generic_model_add_content (GtModel   *model,
                              GtContent *content)
{
  GtGenericModel *gm = (GtGenericModel *) model;
  GtGenericModelPrivate *priv = gm->priv;
  gint pos;

  g_object_ref_sink (content);
  pos = priv->items->len;
  g_array_append_val (priv->items, content);

  g_object_notify (G_OBJECT (model), "length");
}

static void
gt_generic_model_add (GtModel *model,
                      GList *content_list)
{
  GtGenericModel *gm = (GtGenericModel *) model;
  GtGenericModelPrivate *priv = gm->priv;
  GtContent *content;
  gint pos;
  GList *list = g_list_copy (content_list);

  while (list) {
    content = list->data;
    g_object_ref_sink (content);

    pos = priv->items->len;
    g_array_append_val (priv->items, content);

    list = g_list_delete_link (list, list);
  }

  g_object_notify (G_OBJECT (model), "length");
  g_list_free (list);
}


static int
array_find (GArray   *array,
            gpointer  data)
{
  int i;

  for (i = 0; i < array->len; i++) {
    gpointer child = g_array_index (array, gpointer, i);
    if (child == data) {
      return i;
    }
  }

  return -1;
}

static void
gt_generic_model_remove_content (GtModel   *model,
                                 GtContent *content)
{
  GtGenericModel *gm = (GtGenericModel *) model;
  GtGenericModelPrivate *priv = gm->priv;
  int idx;

  idx = array_find (priv->items, content);
  if (idx == -1) {
    return;
  }

  g_array_remove_index (priv->items, idx);
  g_object_unref (content);

  g_object_notify (G_OBJECT (model), "length");
}

static void
gt_generic_model_clear (GtModel *model)
{
  GtGenericModel *gm = (GtGenericModel *) model;
  GtGenericModelPrivate *priv = gm->priv;
  gint i;

  for (i = 0; i < priv->items->len; i++) {
    GtContent *child = g_array_index (priv->items, GtContent *, i);
    g_object_unref (child);
  }
  g_array_set_size (priv->items, 0);
}

static GtContent *
gt_generic_model_get_content (GtModel *model,
                              guint     index_)
{
  GtGenericModel *gm = (GtGenericModel *) model;

  return get_content_internal (gm, index_);
}

static guint
gt_generic_model_get_length (GtModel *model)
{
  GtGenericModel *gm = (GtGenericModel *) model;

  GtGenericModelPrivate *priv = gm->priv;

  return priv->items->len;
}

static gint
gt_generic_model_index (GtModel   *model,
                         GtContent *content)
{
  return array_find (GT_GENERIC_MODEL (model)->priv->items, content);
}

static void
gt_model_iface_init (GtModelIface *iface)
{
  iface->get_content = gt_generic_model_get_content;
  iface->add = gt_generic_model_add;
  iface->add_content = gt_generic_model_add_content;
  iface->remove_content = gt_generic_model_remove_content;
  iface->clear = gt_generic_model_clear;
  iface->get_length = gt_generic_model_get_length;
  iface->index = gt_generic_model_index;
}

static void
gt_generic_model_finalize (GObject *object)
{
  GtGenericModel *self = (GtGenericModel *) object;
  GtGenericModelPrivate *priv = self->priv;

  g_free (priv->title);
  priv->title = NULL;

  G_OBJECT_CLASS (gt_generic_model_parent_class)->finalize (object);
}

static void
gt_generic_model_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GtGenericModel *self = (GtGenericModel *) object;
  GtGenericModelPrivate *priv = self->priv;

  switch (prop_id) {
  case PROP_TITLE:
    g_free (priv->title);
    priv->title = g_value_dup_string (value);
    break;

  default:
    break;
  }
}

static void
gt_generic_model_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GtGenericModel *self = (GtGenericModel *) object;
  GtGenericModelPrivate *priv = self->priv;

  switch (prop_id) {
  case PROP_TITLE:
    g_value_set_string (value, priv->title);
    break;

  case PROP_LENGTH:
    g_value_set_int (value, priv->items->len);
    break;

  default:
    break;
  }
}

static void
gt_generic_model_class_init (GtGenericModelClass *klass)
{
  GObjectClass *o_class = (GObjectClass *) klass;
  GParamSpec *pspec;

  o_class->finalize = gt_generic_model_finalize;
  o_class->set_property = gt_generic_model_set_property;
  o_class->get_property = gt_generic_model_get_property;

  /* GtModel properties */
  g_object_class_override_property (o_class, PROP_TITLE, "title");
   g_object_class_override_property (o_class, PROP_LENGTH, "length");

   g_type_class_add_private (klass, sizeof (GtGenericModelPrivate));
}

static void
gt_generic_model_init (GtGenericModel *self)
{
  GtGenericModelPrivate *priv = GET_PRIVATE (self);

  self->priv = priv;

  priv->title = NULL;
}

/**
 * gt_generic_model_new:
 * @title: String containing the title.
 * @icon_name: String containing the icon name.
 *
 * Creates an empty #GtGenericModel.
 *
 * Return value: (transfer full): A #GtGenericModel
 */
GtModel *
gt_generic_model_new (const gchar *title,
                       const gchar *icon_name)
{
  return g_object_new (GT_TYPE_GENERIC_MODEL,
                       "title", title,
                       "icon-name", icon_name,
                       NULL);
}

const gchar *
gt_generic_model_get_title (GtGenericModel *model)
{
  g_return_val_if_fail (GT_IS_GENERIC_MODEL (model), NULL);

  return model->priv->title;
}
