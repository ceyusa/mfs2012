#include <glib.h>

#include "gt-feed-server.h"

#define APIKEY_FILE "gtrakt.conf"

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

static inline gchar *
read_api_key(gchar *file)
{
	gchar *key = NULL;
	GError *error = NULL;

	GKeyFile *cfg = g_key_file_new();

	if (!g_key_file_load_from_file(cfg, file, 0, &error)) {
		g_print("Reading the API key from file %s failed: %s\n",
			file, error->message);
		goto bail;
	}

	key = g_key_file_get_value(cfg, "trakt", "api-key", &error);
	if (!key || error) {
		g_print("Can't find API key in config file %s: %s\n",
			file, error->message);
		goto bail;
	}

bail:
	if (error)
		g_error_free(error);

	g_key_file_free(cfg);

	return key;
}

static inline gboolean
write_api_key(gchar *apikey, gchar *file)
{
	gchar *data;
	gssize len;
	GError *error = NULL;
	gboolean ret = FALSE;

	GKeyFile *cfg = g_key_file_new();

	g_key_file_set_value(cfg, "trakt", "api-key", apikey);
	data = g_key_file_to_data(cfg, (gsize *) &len, &error);
	if (!data || error) {
		g_print("Can't generate config file %s: %s\n",
			file, error->message);
		goto bail;
	}

	if (!g_file_set_contents(file, data, len, &error)) {
		g_print("Can't write config file %s: %s\n",
			file, error->message);
		goto bail;
	}

	ret = TRUE;

bail:
	g_key_file_free(cfg);

	g_free(data);

	if (error)
		g_error_free(error);

	return ret;

}

static gboolean
read_or_write_api_key(gchar **apikey)
{
	gchar *file = NULL;
	gboolean ret = FALSE;

	file = g_build_path(G_DIR_SEPARATOR_S,
			    g_get_user_config_dir(),
			    APIKEY_FILE, NULL);

	if (!apikey || !*apikey) {
		if ((*apikey = read_api_key(file)) == NULL) {
			goto bail;
		}
	} else {
		if (!write_api_key(*apikey, file)) {
			goto bail;
		}
	}

	ret = TRUE;

bail:
	g_free(file);

	return ret;
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

		if (!read_or_write_api_key(&apikey))
			goto bail;

		ok =  TRUE;

	bail:
		if (!ok)
			g_print(g_option_context_get_help (context, TRUE, NULL));

		g_option_context_free(context);

		if (!ok)
			return -1;
	}

	loop = g_main_loop_new(NULL, TRUE);
	server = g_object_new(GT_TYPE_FEED_SERVER, "api-key", apikey, NULL);

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
