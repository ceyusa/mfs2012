#include <stdio.h>
#include <glib.h>

#include "gt-sample.h"

static gchar *message = "grupo";

static GOptionEntry entries[] = {
        { "greeter", 'g', 0, G_OPTION_ARG_STRING, &message, "greeter", "g" },
        { NULL }
};

int
main (int argc, char **argv)
{
        GtSample *sample;
        GError *error = NULL;
        GOptionContext *context;

        g_type_init ();

        context = g_option_context_new (" - Displays a greeting message");
        g_option_context_add_main_entries (context, entries, NULL);
        if (!g_option_context_parse (context, &argc, &argv, &error)) {
                g_print ("option parsing failed: %s\n", error->message);
                return -1;
        }

        sample = gt_sample_new ();
        g_object_set (sample, "name", message, NULL);
        gt_sample_say_hello (sample);
        g_object_unref (sample);

        return 0;
}
