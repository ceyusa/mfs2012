" Encapsulates D-Bus calls. "
import gobject

import dbus
import dbus.mainloop.glib

from constants import *


BUS_NAME = 'org.mfs.Gtrakt.FeedServer'
OBJECT_PATH = '/org/mfs/Gtrakt/FeedServer'

class FeedServer:
    """ Proxy to query D-Bus service.  """
    def __init__(self):
        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
        session_bus = dbus.SessionBus()
        try:
            proxy = session_bus.get_object(BUS_NAME, OBJECT_PATH)
        except dbus.exceptions.DBusException:
            print("ERROR: Not found service")
            raise
        self.iface = dbus.Interface(proxy, dbus_interface=BUS_NAME)
        self.props = dbus.Interface(proxy, 'org.freedesktop.DBus.Properties')
        self.last_result = None
        self.error = None
        self.loop = None

    def apikey_get(self):
        try:
            apikey = self.props.Get(BUS_NAME, 'ApiKey')
        except dbus.exceptions.DBusException:
            print("ERROR: Unable to connect")
            raise
        return apikey

    def apikey_set(self, apikey):
        try:
            self.props.Set(BUS_NAME, 'ApiKey', apikey)
        except dbus.exceptions.DBusException:
            print("ERROR: Unable to connect")
            raise

    def search(self, query, query_type=SEARCH_MOVIES_TYPE, callback=None):
        self._callback = callback
        try:
            feed = self.iface.Query('+'.join(query.split()), query_type,
                                    reply_handler=self.handle_result,
                                    error_handler=self.handle_error)
        except dbus.exceptions.DBusException:
            print("ERROR: Unable to connect")
            raise

    def run(self):
        self.loop = gobject.MainLoop()
        self.loop.run()

    def stop(self):
        if self.loop:
            self.loop.quit()

    def handle_result(self, res):
        self.last_result = res
        self.on_reply()
        self.stop()

    def handle_error(self, err):
        self.error = err
        self.on_reply()
        self.stop()

    def _set_last_result(self, res):
        self._res = res

    def _get_last_result(self):
        return self._res
    last_result = property(_get_last_result, _set_last_result)

    def _set_error(self, err):
        self._err = err

    def _get_error(self):
        return _err
    error = property(_get_error, _set_error)

    def on_reply(self):
        if self._callback != None:
            self._callback(self.last_result)

def test_feed_change_apikey():
    feed_server = FeedServer()
    old_apikey = feed_server.apikey_get();
    new_apikey = "1234"
    feed_server.apikey_set(new_apikey)
    assert new_apikey == feed_server.apikey_get()
    feed_server.apikey_set(old_apikey)
    old_apikey_reset = feed_server.apikey_get();
    assert old_apikey == old_apikey_reset

def test_feed_server_movie():
    feed_server = FeedServer()
    feed_server.search('batman')
    feed_server.run()
    result = feed_server.last_result
    assert hasattr(result, "__getitem__")
    assert hasattr(result, "__iter__")


def test_feed_server_no_results():
    feed_server = FeedServer()
    feed_server.search('3172708766b5655033d176fedbd014dab9137d6f')
    feed_server.run()
    result = feed_server.last_result
    assert len(result) == 0

def test_async():
    def the_callback(result):
        assert hasattr(result, "__getitem__")
        assert hasattr(result, "__iter__")
    feed_server = FeedServer()
    feed_server.search('batman', SEARCH_MOVIES_TYPE, the_callback)
    feed_server.run()

if __name__ == '__main__':
    test_feed_server_movie()
