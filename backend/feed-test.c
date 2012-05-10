#include <glib.h>

#include "gt-feed.h"

static char *apikey;
static char *query;

static GMainLoop *loop;

static gboolean search_movies = FALSE;
static gboolean search_shows = FALSE;
static gboolean search_episodes = FALSE;

GtFeedSearchType searchtype;

static GOptionEntry entries[] = {
        {
		.long_name = "apikey",
		.short_name = 'k',
		.arg = G_OPTION_ARG_STRING,
		.arg_data = &apikey,
		.description = "trakt.tv API key",
	},
        {
		.long_name = "movies",
		.short_name = 'm',
		.arg = G_OPTION_ARG_NONE,
		.arg_data = &search_movies,
		.description = "Limit search only to movies (default value)",
	},
        {
		.long_name = "shows",
		.short_name = 's',
		.arg = G_OPTION_ARG_NONE,
		.arg_data = &search_shows,
		.description = "Limit search only to shows",
	},
	{
		.long_name = "episodes",
		.short_name = 'e',
		.arg = G_OPTION_ARG_NONE,
		.arg_data = &search_episodes,
		.description = "Limit search only to episodes",
	},
	{
		.long_name = "query",
		.short_name = 'q',
		.arg = G_OPTION_ARG_STRING,
		.arg_data = &query,
		.description = "term to search",
	},
        {
		.long_name = NULL,
	}
};

static void
cb(GObject *source, GAsyncResult *res, void *data)
{
	GError *err = NULL;
	GtFeed *feed = GT_FEED(source);
	GVariant *content;

	content = gt_feed_search_finish(feed, res, &err);
	if (err) {
		g_warning("Error: %s", err->message);
		g_error_free(err);
		goto bail;
	}

	char *str = g_variant_print(content, TRUE);
	g_print("variant content (%c) = %s",
		g_variant_classify(content), str);
	g_free(str);

	g_variant_unref(content);

bail:
	g_main_loop_quit(loop);
	return;
}

static void
response_received_callback (GtFeed *feed, gpointer user_data)
{
        g_message ("Response received!");
}

static gboolean
query_trakttv(void *data)
{
	GtFeed *feed = g_object_new(GT_TYPE_FEED, "api-key", apikey, NULL);
	g_signal_connect(feed, "response-received",
			 G_CALLBACK(response_received_callback), NULL);
	if (!gt_feed_search(feed, searchtype, query, cb, NULL)) {
		g_main_loop_quit(loop);
	}
        g_object_unref(feed);

	return FALSE;
}

int
main (int argc, char **argv)
{
        g_type_init();

	/* argument parsing */
	{
		gboolean ok = FALSE;
		GError *error = NULL;
		GOptionContext *context;

		context = g_option_context_new(" - GTrakt Feed test");
		g_option_context_add_main_entries(context, entries, NULL);
		if (!g_option_context_parse(context, &argc, &argv, &error)) {
			g_print("option parsing failed: %s\n", error->message);
			g_error_free(error);
			goto bail;
		}

		if (!query) {
			g_print("No query provided!\n");
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

	searchtype = GT_FEED_SEARCH_MOVIES;
	if (search_movies)  {
		searchtype = GT_FEED_SEARCH_MOVIES;
	}
	else if (search_shows)  {
		searchtype = GT_FEED_SEARCH_SHOWS;
	}
	else if (search_episodes) {
		searchtype = GT_FEED_SEARCH_EPISODES;
	}

	loop = g_main_loop_new(NULL, TRUE);
	g_idle_add(query_trakttv, NULL);
	g_main_loop_run(loop);

	return 0;
}
