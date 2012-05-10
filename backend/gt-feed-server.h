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

#ifndef _GT_FEED_SERVER
#define _GT_FEED_SERVER

#include <glib-object.h>
#include <gio/gio.h>

#include "gt-feed.h"

G_BEGIN_DECLS

typedef enum {
	GT_FEED_SERVICE_ERROR_MISSING_PARAMETER = 1,
        GT_FEED_SERVICE_ERROR_INVALID_QUERY,
} GtFeedServiceError;

#define GT_FEED_SERVER_ERROR \
  (gt_feed_server_error_quark())

#define GT_TYPE_FEED_SERVER \
  gt_feed_server_get_type()
#define GT_FEED_SERVER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GT_TYPE_FEED_SERVER, GtFeedServer))
#define GT_FEED_SERVER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GT_TYPE_FEED_SERVER, GtFeedServerClass))
#define GT_IS_FEED_SERVER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GT_TYPE_FEED_SERVER))
#define GT_IS_FEED_SERVER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GT_TYPE_FEED_SERVER))
#define GT_FEED_SERVER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GT_TYPE_FEED_SERVER, GtFeedServerClass))

typedef struct _GtFeedServerPrivate GtFeedServerPrivate;
typedef struct _GtFeedServer GtFeedServer;
typedef struct _GtFeedServerClass GtFeedServerClass;

struct _GtFeedServer {
        GObject parent;

        /* <private> */
        GtFeedServerPrivate *priv;
};

struct _GtFeedServerClass {
        GObjectClass parent_class;
};

GQuark gt_feed_server_error_quark(void) G_GNUC_CONST;

GType gt_feed_server_get_type (void) G_GNUC_CONST;

GtFeedServer* gt_feed_server_new (const gchar *apikey);

guint gt_feed_server_register (GtFeedServer *self,
                               GDBusConnection *connection,
                               const gchar *path,
                               GError **error);

void gt_feed_server_set_apikey(GtFeedServer *self, const gchar *apikey);

G_END_DECLS

#endif /* _GT_FEED_SERVER */
