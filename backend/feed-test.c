#include <glib.h>

#include "gt-feed.h"

static char *apikey;
static char *query = "batman";
static GMainLoop *loop;

static GOptionEntry entries[] = {
        {
		.long_name = "apikey",
		.short_name = 'k',
		.arg = G_OPTION_ARG_STRING,
		.arg_data = &apikey,
		.description = "trakt.tv API key",
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

gboolean
test1(void *data)
{
	GtFeed *feed = g_object_new(GT_TYPE_FEED, "api-key", apikey, NULL);
	if (!gt_feed_search(feed, GT_FEED_SEARCH_MOVIES, query, cb, NULL)) {
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
		GError *error = NULL;
		GOptionContext *context;

		context = g_option_context_new(" - GTrakt Feed test");
		g_option_context_add_main_entries(context, entries, NULL);
		if (!g_option_context_parse(context, &argc, &argv, &error)) {
			g_print("option parsing failed: %s\n", error->message);
			return -1;
		}
	}

	if (!apikey) {
		g_warning("No API key provided.");
		return -1;
	}

	loop = g_main_loop_new(NULL, TRUE);
	g_idle_add(test1, NULL);
	g_main_loop_run(loop);

        return 0;
}
