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


#ifndef __GT_CONTENT_H__
#define __GT_CONTENT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GT_TYPE_CONTENT            \
    (gt_content_get_type ())
#define GT_CONTENT(obj)            \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GT_TYPE_CONTENT, GtContent))
#define GT_IS_CONTENT(obj)         \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GT_TYPE_CONTENT))
#define GT_CONTENT_IFACE(iface)    \
    (G_TYPE_CHECK_CLASS_CAST ((iface), GT_TYPE_CONTENT, GtContentIface))
#define GT_IS_CONTENT_IFACE(iface) \
    (G_TYPE_CHECK_CLASS_TYPE ((iface), GT_TYPE_CONTENT))
#define GT_CONTENT_GET_IFACE(obj)  \
    (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GT_TYPE_CONTENT, GtContentIface))

typedef struct _GtContent         GtContent;
typedef struct _GtContentIface    GtContentIface;
typedef struct _GtContentProperty GtContentProperty;

#include "gt-model.h"

typedef enum {
/* Media: */
 GT_CONTENT_METADATA_TITLE
 GT_CONTENT_METADATA_YEAR
 GT_CONTENT_METADATA_COUNTRY
 GT_CONTENT_METADATA_OVERVIEW
 GT_CONTENT_METADATA_RUNTIME
 GT_CONTENT_METADATA_IMDB_ID
 GT_CONTENT_METADATA_IMAGE
 GT_CONTENT_METADATA_CERTIFICATION
 GT_CONTENT_METADATA_TRAKT_URL
 GT_CONTENT_METADATA_GENRES
/* Movie (Media): */
 GT_CONTENT_METADATA_TRAILER
 GT_CONTENT_METADATA_TAGLINE
 GT_CONTENT_METADATA_RELEASED
 GT_CONTENT_METADATA_TMDB_ID
/* Show (Media): */
 GT_CONTENT_METADATA_FIRST_AIRED
 GT_CONTENT_METADATA_AIR_DAY
 GT_CONTENT_METADATA_AIR_TIME
 GT_CONTENT_METADATA_NETWORK
 GT_CONTENT_METADATA_THETVDB_ID
/* Episode: */
 GT_CONTENT_METADATA_SHOW
 GT_CONTENT_METADATA_EPISODE_TITLE
 GT_CONTENT_METADATA_OVERVIEW
 GT_CONTENT_METADATA_SEASON
 GT_CONTENT_METADATA_NUMBER
 GT_CONTENT_METADATA_FIRST_AIRED
 GT_CONTENT_METADATA_TRAKT_URL
 GT_CONTENT_METADATA_IMAGES
 GT_CONTENT_METADATA_WATCHED
/* Image: */
 GT_CONTENT_METADATA_TYPE
 GT_CONTENT_METADATA_URI
} GtContentMetadata;

typedef void (*GtContentMetadataCb) (GtContentMetadata key,
                                      const gchar *value,
                                      gpointer data);

struct _GtContentIface
{
  GTypeInterface g_iface;

  /* virtual functions */
  const gchar * (*get_metadata) (GtContent         *content,
                                 GtContentMetadata key);
  void (*set_metadata) (GtContent         *content,
                        GtContentMetadata key,
                        const gchar       *value);
  void (*foreach_metadata) (GtContent           *content,
                            GtContentMetadataCb callback,
                            gpointer            data);

  void (*open) (GtContent *content,
                GtModel   *context);
};

GType gt_content_get_type (void) G_GNUC_CONST;

const gchar *gt_content_get_metadata (GtContent         *content,
                                      GtContentMetadata key);
void gt_content_set_metadata (GtContent         *content,
                              GtContentMetadata key,
                              const gchar       *value);

void gt_content_foreach_metadata (GtContent           *content,
                                  GtContentMetadataCb callback,
                                  gpointer            data);

void gt_content_open (GtContent *content,
                      GtModel   *context);

const gchar *gt_content_metadata_key_to_string  (GtContentMetadata key);

G_END_DECLS

#endif /* __GT_CONTENT_H__ */
