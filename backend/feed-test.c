#include <glib.h>

#include "gt-feed.h"

/*
 *  CONSTANTS
 */

// File to search the API key in case is not given by the user
#define APIKEY_FILE ".trakttv_apikey"

/*
 *  VARIABLES
 */

// Commandline options
static char *apikey;	// Text of the API developer key
static char *query;	// Text for the user query
static gboolean search_movies = FALSE;	// Type of search (movies, shows or episodes)
static gboolean search_shows = FALSE;
static gboolean search_episodes = FALSE;

GtFeedSearchType searchtype;
static GMainLoop *loop;	// GLib/GTK+ Main event loop 

// Options list admited by the script
static GOptionEntry entries[] = {
        {
		.long_name = "apikey",
		.short_name = 'k',
		.arg = G_OPTION_ARG_STRING,
		.arg_data = &apikey,
		.description = "trakt.tv API key (if not given key will be searched in ~/.trakttv_apikey file)"
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

/*
 *  FUNCTIONS
 */

// Fonction which is called when the query results are sent by the Trackt.tv server
void
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

// This fonction is the fonction that sends the query to the trackt.tv
gboolean
query_trakttv(void *data)
{
	// Instance creation and api-key property setting
	GtFeed *feed = g_object_new(GT_TYPE_FEED, "api-key", apikey, NULL);
	if (!gt_feed_search(feed, searchtype, query, cb, NULL)) {
		g_main_loop_quit(loop);
	}
        g_object_unref(feed);

	return FALSE;
}

/*
 *  MAIN FUNCTIONS
 */
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
		
		// Query argument is mandatory
		if (!query) {
			g_critical("No query provided!\n\n");
			
			g_print(g_option_context_get_help (context, TRUE, NULL));
			return -1;
		}

		// We get the search type from -m, -s, -e flags (Movies by default)
		searchtype = GT_FEED_SEARCH_MOVIES;  // Search on movies by default
		if (search_movies)  {
			searchtype = GT_FEED_SEARCH_MOVIES;
		}
		else if (search_shows)  {
			searchtype = GT_FEED_SEARCH_SHOWS;
		}
		else if (search_episodes) {
			searchtype = GT_FEED_SEARCH_EPISODES;
		}

		// If the API key is not given from arguments we read it from ~/.trakttv_apikey
		if (!apikey)  {
			gchar *apikey_file_path;
			gchar *apikey_file_contents;
			
			apikey_file_path = g_strdup_printf("%s/%s", g_get_home_dir(), APIKEY_FILE);
			if (g_file_test(apikey_file_path, G_FILE_TEST_EXISTS))  {
				if (!g_file_get_contents(apikey_file_path, &apikey_file_contents, NULL, &error))  {
					g_print(g_strdup_printf("Reading the API key from file %s failed: %s\n", apikey_file_path, error->message));
					return -1;
				}
				
				// First line is asigned to apikey variable. File is supposed to contain only API KEY
				apikey = g_strdup(apikey_file_contents);
				g_strstrip(apikey);
				
			}
			else  {
				g_critical(g_strdup_printf("No API key provided nor %s file exists!\n\n", apikey_file_path));
				
				g_print(g_option_context_get_help (context, TRUE, NULL));
				return -1;
			}
		}
		
		// End of argument parsing, we free the context
		g_option_context_free(context);

	}

	loop = g_main_loop_new(NULL, TRUE);
	g_idle_add(query_trakttv, NULL);
	g_main_loop_run(loop);

    return 0;
}
