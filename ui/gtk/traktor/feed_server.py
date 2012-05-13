" Encapsulates D-Bus calls. "
import dbus

from constants import *


BUS_NAME = 'org.mfs.Gtrakt.FeedServer'
OBJECT_PATH = '/org/mfs/Gtrakt/FeedServer'


class FeedServer:
    """ Proxy to query D-Bus service.  """
    def __init__(self):
        session_bus = dbus.SessionBus()
        try:
            proxy = session_bus.get_object(BUS_NAME, OBJECT_PATH)
        except dbus.exceptions.DBusException:
            print("ERROR: Not found service")
            raise
        self.iface = dbus.Interface(proxy, dbus_interface=BUS_NAME)

    def search(self, query, query_type=SEARCH_MOVIES_TYPE):
        try:
            feed = self.iface.Query(query, query_type)
        except dbus.exceptions.DBusException:
            print("ERROR: Unable to connect")
            raise
        return feed


def test_feed_server_movie():
    feed_server = FeedServer()
    result = feed_server.search('batman')
    assert hasattr(result, "__getitem__")
    assert hasattr(result, "__iter__")


def test_feed_server_no_results():
    feed_server = FeedServer()
    result = feed_server.search('3172708766b5655033d176fedbd014dab9137d6f')
    assert len(result) == 0


if __name__ == '__main__':
    test_feed_server_movie()
