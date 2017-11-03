libconn - libevent2 tcp/udp connection management
-------------------------------------------------

I like bufferevents, but they're ... well, they're not completely complete.
Specifically:

* they don't handle DNS lookups;
* they don't handle multiple connection attempts;
* they don't handle UDP;
* it isn't zero-copy socket IO ready;
* it isn't /entirely/ scatter/gather DMA IO ready.

Now, yes this code doesn't do all of the above just yet, but the eventual
goal is to make the API support all of the above in a very useful, easy
to use way.

It leverages using libbuf, which is a simple linked list buffer representation
which will eventually grow zero-copy pieces.

The basics:

* eb.h is a tuple of libevent_base and libevdns_base.  Since this code
  is intended to run in multiple worker threads, you need to know what
  your libevent loop and DNS base are.  This is passed into every
  connection.

* create a connection using conn_create()
* optionally set local socket address using conn_set_lcl()
* if you know what you're connecting to, call conn_set_peer()
* else, call conn_set_peer_host() with a hostname and port.
  Yes, this should take a URL later (eg for service lookups)
  and an address family, interface name hint, etc.
* call conn_connect() to start the connection process.

Callbacks are called when receiving data and when a buffer has
been sent.  The caller can then free the buffer.  Later the
receive path will optionally be supplied buffers to read into
(eg if you're doing zero-copy reads into a shared memory
segment or some other underlying buffer representation)
to potentially avoid having to memcpy() it into its final
place.

