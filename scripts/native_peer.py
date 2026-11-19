"""Play the native SDL3 client in the same two-seat room as a browser or native friend."""

import argparse
import asyncio
import ctypes as c
import json
import math
import os
from pathlib import Path
import sys
import time

from websockets.asyncio.client import connect
from websockets.exceptions import ConnectionClosed

WORDS = 32
FloatPtr = c.POINTER(c.c_float)


def runtime(path):
    if os.name == "nt":
        # Keep SDL3.dll discoverable beside the game library.
        global dll_directory
        dll_directory = os.add_dll_directory(str(path.parent.resolve()))
    lib = c.CDLL(str(path.resolve()))
    for name, (result, arguments) in {
        "init": (c.c_int, []),
        "close": (None, []),
        "capture": (None, []),
        "today": (c.c_int, [c.c_double]),
        "open": (None, [c.c_int]),
        "saved": (c.c_int, []),
        "save_ticket": (None, [c.c_int]),
        "start": (None, [c.c_int] * 4),
        "preview": (c.c_int, [c.c_int]),
        "native_input": (c.c_int, [FloatPtr]),
        "native_title": (None, [c.c_int]),
        "input": (None, [c.c_int] + [c.c_float] * 4 + [c.c_int]),
        "aim": (None, [c.c_int, c.c_float, c.c_float]),
        "state": (FloatPtr, []),
        "wire": (FloatPtr, []),
        "receive": (c.c_int, []),
        "tick": (c.c_int, [c.c_float, c.c_int]),
        "draw": (None, [c.c_int] * 3),
        "event": (c.c_int, []),
    }.items():
        fn = getattr(lib, "lh_" + name)
        fn.restype, fn.argtypes = result, arguments
    return lib


def numeric_frame(msg, count):
    return (
        isinstance(msg, list)
        and len(msg) == count
        and all(type(v) in (int, float) and math.isfinite(v) for v in msg)
    )


def guest_input(lib, seat, frame):
    if not numeric_frame(frame, 7) or any(abs(v) > 1 for v in frame[:4]):
        return False
    if frame[4] not in range(8):
        return False
    lib.lh_input(seat, *frame[:4], int(frame[4]))
    lib.lh_aim(seat, *(max(-1, min(1, v)) for v in frame[5:]))
    return True


async def play(args):
    host = not args.join
    seat = 0 if host else 1
    lib = runtime(args.library)
    async with connect(args.relay, max_size=4096, compression=None) as ws:
        await ws.send(
            json.dumps({"create": True} if host else {"join": args.join.upper()})
        )
        while True:
            hello = json.loads(await ws.recv())
            if "error" in hello:
                raise RuntimeError(hello["error"])
            if "room" in hello:
                print("Room", hello["room"], flush=True)
            if hello.get("ready") == ("host" if host else "guest"):
                break
        if not lib.lh_init():
            raise RuntimeError("SDL3 could not open a window")
        try:
            lib.lh_start(2, lib.lh_today(time.time()), 0, lib.lh_saved())
            if host and args.door:
                lib.lh_preview(args.door)
            lib.lh_capture()
            print("Connected as", "host" if host else "guest", flush=True)
            version = lib.lh_state()[0]
            keys = (c.c_float * 7)()
            remote = [0.0] * 7
            last = last_packet = last_remote = time.monotonic()
            queue = asyncio.Queue(maxsize=64)

            async def read():
                try:
                    async for msg in ws:
                        await queue.put(json.loads(msg))
                finally:
                    await queue.put(None)

            reader = asyncio.create_task(read())
            remote_presses = 0
            try:
                while True:
                    now = time.monotonic()
                    if not lib.lh_native_input(keys):
                        break
                    while not queue.empty():
                        frame = queue.get_nowait()
                        if frame is None:
                            return
                        if host and guest_input(lib, 1, frame):
                            remote_presses |= int(frame[4]) & ~int(remote[4])
                            remote = frame
                            last_remote = now
                        elif not host and numeric_frame(frame, WORDS):
                            if frame[0] != version:
                                raise RuntimeError(
                                    "Game versions differ. Update both clients and reconnect."
                                )
                            wire = lib.lh_wire()
                            for i, v in enumerate(frame):
                                wire[i] = v
                            if lib.lh_receive() and lib.lh_event() == 3:
                                lib.lh_save_ticket(1 << (int(frame[8]) - 1))
                    if host:
                        guest_input(lib, 0, list(keys))
                        if now - last_remote > 0.4:
                            remote = [0.0] * 7
                        lib.lh_input(1, *remote[:4], int(remote[4]) | remote_presses)
                        lib.lh_open(lib.lh_today(time.time()))
                        steps = lib.lh_tick(now - last, 1)
                        state = list(lib.lh_state()[:WORDS])
                        if now - last_packet >= 0.05 or state[7]:
                            await ws.send(json.dumps(state))
                            last_packet = now
                        if lib.lh_event() == 3:
                            lib.lh_save_ticket(int(state[6]))
                        if steps:
                            remote_presses = 0
                    else:
                        await ws.send(json.dumps(list(keys)))
                        lib.lh_tick(now - last, 0)
                    lib.lh_native_title(seat)
                    lib.lh_draw(0, 0, seat)
                    last = now
                    await asyncio.sleep(max(0.001, 1 / 60 - (time.monotonic() - now)))
            finally:
                reader.cancel()
                await asyncio.gather(reader, return_exceptions=True)
        finally:
            lib.lh_close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("relay", help="ws://host:port or wss://host")
    parser.add_argument(
        "--join", metavar="CODE", help="join a friend's eight-character room"
    )
    parser.add_argument(
        "--door",
        type=int,
        choices=range(1, 25),
        metavar="N",
        help="host a campaign playtest",
    )
    parser.add_argument(
        "--library",
        type=Path,
        default=Path(__file__).with_name(
            "lantern_native.dll" if os.name == "nt" else "lantern_native.so"
        ),
    )
    try:
        asyncio.run(play(parser.parse_args()))
    except (ConnectionClosed, RuntimeError, OSError) as exc:
        sys.exit(str(exc))
