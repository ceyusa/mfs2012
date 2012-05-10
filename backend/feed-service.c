#include <glib.h>

#include "gt-feed-server.h"

static GMainLoop *loop;

static char *apikey;

static GOptionEntry entries[] = {
        {
		.long_name = "apikey",
		.short_name = 'k',
		.arg = G_OPTION_ARG_STRING,
		.arg_data = &apikey,
		.description = "trakt.tv API key",
	},
        {
		.long_name = NULL,
	}
};

static void
print_peer_credentials(GDBusConnection *connection)
{
	GCredentials *credentials;
	gchar *s;

	credentials = g_dbus_connection_get_peer_credentials(connection);
	if (credentials == NULL)
		s = g_strdup("(no credentials received)");
	else
		s = g_credentials_to_string(credentials);


	g_print("Client connected.\n"
		"Peer credentials: %s\n"
		"Negotiated capabilities: unix-fd-passing=%d\n",
		s,
		g_dbus_connection_get_capabilities(connection) &
		G_DBUS_CAPABILITY_FLAGS_UNIX_FD_PASSING);

	g_free(s);
}

static void
on_bus_acquired(GDBusConnection *connection,
		const gchar *name,
		gpointer data)
{
	GtFeedServer *server = GT_FEED_SERVER(data);
	guint registration_id;
	GError *error = NULL;

	print_peer_credentials(connection);

	registration_id = gt_feed_server_register(server,
						  connection,
						  "/org/mfs/Gtrakt/FeedServer",
						  &error);

	if (error || registration_id <= 0) {
		g_critical("Couldn't register the feed server: %s\nQuitting...",
			   error->message);
		g_error_free(error);
		g_main_loop_quit(loop);
	}
}

static void
on_name_acquired(GDBusConnection *connection,
		 const gchar *name,
		 gpointer data)
{
}

static void
on_name_lost(GDBusConnection *connection,
	     const gchar *name,
	     gpointer data)
{
	g_print("Could not acquire name. Quitting...\n");
	g_main_loop_quit(loop);
}

int
main (int argc, char **argv)
{
	guint owner_id;
	GtFeedServer *server;

        g_type_init();

	/* argument parsing */
	{
		gboolean ok = FALSE;
		GError *error = NULL;
		GOptionContext *context;

		context = g_option_context_new(" - GTrakt Feed D-BUS Service");
		g_option_context_add_main_entries(context, entries, NULL);
		if (!g_option_context_parse(context, &argc, &argv, &error)) {
			g_print("option parsing failed: %s\n", error->message);
			g_error_free(error);
			goto bail;
		}

		ok =  TRUE;

	bail:
		if (!ok)
			g_print(g_option_context_get_help (context, TRUE, NULL));

		g_option_context_free(context);

		if (!ok)
			return -1;
	}

	loop = g_main_loop_new(NULL, TRUE);

	server = g_object_new(GT_TYPE_FEED_SERVER, NULL);

	if (apikey)
		g_object_set(server, "api-key", apikey, NULL);

	owner_id = g_bus_own_name(G_BUS_TYPE_SESSION,
				  "org.mfs.Gtrakt.FeedServer",
				  G_BUS_NAME_OWNER_FLAGS_NONE,
				  on_bus_acquired,
				  on_name_acquired,
				  on_name_lost,
				  server,
				  NULL);

	g_main_loop_run(loop);

	g_bus_unown_name(owner_id);
	g_object_unref(server);
	g_main_loop_unref(loop);

	return 0;
}
