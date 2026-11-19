"""Connect to the native SDL3 host and guest over their normal WebSocket relay."""

import argparse
import asyncio
import json
import os
from pathlib import Path
import sys
import tempfile

from websockets.asyncio.client import connect

ROOT = Path(__file__).resolve().parents[1]


async def launch(*args, **kw):
    return await asyncio.create_subprocess_exec(
        sys.executable, *map(str, args), stdout=asyncio.subprocess.PIPE, **kw
    )


async def stop(process):
    if process.returncode is None:
        process.terminate()
    await process.wait()


async def run(library, directory):
    env = dict(os.environ, SDL_VIDEODRIVER="offscreen", XDG_DATA_HOME=directory)
    service = await launch(ROOT / "scripts/relay.py", "--port", 0)
    peer = None
    try:
        port = int((await service.stdout.readline()).split()[-1])
        address = f"ws://127.0.0.1:{port}"
        args = (ROOT / "scripts/native_peer.py", address, "--library", library)
        peer = await launch(*args, "--door", 3, env=env)
        code = (await peer.stdout.readline()).decode().split()[-1]
        async with connect(address) as remote:
            await remote.send(json.dumps({"join": code}))
            assert json.loads(await remote.recv())["ready"] == "guest"
            start = json.loads(await remote.recv())
            assert (
                len(start) == 32 and start[1] == 2 and start[8] == 3 and start[3] == 1
            )
            for _ in range(40):
                await remote.send(json.dumps([0, 0, 1, 0, 0, 0, 0]))
                state = json.loads(await remote.recv())
                if abs(state[19]-start[19]) > 0.1:
                    break
            assert abs(state[19]-start[19]) > 0.1
        await peer.wait()
        assert peer.returncode == 0
        async with connect(address) as host:
            await host.send('{"create":true}')
            code = json.loads(await host.recv())["room"]
            peer = await launch(*args, "--join", code, env=env)
            assert json.loads(await host.recv())["ready"] == "host"
            state[3] = 0
            state[1] = 0
            state[6] = 4
            state[7] = 3
            for off in (10, 16):
                state[off : off + 6] = [0, 0, 20.5, 0, 0, 1]
            await host.send(json.dumps(state))
            for _ in range(80):
                assert len(json.loads(await host.recv())) == 7
                files = list(Path(directory).rglob("ticket-v1"))
                if files and files[0].read_text() == "4":
                    break
            assert files and files[0].read_text() == "4"
        await peer.wait()
        print(
            "Native relay host accepted guest controls. Native guest received and saved the shared extraction."
        )
    finally:
        if peer:
            await stop(peer)
        await stop(service)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("library", type=Path)
    with tempfile.TemporaryDirectory() as directory:
        asyncio.run(
            asyncio.wait_for(run(parser.parse_args().library.resolve(), directory), 40)
        )
