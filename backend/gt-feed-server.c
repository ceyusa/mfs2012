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

#include "gt-feed-server.h"
#include "gt-feed-server-private.h"

#include "gt-feed.h"

G_DEFINE_TYPE(GtFeedServer, gt_feed_server, G_TYPE_OBJECT)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE((o), GT_TYPE_FEED_SERVER, GtFeedServerPrivate))

enum {
        PROP_APIKEY = 1,
};

struct _GtFeedServerPrivate {
	GtFeed *feed;
	GDBusNodeInfo *dbusinfo;
};

static void
get_property(GObject *object, guint property_id,
	     GValue *value, GParamSpec *pspec)
{
        GtFeedServer *self = GT_FEED_SERVER(object);
	GObject *feed = G_OBJECT(self->priv->feed);

        switch (property_id) {
        case PROP_APIKEY:
		g_object_get_property(feed, "api-key", value);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
}

static void
set_property(GObject *object, guint property_id,
	     const GValue *value, GParamSpec *pspec)
{
        GtFeedServer *self = GT_FEED_SERVER(object);
	GObject *feed = G_OBJECT(self->priv->feed);

        switch (property_id) {
        case PROP_APIKEY:
		g_object_set_property(feed, "api-key", value);
                break;
        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        }
}

static void
dispose(GObject *object)
{
        GtFeedServer *self = GT_FEED_SERVER(object);
        g_object_unref(self->priv->feed);

        G_OBJECT_CLASS(gt_feed_server_parent_class)->dispose(object);
}

static void
finalize(GObject *object)
{
        GtFeedServer *self = GT_FEED_SERVER(object);
        g_dbus_node_info_unref(self->priv->dbusinfo);

        G_OBJECT_CLASS(gt_feed_server_parent_class)->finalize(object);
}

static void
gt_feed_server_class_init(GtFeedServerClass *klass)
{
        GObjectClass *gclass = G_OBJECT_CLASS(klass);

        g_type_class_add_private(klass, sizeof(GtFeedServerPrivate));

	gclass->dispose = dispose;
	gclass->finalize = finalize;
	gclass->set_property = set_property;
	gclass->get_property = get_property;

	g_object_class_install_property
		(gclass, PROP_APIKEY,
		 g_param_spec_string("api-key", "API key", "Trakt.tv API key",
				     NULL,
				     G_PARAM_READWRITE |
				     G_PARAM_CONSTRUCT |
				     G_PARAM_STATIC_STRINGS));
}

static void
gt_feed_server_init(GtFeedServer *self)
{
	GError *error = NULL;
	GtFeedServerPrivate *priv;

        priv = self->priv = GET_PRIVATE(self);

	priv->feed = g_object_new(GT_TYPE_FEED, NULL);

	priv->dbusinfo = g_dbus_node_info_new_for_xml(interface_xml, &error);
	if (error) {
		g_critical("Couldn't parse gdbus node info: %s",
			   error->message);
		g_error_free(error);
	}
}

GtFeedServer*
gt_feed_server_new(const gchar *apikey)
{
        return g_object_new(GT_TYPE_FEED_SERVER, "api-key", apikey);
}

static void
cb(GObject *source,
   GAsyncResult *res,
   void *data)
{
	GVariant *content;
	GError *error = NULL;
	GtFeed *feed = GT_FEED(source);
	GDBusMethodInvocation *invocation = data;

	content = gt_feed_search_finish(feed, res, &error);
	if (error) {
		g_dbus_method_invocation_take_error(invocation, error);
		return;
	}

	g_dbus_method_invocation_return_value(invocation,
					      g_variant_new_tuple(&content,1));
}

static void
query(GtFeedServer *self,
      GVariant *parameters,
      GDBusMethodInvocation *invocation)
{
	gchar *query;
	gint type;
	gboolean res;
	GtFeed *feed = self->priv->feed;

	query = NULL;
	g_variant_get(parameters, "(si)", &query, &type);

	if (!query) {
		g_dbus_method_invocation_return_error
			(invocation,
			 GT_FEED_SERVER_ERROR,
			 GT_FEED_SERVICE_ERROR_MISSING_PARAMETER,
			 "No query was specified.");
		return;
	}

	if (type < GT_FEED_SEARCH_MOVIES || type >= GT_FEED_SEARCH_MAX) {
		g_dbus_method_invocation_return_error
			(invocation,
			 GT_FEED_SERVER_ERROR,
			 GT_FEED_SERVICE_ERROR_MISSING_PARAMETER,
			 "Search type is not valid.");
		return;
	}

	res = gt_feed_search(feed, type, query, cb, invocation);
	g_free(query);

	if (!res) {
		g_dbus_method_invocation_return_error
			(invocation,
			 GT_FEED_SERVER_ERROR,
			 GT_FEED_SERVICE_ERROR_INVALID_QUERY,
			 "The specified query is invalid.");
	}
}

static void
method_call(GDBusConnection *connection,
	    const gchar *sender,
	    const gchar *object_path,
	    const gchar *interface_name,
	    const gchar *method_name,
	    GVariant *parameters,
	    GDBusMethodInvocation *invocation,
	    gpointer data)
{
	GtFeedServer *self = GT_FEED_SERVER(data);

	if (g_strcmp0(method_name, "Query") == 0) {
		query(self, parameters, invocation);
	} else {
		g_object_unref(invocation);
	}
}


static const GDBusInterfaceVTable iface_vtable = {
  .method_call = method_call,
};

guint
gt_feed_server_register(GtFeedServer *self,
			GDBusConnection *connection,
			const gchar *path,
			GError **error)
{
	guint res;
	GDBusNodeInfo *dbusinfo;

	g_return_val_if_fail(self &&
			     self->priv &&
			     self->priv->dbusinfo, 0);

	dbusinfo = self->priv->dbusinfo;

	res = g_dbus_connection_register_object(connection,
						path,
						dbusinfo->interfaces[0],
						&iface_vtable,
						self,
						NULL,
						error);

	return res;
}

void
gt_feed_server_set_apikey(GtFeedServer *self, const gchar *apikey)
{
	g_return_if_fail(self && self->priv && self->priv->feed && apikey);

	g_object_set(self->priv->feed, "api-key", apikey, NULL);
}

static const GDBusErrorEntry dbus_errors[] = {
	{
		.error_code = GT_FEED_SERVICE_ERROR_MISSING_PARAMETER,
		.dbus_error_name = "org.mfs.GtraktError.MissingParameter",
	},
	{
		.error_code = GT_FEED_SERVICE_ERROR_INVALID_QUERY,
		.dbus_error_name = "org.mfs.GtraktError.InvalidQuery",
	},
};

GQuark
gt_feed_server_error_quark(void)
{
	static volatile gsize quark = 0;

	g_dbus_error_register_error_domain("feed-server-error-quark",
					   &quark,
					   dbus_errors,
					   G_N_ELEMENTS(dbus_errors));

	return (GQuark) quark;
}
