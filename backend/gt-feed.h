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

#ifndef __GT_FEED_H__
#define __GT_FEED_H__

#include <gio/gio.h>

G_BEGIN_DECLS

typedef enum {
	GT_FEED_SEARCH_MOVIES,
	GT_FEED_SEARCH_SHOWS,
	GT_FEED_SEARCH_EPISODES,
} GtFeedSearchType;

typedef enum {
	GT_FEED_ERROR_UNAVAILABLE = 1,
	GT_FEED_ERROR_PROTOCOL_ERROR,
	GT_FEED_ERROR_AUTHENTICATION_REQUIRED,
	GT_FEED_ERROR_NOT_FOUND,
	GT_FEED_ERROR_CONFLICT,
	GT_FEED_ERROR_FORBIDDEN,
	GT_FEED_ERROR_NETWORK_ERROR,
	GT_FEED_ERROR_PROXY_ERROR,
	GT_FEED_ERROR_CANCELLED,
} GtFeedError;

#define GT_FEED_ERROR				\
	(gt_feed_error_quark())

#define GT_TYPE_FEED				\
	(gt_feed_get_type())
#define GT_FEED(obj)				\
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GT_TYPE_FEED, GtFeed))

typedef struct _GtFeedPrivate GtFeedPrivate;
typedef struct _GtFeed GtFeed;
typedef struct _GtFeedClass GtFeedClass;

struct _GtFeed {
	GObject parent;
	GtFeedPrivate *priv;
};

struct _GtFeedClass {
	GObjectClass parent_class;
};

GQuark gt_feed_error_quark(void) G_GNUC_CONST;

GType gt_feed_get_type(void) G_GNUC_CONST;

gboolean gt_feed_search(GtFeed *self, GtFeedSearchType type, const char *query,
			GAsyncReadyCallback cb,  void *data);
gboolean gt_feed_search_finish(GtFeed *self, GAsyncResult *res, GError **err);

G_END_DECLS

#endif /* __GT_FEED_H__ */
