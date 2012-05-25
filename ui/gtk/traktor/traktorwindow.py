from gi.repository import Gtk, Pango, WebKit
import constants
from preferencesdialog import PreferencesDialog
from feed_server import FeedServer
import os

MENU_UI = '''
<ui>
    <menubar name="MenuBar">
        <menu action="TraktorMenu">
            <menuitem action="Preferences" />
            <menuitem action="About" />
            <separator />
            <menuitem action="Quit" />
        </menu>
    </menubar>
</ui>
'''

class TraktorWindow(Gtk.Window):

    def __init__(self):
        Gtk.Window.__init__(self)
        self.set_title('Traktor')
        self.set_size_request(800, 600)

		# Top container (for the menu, combo and search)
        top_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=5)
        self.add(top_box)
        self.ui_manager = self._setup_ui_manager()
        menu_bar = self.ui_manager.get_widget('/MenuBar')
        top_box.pack_start(menu_bar, False, False, 0)
        
        # Combo and search text entry
        search_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL,spacing=20)
        self.program_combo = self._get_combo()
        search_box.pack_start(self.program_combo, False, False, 0)
        program_entry = self._get_entry()
        program_entry.set_size_request(300,10)
        search_box.pack_start(program_entry, False, False, 0)
        top_box.pack_start(search_box, False, False, 0)
        
        # Container for the rest
        content_box = Gtk.Box(True, orientation=Gtk.Orientation.HORIZONTAL, spacing=10)
        #content_box.set_homogeneus()
        top_box.pack_start(content_box, True, True, 0)
        
        # Left UI section (combo, search and results)
        left_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        #left_box.set_size_request(400,600)
        content_box.pack_start(left_box, True, True, 0)
                
        # Results (left)
        self.store = Gtk.ListStore(object, str, str)
        scrollview = Gtk.ScrolledWindow()
        view = Gtk.TreeView(self.store)
        scrollview.add(view)
        scrollview.set_property("min-content-height", 255)

        renderer = Gtk.CellRendererText()
        renderer.set_property("ellipsize", Pango.EllipsizeMode.END)

        column = Gtk.TreeViewColumn('Title', renderer, text=1)
        column.set_property("min-width", 100)
        column.set_property("resizable", True)
        view.append_column(column)

        column = Gtk.TreeViewColumn('Description', renderer, text=2)
        column.set_property("min-width", 250)
        column.set_property("resizable", True)
        view.append_column(column)

        view.connect('row-activated', self._on_row_activated)
        left_box.pack_start(scrollview, True, True, 5)

		# Webkit window (right)
        right_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        self.webkit_view = WebKit.WebView()
        program = {'imageSrc': 'http://upload.wikimedia.org/wikipedia/en/thumb/5/56/Real_Madrid_CF.svg/200px-Real_Madrid_CF.svg.png',
        'title': 'Looking for the 10th', 'rate': '10/10',
        'description': 'This film tells the story of the best team on earth'}
        self._set_webkit(program)
        right_box.pack_start(self.webkit_view, True, True, 0)
        content_box.pack_start(right_box, True, True, 0);

		

        self.connect('delete-event', self._quit)


    def read_html(self, url):
        file = open(url)
        htmlFile = ""
        while 1:
                line = file.readline()
                if not line:
                        break
                else:
                        htmlFile += line

        return htmlFile

    def _setup_ui_manager(self):
        ui_manager = Gtk.UIManager()
        ui_manager.add_ui_from_string(MENU_UI)
        accel_group = ui_manager.get_accel_group()
        self.add_accel_group(accel_group)
        action_group = Gtk.ActionGroup('Actions')
        action_group.add_actions([
                ('TraktorMenu', None, '_Traktor', None, None, None),
                ('Preferences', Gtk.STOCK_PREFERENCES,
                 '_Preferences', None, 'Key Management',
                 self._on_preferences_action),
                ('About', Gtk.STOCK_ABOUT,
                 '_About', None, 'About this application',
                 self._on_about_action),
                ('Quit', Gtk.STOCK_QUIT,
                 '_Quit', None, 'Quit application',
                 self._on_quit),
                ])
        ui_manager.insert_action_group(action_group)
        return ui_manager

    def _on_about_action(self, action):
        about = Gtk.AboutDialog()
        about.set_program_name("Traktor")
        about.set_version("0.1")
        about.set_copyright( "Master Free Software 2011/2012" )
        about.set_license_type(Gtk.License.GPL_3_0)
        about.set_comments("A client for the Trakt.tv service")
        about.set_website("https://github.com/ceyusa/mfs2012")
        about.set_authors(["Master Free Software 2011/2012",None])
        about.run()
        about.destroy()

    def _on_quit(self, data):
        Gtk.main_quit()

    def _on_row_activated(self, tree_view, path, column):
        item = tree_view.get_model().get_iter(path)
        program_dict = tree_view.get_model().get_value(item, 0)
        rate = repr(program_dict["ratings"]["percentage"]/10.)
        rate += '/10'

        if program_dict['images'].has_key("poster"):
            image_type = 'poster'
        else:
            image_type = 'screen'

        program = {'imageSrc': program_dict['images'][image_type],
        'title': program_dict['title'],
        'rate': rate,
        'description':  program_dict['overview']}
        self._set_webkit(program)

    def _on_preferences_action(self, action):
        preferences = PreferencesDialog(self)
        fs = FeedServer();

        preferences.entry.set_text(fs.apikey_get())
        response=preferences.run()
        if response == Gtk.ResponseType.OK:
            entry_value = preferences.entry.get_text()
            result_set = fs.apikey_set(entry_value)
            preferences.destroy()

        elif response == Gtk.ResponseType.CANCEL:
            preferences.destroy()

    def _quit(self, window, event):
        Gtk.main_quit()

    def run(self):
        self.show_all()
        Gtk.main()

    def _get_combo(self):
        tv_programs = [
        (repr(constants.SEARCH_MOVIES_TYPE),'Movies'),
        (repr(constants.SEARCH_SHOWS_TYPE), 'Shows'),
        (repr(constants.SEARCH_EPISODES_TYPE), 'Episodes')]
        program_combo = Gtk.ComboBoxText()
        for tv_program in tv_programs:
            program_combo.append(tv_program[0], tv_program[1])
        program_combo.set_active_id(repr(constants.SEARCH_MOVIES_TYPE))
        return program_combo

    def _get_entry(self):
        search_entry = Gtk.Entry()
        search_entry.set_icon_from_stock(Gtk.EntryIconPosition.SECONDARY,
        Gtk.STOCK_FIND)
        search_entry.set_placeholder_text("Search...")
        search_entry.connect("icon-press", self._on_search_icon_pressed)
        search_entry.connect("activate", self._on_search_enter_pressed)
        return search_entry

    def _set_webkit(self, program):
        htmlFile = self.read_html(os.path.abspath("./page.html"))
        data = {
                'imageSrc': program['imageSrc'],
                'title': program['title'],
                'rate': program['rate'],
                'description': program['description'],
        }
        self.webkit_view.load_string(htmlFile%data, "text/html", "utf-8", "")

    def _on_search_icon_pressed(self, program_entry, icon_pos, event):
        self._on_send_query(program_entry.get_text(),
                            int(self.program_combo.get_active_id()))

    def _on_search_enter_pressed(self, program_entry):
        self._on_send_query(program_entry.get_text(),
                            int(self.program_combo.get_active_id()))

    def _on_send_query(self, search_text, search_type):
        feed_server = FeedServer()
        feed_server.search(search_text, search_type, self._on_query_response)

    def _on_query_response(self, results):
        self.store.clear()
        for res in results:
            if not res.has_key("title"):
                res = res["episode"]
            self.store.append([res, res['title'], res['overview']])
