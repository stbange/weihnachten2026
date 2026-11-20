"""Optional two-seat WebSocket relay for Lantern Hall. Python 3.10+, websockets 14–16."""
import argparse
import asyncio
from dataclasses import dataclass
import json
import secrets
import ssl
from websockets.asyncio.server import serve
from websockets.exceptions import ConnectionClosed

@dataclass
class Room:
    host: object
    guest: object = None

rooms = {}

async def connection(ws):
    room = code = None
    try:
        hello = json.loads(await asyncio.wait_for(ws.recv(), timeout=15))
        if not isinstance(hello, dict):
            return
        if hello.get("create") is True:
            if len(rooms) >= 128:
                await ws.send('{"error":"The relay is full."}')
                return
            alphabet = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789"
            while True:
                code = "".join(secrets.choice(alphabet) for _ in range(8))
                if code not in rooms:
                    break
            room = rooms[code] = Room(ws)
            await ws.send(json.dumps({"room": code}))
        else:
            requested = hello.get("join")
            if not isinstance(requested, str) or len(requested) != 8:
                await ws.send('{"error":"Enter the eight-character room code."}')
                return
            code = requested.upper()
            candidate = rooms.get(code)
            if candidate is None or candidate.guest is not None:
                await ws.send('{"error":"That room is unavailable."}')
                return
            room = candidate
            room.guest = ws
            await ws.send('{"ready":"guest"}')
            await room.host.send('{"ready":"host"}')
        while True:
            # An invitation can wait five minutes; an abandoned game expires.
            message = await asyncio.wait_for(ws.recv(), timeout=300)
            peer = room.guest if room.host is ws else room.host
            if peer is None or not isinstance(message, str):
                continue
            frame = json.loads(message)
            count = 39 if room.host is ws else 7
            if isinstance(frame,list) and len(frame)==count and all(type(n) in (int,float) for n in frame):
                await peer.send(message)
    except (ConnectionClosed, asyncio.TimeoutError, ValueError):
        pass
    finally:
        if room is not None and rooms.get(code) is room:
            del rooms[code]
            peer = room.guest if room.host is ws else room.host
            if peer is not None:
                await peer.close()

async def main(args):
    tls=None
    if args.cert:
        tls=ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        tls.load_cert_chain(args.cert,args.key)
    async with serve(connection,args.bind,args.port,ssl=tls,max_size=4096,max_queue=8,compression=None) as service:
        port=service.sockets[0].getsockname()[1]
        print(f"Lantern Hall relay listening on {port}",flush=True)
        await asyncio.Future()

if __name__=="__main__":
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--bind",default="127.0.0.1")
    parser.add_argument("--port",type=int,default=8765)
    parser.add_argument("--cert")
    parser.add_argument("--key")
    args=parser.parse_args()
    if bool(args.cert)!=bool(args.key):parser.error("--cert and --key go together")
    asyncio.run(main(args))
