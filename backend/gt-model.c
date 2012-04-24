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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gt-model.h"

static void
gt_model_base_finalize (gpointer g_iface)
{
}

static void
gt_model_base_init (gpointer g_iface)
{
  static gboolean initialised = FALSE;

  if (!initialised) {
    GParamSpec *pspec;

    pspec = g_param_spec_string ("title", "Title", "The title of the feed", "",
                                 G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE);
    g_object_interface_install_property (g_iface, pspec);

    pspec = g_param_spec_int ("length",
                              "length",
                              "The number of items in the feed",
                              0, G_MAXINT, 0,
                              G_PARAM_STATIC_STRINGS | G_PARAM_READABLE);
    g_object_interface_install_property (g_iface, pspec);

    initialised = TRUE;
  }
}

GType
gt_model_get_type (void)
{
  static GType our_type = 0;

  if (G_UNLIKELY (our_type == 0)) {
    GTypeInfo model_info = {
      sizeof (GtModelIface),
      gt_model_base_init,
      gt_model_base_finalize
    };

    our_type = g_type_register_static (G_TYPE_INTERFACE,
                                       "GtModel",
                                       &model_info, 0);
  }

  return our_type;
}

/**
 * gt_model_get_content:
 * @model: a #GtModel
 * @index_: a position
 *
 * Retrieves the #GtContent object at position @index_ for this @model.
 *
 * Return value: (transfer full): A #GtContent. Call
 * g_object_unref() on the controller once finished with it.
 *
 * Since: 0.2
 */
GtContent *
gt_model_get_content (GtModel *model,
                      guint    index_)
{
  GtModelIface *iface;

  g_return_val_if_fail (GT_IS_MODEL (model), NULL);

  iface = GT_MODEL_GET_IFACE (model);

  if (G_LIKELY (iface->get_content))
    return iface->get_content (model, index_);

  g_warning ("GtModel of type '%s' does not implement get_content()",
             g_type_name (G_OBJECT_TYPE (model)));

  return NULL;
}

void
gt_model_add (GtModel *model,
              GList   *content_list)
{
  GtModelIface *iface;

  g_return_if_fail (GT_IS_MODEL (model));

  iface = GT_MODEL_GET_IFACE (model);

  if (G_LIKELY (iface->add)) {
    iface->add (model, content_list);
    return;
  }

  g_warning ("GtModel of type '%s' does not implement add ()",
             g_type_name (G_OBJECT_TYPE (model)));
}

void
gt_model_add_content (GtModel   *model,
                      GtContent *content)
{
  GtModelIface *iface;

  g_return_if_fail (GT_IS_MODEL (model));

  iface = GT_MODEL_GET_IFACE (model);

  if (G_LIKELY (iface->add_content)) {
    iface->add_content (model, content);
    return;
  }

  g_warning ("GtModel of type '%s' does not implement add_content ()",
             g_type_name (G_OBJECT_TYPE (model)));
}

void
gt_model_remove_content (GtModel   *model,
                         GtContent *content)
{
  GtModelIface *iface;

  g_return_if_fail (GT_IS_MODEL (model));

  iface = GT_MODEL_GET_IFACE (model);

  if (G_LIKELY (iface->remove_content)) {
    iface->remove_content (model, content);
    return;
  }

  g_warning ("GtModel of type '%s' does not implement remove_content ()",
             g_type_name (G_OBJECT_TYPE (model)));
}

void
gt_model_clear (GtModel *model)
{
  GtModelIface *iface;

  g_return_if_fail (GT_IS_MODEL (model));

  iface = GT_MODEL_GET_IFACE (model);

  if (G_LIKELY (iface->clear)) {
    iface->clear (model);
    return;
  }

  g_warning ("GtModel of type '%s' does not implement clear ()",
             g_type_name (G_OBJECT_TYPE (model)));
}

guint
gt_model_get_length (GtModel *model)
{
  GtModelIface *iface;

  g_return_val_if_fail (GT_IS_MODEL (model), 0);

  iface = GT_MODEL_GET_IFACE (model);

  if (G_LIKELY (iface->get_length)) {
    return iface->get_length (model);
  }

  g_warning ("GtModel of type '%s' does not implement get_length ()",
             g_type_name (G_OBJECT_TYPE (model)));

  return 0;
}

/**
 * gt_model_to_string :
 * @model: the model
 * @verbosity: the verbosity of the dump
 *
 * Return value: a string representation of the model
 *
 * Since: 0.2
 */
gchar *
gt_model_to_string (GtModel *model)
{
  GString *dump;

  dump = g_string_new (NULL);

  g_string_append_printf (dump,
                          "%s (%u elements)\n",
                          G_OBJECT_TYPE_NAME (model),
                          gt_model_get_length (model));

  {
    GtContent *content;
    const gchar *title;
    guint i, len;

    len = gt_model_get_length (model);
    for (i = 0; i < len - 1; i++) {
      content = gt_model_get_content (model, i);
      title = gt_content_get_metadata (content,
                                       GT_CONTENT_METADATA_TITLE);
      g_string_append_printf (dump, "%s, ", title);
    }

    content = gt_model_get_content (model, len - 1);
    title = gt_content_get_metadata (content, GT_CONTENT_METADATA_TITLE);
    g_string_append (dump, title);
  }

  return g_string_free (dump, FALSE);
}
