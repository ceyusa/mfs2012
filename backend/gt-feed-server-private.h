/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Contact: mfs-dm-2011@igalia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef _GT_FEED_SERVER_PRIVATE
#define _GT_FEED_SERVER_PRIVATE

G_BEGIN_DECLS

/* Introspection data for the service we are exporting */
static const gchar interface_xml[] =
  "<node>"
  "  <interface name='org.mfs.Gtrakt.FeedServer'>"
  "    <method name='Query'>"
  "      <arg type='s' name='apikey' direction='in'/>"
  "      <arg type='s' name='query' direction='in'/>"
  "      <arg type='i' name='type' direction='in'/>"
  "      <arg type='av' name='response' direction='out'/>"
  "    </method>"
  "  </interface>"
  "</node>";

G_END_DECLS

#endif /* _GT_FEED_SERVER_PRIVATE */
