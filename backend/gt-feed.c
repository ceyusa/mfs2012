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

#include "gt-feed.h"

#include <libsoup/soup.h>
#include <json-glib/json-glib.h>

enum {
        PROP_APIKEY = 1,
        PROP_LAST
};

enum {
    RESPONSE_RECEIVED,
    LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = {0};
static GParamSpec *properties[PROP_LAST];

struct _GtFeedPrivate {
	char *apikey;
	SoupSession *session;
};

#define GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), GT_TYPE_FEED, GtFeedPrivate))

G_DEFINE_TYPE(GtFeed, gt_feed, G_TYPE_OBJECT)

static void
get_property(GObject *object, guint property_id,
	     GValue *value, GParamSpec *pspec)
{
        GtFeed *self = GT_FEED(object);

        switch (property_id) {
        case PROP_APIKEY:
                g_value_set_string(value, self->priv->apikey);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
}

static void
set_property(GObject *object, guint property_id,
	     const GValue *value, GParamSpec *pspec)
{
        GtFeed *self = GT_FEED(object);

        switch (property_id) {
        case PROP_APIKEY:
		gt_feed_set_apikey(self, g_value_get_string(value));
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
}

static void
dispose(GObject *object)
{
	GtFeed *self = GT_FEED(object);
	g_object_unref(self->priv->session);
	G_OBJECT_CLASS(gt_feed_parent_class)->dispose(object);
}

static void
finalize(GObject *object)
{
        GtFeed *self = GT_FEED(object);
        g_free(self->priv->apikey);
        G_OBJECT_CLASS(gt_feed_parent_class)->finalize(object);
}
static void
gt_feed_class_init(GtFeedClass *klass)
{
	GObjectClass *gclass = G_OBJECT_CLASS(klass);
	gclass->dispose = dispose;
	gclass->finalize = finalize;
	gclass->set_property = set_property;
	gclass->get_property = get_property;

	g_type_class_add_private(klass, sizeof(GtFeedPrivate));

	properties[PROP_APIKEY] = g_param_spec_string("api-key",
						      "API key",
						      "Trakt.tv API key",
						      NULL,
						      G_PARAM_READWRITE |
						      G_PARAM_CONSTRUCT |
						      G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties(gclass, PROP_LAST, properties);

	signals[RESPONSE_RECEIVED] =
                        g_signal_newv("response-received",
				      G_TYPE_FROM_CLASS(klass),
				      G_SIGNAL_RUN_LAST |
				      G_SIGNAL_NO_RECURSE |
				      G_SIGNAL_NO_HOOKS,
				      NULL,
				      NULL,
				      NULL,
				      g_cclosure_marshal_VOID__VOID,
				      G_TYPE_NONE,
				      0,
				      NULL);
}

static void
gt_feed_init(GtFeed *self)
{
	GtFeedPrivate *priv;

	self->priv = priv = GET_PRIVATE(self);
	priv->session = soup_session_async_new();
}

static void
set_error(SoupMessage *msg,
	  GSimpleAsyncResult *res)
{
	const char *reason = msg->reason_phrase;
	const char *response = msg->response_body->data;

	if (!response || *response == '\0')
		response = reason;

	switch (msg->status_code) {
	case SOUP_STATUS_CANT_RESOLVE:
	case SOUP_STATUS_CANT_CONNECT:
	case SOUP_STATUS_SSL_FAILED:
	case SOUP_STATUS_IO_ERROR:
		g_simple_async_result_set_error(res, GT_FEED_ERROR,
						GT_FEED_ERROR_NETWORK_ERROR,
						"Cannot connect to the server");
		return;
	case SOUP_STATUS_CANT_RESOLVE_PROXY:
	case SOUP_STATUS_CANT_CONNECT_PROXY:
		g_simple_async_result_set_error(res, GT_FEED_ERROR,
						GT_FEED_ERROR_PROXY_ERROR,
						"Cannot connect to the proxy server");
		return;
	case SOUP_STATUS_INTERNAL_SERVER_ERROR: /* 500 */
	case SOUP_STATUS_MALFORMED:
	case SOUP_STATUS_BAD_REQUEST: /* 400 */
		g_simple_async_result_set_error(res, GT_FEED_ERROR,
						GT_FEED_ERROR_PROTOCOL_ERROR,
						"Invalid request URI or header: %s",
						response);
		return;
	case SOUP_STATUS_UNAUTHORIZED: /* 401 */
	case SOUP_STATUS_FORBIDDEN: /* 403 */
		g_simple_async_result_set_error(res, GT_FEED_ERROR,
						GT_FEED_ERROR_AUTHENTICATION_REQUIRED,
						"Authentication required: %s",
						response);
    return;
	case SOUP_STATUS_NOT_FOUND: /* 404 */
		g_simple_async_result_set_error(res, GT_FEED_ERROR,
						GT_FEED_ERROR_NOT_FOUND,
						"The requested resource was not found: %s",
						response);
		return;
	case SOUP_STATUS_CONFLICT: /* 409 */
	case SOUP_STATUS_PRECONDITION_FAILED: /* 412 */
		g_simple_async_result_set_error(res, GT_FEED_ERROR,
						GT_FEED_ERROR_CONFLICT,
						"The entry has been modified since it was downloaded: %s",
						response);
		return;
	case SOUP_STATUS_CANCELLED:
		g_simple_async_result_set_error(res, GT_FEED_ERROR,
						GT_FEED_ERROR_CANCELLED,
						"Operation was cancelled");
		return;
	default:
		g_message("Unhandled status: %s",
			  soup_status_get_phrase(msg->status_code));
	}
}

static void
reply_cb(SoupSession *session,
	 SoupMessage *msg,
	 void *data)
{
	GSimpleAsyncResult *res = G_SIMPLE_ASYNC_RESULT(data);

	GtFeed *self = g_async_result_get_source_object(res);

	if (msg->status_code != SOUP_STATUS_OK)
		set_error(msg, res);
	else
		g_signal_emit(self, signals[RESPONSE_RECEIVED], 0);

	g_object_unref(self);

	g_simple_async_result_complete(res);
	g_object_unref(res);
}

static const char *search_type[] = {
	[GT_FEED_SEARCH_MOVIES] = "movies",
	[GT_FEED_SEARCH_SHOWS] = "shows",
	[GT_FEED_SEARCH_EPISODES] = "episodes",
};

static inline char *
build_url(GtFeedSearchType type, const char *query, const char *apikey)
{
	char *q = soup_uri_encode(query, NULL);
	char *url = g_strconcat("http://api.trakt.tv/search/",
				search_type[type], ".json/",
				apikey, "/", q, NULL);
	g_free(q);

	return url;
}

gboolean
gt_feed_search(GtFeed *self,
	       GtFeedSearchType type,
	       const char *query,
	       GAsyncReadyCallback cb,
	       void *data)
{
	GtFeedPrivate *priv = self->priv;
	SoupMessage *msg;
	GSimpleAsyncResult *res;

	g_return_val_if_fail(query != NULL, FALSE);
	g_return_val_if_fail(type >= GT_FEED_SEARCH_MOVIES ||
			     type < GT_FEED_SEARCH_MAX, FALSE);

	{
		char *url = build_url(type, query, priv->apikey);
		msg = soup_message_new(SOUP_METHOD_GET, url);
		if (!msg)
			g_warning("Malformed URL: %s", url);
		g_free(url);
	}

	if (!msg)
		return FALSE;

	res = g_simple_async_result_new(G_OBJECT(self), cb, data, NULL);
	g_simple_async_result_set_op_res_gpointer(res, msg, NULL);

	soup_session_queue_message(priv->session, msg, reply_cb, res);

	return TRUE;
}

GVariant *
parse(GtFeed *self, SoupMessage *msg, GError **err)
{
	const char *content = msg->response_body->data;
	goffset length = msg->response_body->length;

	if (!content || length <= 0)
		return NULL;

	return json_gvariant_deserialize_data(content, length, NULL, err);
}

GVariant *
gt_feed_search_finish(GtFeed *self, GAsyncResult *result, GError **err)
{
	GSimpleAsyncResult *res = G_SIMPLE_ASYNC_RESULT(result);

	if (g_simple_async_result_propagate_error(res, err))
		return FALSE;

	SoupMessage *msg = g_simple_async_result_get_op_res_gpointer(res);
	return parse(self, msg, err);
}

void
gt_feed_set_apikey(GtFeed *self, const gchar *apikey)
{
	GtFeedPrivate *priv;

	g_return_if_fail(self);

	if (!apikey)
		return;

	priv = self->priv;
	g_free(priv->apikey);
	priv->apikey = g_strdup(apikey);

	g_object_notify_by_pspec(self, properties[PROP_APIKEY]);
}


GQuark
gt_feed_error_quark(void)
{
	return g_quark_from_static_string ("gt-feed-error-quark");
}
