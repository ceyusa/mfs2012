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

#include <stdlib.h>
#include <stdio.h>

#include "gt-enum-types.h"
#include "gt-content.h"

static void
gt_content_base_finalize (gpointer g_iface)
{
}

static void
gt_content_base_init (gpointer g_iface)
{
  static gboolean is_initialized = FALSE;
  GParamSpec *param;

  if (!is_initialized) {
    param = g_param_spec_boolean ("last-position-start",
                                  "Last position start",
                                  "Start media content from last position",
                                  TRUE,
                                  G_PARAM_READWRITE);
    g_object_interface_install_property (g_iface, param);

    is_initialized = TRUE;
  }
}

GType
gt_content_get_type (void)
{
  static GType our_type = 0;

  if (G_UNLIKELY (our_type == 0))
    {
      GTypeInfo content_info = {
        sizeof (GtContentIface),
        gt_content_base_init,
        gt_content_base_finalize
      };

      our_type = g_type_register_static (G_TYPE_INTERFACE,
                                         I_("GtContent"),
                                         &content_info, 0);
    }

  return our_type;
}

/**
 * gt_content_get_metadata:
 * @content: a #GtContent
 * @key: a #GtContentMetadata
 *
 * Retrieves a string for @key for this content.
 *
 * Return value: The string corresponding to the @key
 */
const gchar *
gt_content_get_metadata (GtContent         *content,
                         GtContentMetadata  key)
{
  GtContentIface *iface;

  g_return_val_if_fail (GT_IS_CONTENT (content), NULL);

  iface = GT_CONTENT_GET_IFACE (content);

  if (G_LIKELY (iface->get_metadata))
    return iface->get_metadata (content, key);

  g_warning ("GtContent of type '%s' does not implement get_metadata()",
             g_type_name (G_OBJECT_TYPE (content)));

  return NULL;
}

/**
 * gt_content_get_metadata_fallback:
 * @content: a #GtContent
 * @key: a #GtContentMetadata
 *
 * Retrieves a string for @key for this content.
 *
 * Return value: The string corresponding to the @key. Because this string
 * may be dynamically created it should be freed with g_free when no longer
 * needed.
 */
gchar *
gt_content_get_metadata_fallback (GtContent         *content,
                                  GtContentMetadata key)
{
  GtContentIface *iface;

  g_return_val_if_fail (GT_IS_CONTENT (content), NULL);

  iface = GT_CONTENT_GET_IFACE (content);

  if (G_LIKELY (iface->get_metadata_fallback))
    return iface->get_metadata_fallback (content, key);

  return NULL;
}

/**
 * gt_content_set_metadata:
 * @content: a #GtContent
 * @key: a #GtContentMetadata
 * @value: a string corresponding to the new value of @key
 *
 * Sets a string for @key for this content.
 */
void
gt_content_set_metadata (GtContent          *content,
                         GtContentMetadata  key,
                         const gchar        *value)
{
  GtContentIface *iface;

  g_return_if_fail (GT_IS_CONTENT (content));

  iface = GT_CONTENT_GET_IFACE (content);

  if (G_LIKELY (iface->set_metadata)) {
    iface->set_metadata (content, key, value);
    return;
  }

  g_warning ("GtContent of type '%s' does not implement set_metadata()",
             g_type_name (G_OBJECT_TYPE (content)));
}

void
gt_content_foreach_metadata (GtContent           *content,
                             GtContentMetadataCb  callback,
                             gpointer              data)
{
  GtContentIface *iface;

  g_return_if_fail (GT_IS_CONTENT (content));
  g_return_if_fail (callback != NULL);

  iface = GT_CONTENT_GET_IFACE (content);

  if (iface->foreach_metadata) {
    iface->foreach_metadata (content, callback, data);
    return;
  }

  g_warning ("GtContent of type '%s' does not implement foreach_metadata()",
             g_type_name (G_OBJECT_TYPE (content)));
}

/**
 * gt_content_open:
 * @content: a #GtContent
 *
 * Open a content.
 *
 * Since: 0.2
 */
void
gt_content_open (GtContent *content, GtModel *context)
{
  GtContentIface *iface;

  g_return_if_fail (GT_IS_CONTENT (content));

  iface = GT_CONTENT_GET_IFACE (content);

  if (iface->open) {
    iface->open (content, context);
    return;
  }

  g_warning ("GtContent of type '%s' does not implement open()",
             g_type_name (G_OBJECT_TYPE (content)));
}

const gchar *
gt_content_metadata_key_to_string (GtContentMetadata key)
{
  return gt_enum_to_string (GT_TYPE_CONTENT_METADATA, key);
}
