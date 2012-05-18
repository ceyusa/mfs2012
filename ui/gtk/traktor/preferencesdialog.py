
from gi.repository import Gtk, GObject

class PreferencesDialog(Gtk.Dialog):
    def __init__(self,parent):

        Gtk.Dialog.__init__(self, "Preferences Dialog", parent, Gtk.DialogFlags.DESTROY_WITH_PARENT,
                            (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
                             Gtk.STOCK_OK, Gtk.ResponseType.OK))


        self.set_default_size(200, 150)


        box = self.get_content_area()
        label = Gtk.Label(label="Key:", halign=Gtk.Align.START)
        box.add(label)

        self.entry = Gtk.Entry()
        self.entry.set_text("Key")
        box.add(self.entry)

        self.show_all()

