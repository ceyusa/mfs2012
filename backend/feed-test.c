#include <glib.h>

#include "gt-feed.h"

#define APIKEY_FILE ".trakttv_apikey"

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

	if (!gt_feed_search_finish(feed, res, &err)) {
		g_warning("Error: %s", err->message);
		goto bail;
	}

	GVariant *content;
	g_object_get(feed, "content", &content, NULL);
	if (content) {
		char *str = g_variant_print(content, TRUE);
		g_print("variant content (%c) = %s",
			g_variant_classify(content), str);
		g_free(str);
	}

bail:
	g_main_loop_quit(loop);
	return;
}

static gboolean
query_trakttv(void *data)
{
	GtFeed *feed = g_object_new(GT_TYPE_FEED, "api-key", apikey, NULL);
	if (!gt_feed_search(feed, searchtype, query, cb, NULL)) {
		g_main_loop_quit(loop);
	}
        g_object_unref(feed);

	return FALSE;
}

static gboolean
read_api_key()
{
	gchar *file = NULL;
	gchar *contents = NULL;
	GError *error = NULL;
	gboolean ret = FALSE;

	file = g_build_path(g_get_user_config_dir(), APIKEY_FILE, NULL);
	if (g_file_test(file, G_FILE_TEST_EXISTS))  {
		g_print("No API key provided nor %s file exists!\n", file);
		goto bail;
	}

	if (!g_file_get_contents(file, &contents, NULL, &error))  {
		g_print("Reading the API key from file %s failed: %s\n",
			file, error->message);
		g_error_free(error);
		goto bail;
	}

	apikey = contents;
	g_strstrip(apikey);

	ret = TRUE;

bail:
	g_free(file);
	g_free(contents);

	return ret;
}

int
main (int argc, char **argv)
{
        g_type_init();

	/* argument parsing */
	{
		GError *error = NULL;
		GOptionContext *context;

		context = g_option_context_new(" - GTrakt Feed test");
		g_option_context_add_main_entries(context, entries, NULL);
		if (!g_option_context_parse(context, &argc, &argv, &error)) {
			g_print("option parsing failed: %s\n", error->message);
			return -1;
		}

		if (!query) {
			g_critical("No query provided!\n\n");

			g_print(g_option_context_get_help (context, TRUE, NULL));
			return -1;
		}

		if (!apikey && !read_api_key()) {
			g_print(g_option_context_get_help(context, TRUE, NULL));
			return -1;
		}

		g_option_context_free(context);
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
