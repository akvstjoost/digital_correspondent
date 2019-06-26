#!/usr/bin/env python3

import sys
import socket
import selectors
import types

sel = selectors.DefaultSelector()
addresses = []
addressesToWrite = []
theData = b''

def accept_wrapper(sock):
    conn, addr = sock.accept()  # Should be ready to read
    print("accepted connection from", addr)
    conn.setblocking(False)
    data = types.SimpleNamespace(addr=addr, inb=b"", outb=b"")
    events = selectors.EVENT_READ | selectors.EVENT_WRITE
    sel.register(conn, events, data=data)


def service_connection(key, mask):
    sock = key.fileobj
    data = key.data
    addr = data.addr
    global theData
    global addresses
    global addressesToWrite
    if not addr in addresses:
        addresses.append(addr)
        print(addresses)
    if mask & selectors.EVENT_READ:
        recv_data = sock.recv(1024)  # Should be ready to read
        if recv_data:
            theData = recv_data
            addressesToWrite = addresses.copy()
            if theData[0:10] == b'whoisthere':
                theData = b''
                for s in addresses:
                    theData += bytes(s[0],'utf-8')
                    theData += b'\r\n'
        else:
            print("closing connection to", data.addr)
            sel.unregister(sock)
            addresses.remove(addr)
            sock.close()
    if mask & selectors.EVENT_WRITE:
        if addr in addressesToWrite:
            data.outb = theData
            addressesToWrite.remove(addr)
        if data.outb:
            print("echoing", repr(data.outb), "to", data.addr)
            sent = sock.send(data.outb)  # Should be ready to write
            data.outb = data.outb[sent:]


if len(sys.argv) != 3:
    print("usage:", sys.argv[0], "<host> <port>")
    sys.exit(1)

host, port = sys.argv[1], int(sys.argv[2])
lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
lsock.bind((host, port))
lsock.listen()
print("listening on", (host, port))
lsock.setblocking(False)
sel.register(lsock, selectors.EVENT_READ, data=None)

try:
    while True:
        events = sel.select(timeout=None)
        for key, mask in events:
            if key.data is None:
                accept_wrapper(key.fileobj)
            else:
                try:
                    service_connection(key, mask)
                except Exception:
                    print("error....")
except KeyboardInterrupt:
    print("caught keyboard interrupt, exiting")
finally:
    sel.close()
