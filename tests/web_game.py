"""Browser acceptance against a local or published build, using its normal controls.

Run with python tests/web_game.py [URL]. Set CHROMIUM to a system browser path
or install Playwright's managed chromium. No test endpoint is built into the game.
"""

import argparse
import os
import subprocess
import sys
from pathlib import Path
from playwright.sync_api import sync_playwright, expect
from browser_replay import browser_clear


def visit(context, url, when, errors):
    context.add_init_script(f"Date.now = () => {when} * 1000")
    page = context.new_page()
    page.on("pageerror", lambda error: errors.append(str(error)))
    page.goto(url)
    expect(page.locator("#enter")).to_be_enabled(timeout=30000)
    return page


def open_menu(page):
    page.evaluate("document.exitPointerLock?.()")
    page.locator("#menu").click()
    expect(page.locator("#panel")).to_be_visible()


def home(page):
    open_menu(page)
    page.locator("#leave").click()
    expect(page.locator("#welcome")).to_be_visible()


def desktop(browser, url, errors):
    context = browser.new_context(viewport={"width": 960, "height": 600})
    page = visit(context, url, 1796079599, errors)
    page.wait_for_function("document.querySelector('#canvas').width === 320")
    assert page.evaluate("Math.abs(canvas.width/canvas.height-canvas.clientWidth/canvas.clientHeight)<.02")
    page.locator("#book").click()
    expect(page.locator(".day")).to_have_count(24)
    expect(page.locator(".day.ready")).to_have_count(0)
    for day in range(1, 25):
        page.locator(f'[data-day="{day}"]').click()
        expect(page.locator("#panelbody li")).to_have_count(3)
        expect(page.locator("#panelbody h2")).not_to_be_empty()
        page.locator("#backbook").click()
    page.locator('[data-day="9"]').click()
    page.locator("#atlas").click()
    expect(page.locator("#atlasReference")).to_contain_text("Tribes")
    page.locator("#atlasSector").select_option("4")
    expect(page.locator("#atlasBrief")).to_contain_text("home bowl")
    page.locator("#atlasOpen").check()
    page.locator("#atlasDoor").select_option("24")
    expect(page.locator("#atlasFeature")).to_contain_text("Chrono")
    expect(page.locator("#atlasField")).to_contain_text("Sorting die")
    expect(page.locator("#atlasBeats li")).to_have_count(2)
    page.locator("#atlasBack").click()
    page.locator("#close").click()
    page.locator("#featured").click()
    expect(page.locator(".viewinfo")).to_contain_text("Tram terminus")
    colours = page.evaluate("""() => new Promise(resolve => requestAnimationFrame(() => {
        const gl=canvas.getContext('webgl') || canvas.getContext('experimental-webgl');
        const w=gl.drawingBufferWidth,h=gl.drawingBufferHeight;
        const pixels=new Uint8Array(w*h*4), palette=new Set();
        gl.readPixels(0,0,w,h,gl.RGBA,gl.UNSIGNED_BYTE,pixels);
        for(let y=0;y<h;y+=3) for(let x=0;x<w;x+=3) {
          const i=4*(y*w+x);
          palette.add((pixels[i]>>5)*64+(pixels[i+1]>>5)*8+(pixels[i+2]>>5));
        }
        resolve(palette.size);
    }))""")
    assert colours >= 10, f"Cinema arrival is obscured ({colours} world colours)"
    page.evaluate("document.exitPointerLock?.()")
    page.locator("#chart").click()
    expect(page.locator("#chartmap")).to_be_visible()
    page.locator("#backgame").click()
    home(page)
    page.locator("#enter").click()
    expect(page.locator("#location")).to_contain_text("0/24 open")
    page.evaluate("Date.now = () => 1796079600 * 1000")
    expect(page.locator("#location")).to_contain_text("1/24 open", timeout=7000)
    page.evaluate("Date.now = () => (1796079600 + 23 * 86400) * 1000")
    expect(page.locator("#location")).to_contain_text("24/24 open", timeout=7000)
    home(page)
    page.locator("#together").click()
    page.locator("#splits").click()
    expect(page.locator(".view")).to_have_count(2)
    expect(page.locator("#location")).to_contain_text("rehearsal")
    expect(page.locator(".viewinfo").nth(1)).to_contain_text("PLAYER 2")
    context.close()


def touch(browser, url, errors):
    context = browser.new_context(
        viewport={"width": 390, "height": 740}, has_touch=True, is_mobile=True
    )
    page = visit(context, url, 1796166000, errors)
    page.wait_for_function("document.querySelector('#canvas').height === 320")
    assert page.evaluate("canvas.width<320 && Math.abs(canvas.width/canvas.height-canvas.clientWidth/canvas.clientHeight)<.02")
    page.locator("#practice").tap()
    expect(page.locator("#touch")).to_be_visible()
    expect(page.locator(".viewinfo")).to_contain_text("tap Pulse")
    page.locator('[data-bit="2"]').tap()
    page.locator("#menu").tap()
    expect(page.locator("#panelbody")).to_contain_text("Controls")
    context.close()


def timed_trial(browser, url, errors):
    context = browser.new_context(viewport={"width": 860, "height": 600})
    page = visit(context, url.rstrip("/") + "/?playtest=3", 1796079599, errors)
    expect(page.locator("#trialday")).to_have_value("3")
    page.evaluate("localStorage.setItem('lantern-hall.sector.v1.3.1','2')")
    page.locator("#trialday").select_option("3")
    page.locator("#trialmode").select_option("Split screen")
    page.locator("#firstvisit").check()
    page.locator("#begintrial").click()
    expect(page.locator(".view")).to_have_count(2)
    expect(page.locator(".viewinfo").first).to_contain_text("1/5 · Survey camp")
    home(page)
    page.locator("#book").click()
    page.locator("#fieldnotes").click()
    expect(page.locator("#panelbody summary").first).to_contain_text(
        "Split screen · Stopped"
    )
    page.locator("#panelbody summary").first.click()
    page.locator("[data-device]").fill("Two game seats")
    page.locator("[data-save]").click()
    expect(page.locator("#logstatus")).to_contain_text("saved")
    with page.expect_download() as download:
        page.locator("#exportlog").click()
    assert download.value.suggested_filename == "lantern-hall-playtests.json"
    assert page.evaluate("localStorage.getItem('lantern-hall.sector.v1.3.1')") == "2"
    context.close()


def simulated_xr(browser, url, errors):
    context = browser.new_context(viewport={"width": 800, "height": 480})
    # Supply only the device boundary. The page runs its production XR code,
    # Wasm core and real GL renderer for both eyes.
    context.add_init_script("""(() => {
      const identity=new Float32Array([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]);
      window.eyePasses=new Set();window.guideDraws=0;
      const draw=WebGLRenderingContext.prototype.drawArrays;
      WebGLRenderingContext.prototype.drawArrays=function(mode,...args){
        if(mode===this.TRIANGLE_STRIP)window.guideDraws++;return draw.call(this,mode,...args);
      };
      WebGLRenderingContext.prototype.makeXRCompatible=async()=>{};
      window.XRWebGLLayer=class {
        constructor(session,gl,options){if(options.framebufferScaleFactor!==.7)throw Error('XR scale');this.framebuffer=null;}
        getViewport(v){window.eyePasses.add(v.eye);return {x:v.eye==='left'?0:200,y:0,width:200,height:200};}
      };
      class Session extends EventTarget {
        inputSources=[{handedness:'right',gamepad:{axes:[0,0,0,0],buttons:Array.from({length:6},()=>({pressed:false}))}}]; ended=false;
        updateRenderState(){}
        async requestReferenceSpace(kind){if(kind==="local-floor")throw Error("Seat-only device");return {};}
        requestAnimationFrame(fn){return requestAnimationFrame(t=>{
          if(this.ended)return;
          const views=['left','right'].map(eye=>({eye,projectionMatrix:identity,transform:{inverse:{matrix:identity}}}));
          fn(t,{getViewerPose:()=>{if(this.lost){this.lostFrames=(this.lostFrames||0)+1;return null;}this.trackedFrames=(this.trackedFrames||0)+1;return {views,transform:{matrix:identity,orientation:{x:0,y:0,z:0,w:1}}};}});
        });}
        async end(){this.ended=true;this.dispatchEvent(new Event('end'));}
      }
      Object.defineProperty(navigator,'xr',{value:{isSessionSupported:async()=>true,
        requestSession:async()=>window.testHeadset=new Session()}});
    })()""")
    page = visit(context, url, 1796079600, errors)
    page.locator("#book").click()
    page.locator("#fieldnotes").click()
    page.locator("#trialmode").select_option("WebXR")
    page.locator("#begintrial").click()
    page.wait_for_function("window.eyePasses.size === 2 && window.guideDraws>=4")
    page.evaluate("testHeadset.inputSources[0].gamepad.buttons[5].pressed=true")
    page.wait_for_timeout(250)
    before = page.evaluate("window.guideDraws")
    page.wait_for_timeout(250)
    assert page.evaluate("window.guideDraws") == before
    page.evaluate("testHeadset.inputSources[0].gamepad.buttons[5].pressed=false")
    page.evaluate("testHeadset.lost=true")
    page.wait_for_function("testHeadset.lostFrames>=2")
    page.evaluate(
        "testHeadset.trackedFrames=0;testHeadset.lost=false;testHeadset.inputSources[0].gamepad.axes[2]=1;testHeadset.inputSources[0].gamepad.buttons[0].pressed=true"
    )
    page.wait_for_function("testHeadset.trackedFrames>=3")
    page.evaluate("window.testHeadset.end()")
    expect(page.locator("#hud")).to_be_visible()
    rows = page.evaluate(
        "Object.keys(localStorage).filter(k=>k.startsWith('lantern-hall.field-notes')).map(k=>JSON.parse(localStorage[k]))"
    )
    assert (
        len(rows) == 1
        and rows[0]["mode"] == "WebXR"
        and rows[0]["status"] == "Stopped"
        and rows[0]["xrFrames"] > 0
    )
    assert (
        rows[0]["xrSpace"] == "local"
        and rows[0]["xrEyes"] == 2
        and rows[0]["trackingLosses"] >= 1
    )
    assert rows[0]["xrActions"] & 2 and rows[0]["xrActions"] & 16
    context.close()


def connect_screen(page):
    page.locator("#together").click()
    page.locator("#net").click()


def peers(browser, url, errors, relay_url=None):
    first = browser.new_context(viewport={"width": 760, "height": 480})
    second = browser.new_context(viewport={"width": 760, "height": 480})
    second.add_init_script("""const parse=JSON.parse;JSON.parse=(...args)=>{
        const value=parse(...args);
        if(Array.isArray(value)&&value.length===39)window.observedFrame=value;
        return value;
    };""")
    host = visit(first, url, 1796079600, errors)
    guest = visit(second, url, 1796079599, errors)
    connect_screen(host)
    connect_screen(guest)
    if relay_url:
        for page in [host, guest]:
            page.locator("#relay-details summary").click()
            page.locator("#relay-url").fill(relay_url)
        host.locator("#relay-create").click()
        host.wait_for_function(
            "document.getElementById('relay-code').value.length === 8"
        )
        guest.locator("#relay-code").fill(host.locator("#relay-code").input_value())
        guest.locator("#relay-join").click()
    else:
        host.locator("#host").click()
        host.wait_for_function(
            "document.getElementById('codeout').value.length > 0", timeout=18000
        )
        guest.locator("#codein").fill(host.locator("#codeout").input_value())
        guest.locator("#join").click()
        guest.wait_for_function(
            "document.getElementById('codeout').value.length > 0", timeout=18000
        )
        host.locator("#codein").fill(guest.locator("#codeout").input_value())
        host.locator("#accept").click()
    for page in [host, guest]:
        expect(page.locator("#location")).to_contain_text("rehearsal", timeout=18000)
        expect(page.locator(".view")).to_have_count(1)
    open_menu(guest)
    guest.locator("#browse").click()
    expect(guest.locator(".day.ready")).to_have_count(1)
    guest.locator("#close").click()
    # Use a snapshot actually received by the guest to stop on the usher sightline.
    # The only commands sent to the game are ordinary keyboard events.
    guest.bring_to_front()
    guest.keyboard.down("a")
    try:
        guest.wait_for_function("window.observedFrame?.[16] < -1.7", timeout=18000)
    finally:
        guest.keyboard.up("a")
    guest.wait_for_timeout(160)
    for _ in range(7):
        guest.keyboard.down("f")
        guest.wait_for_timeout(170)
        guest.keyboard.up("f")
        guest.wait_for_timeout(170)
        if "1/2" in host.locator(".viewinfo").inner_text():
            break
    expect(host.locator(".viewinfo")).to_contain_text("1/2", timeout=7000)
    expect(guest.locator(".viewinfo")).to_contain_text("1/2", timeout=7000)
    first.close()
    second.close()


def native_host(browser, url, errors, relay_url, library):
    peer_script = Path(__file__).resolve().parents[1] / "scripts/native_peer.py"
    env = dict(os.environ, SDL_VIDEODRIVER="offscreen")
    peer = subprocess.Popen(
        [
            sys.executable,
            str(peer_script),
            relay_url,
            "--library",
            str(library),
            "--door",
            "3",
        ],
        stdout=subprocess.PIPE,
        text=True,
        env=env,
    )
    context = browser.new_context(viewport={"width": 760, "height": 480})
    try:
        code = peer.stdout.readline().strip().split()[-1]
        guest = visit(context, url, 1796079599, errors)
        connect_screen(guest)
        guest.locator("#relay-details summary").click()
        guest.locator("#relay-url").fill(relay_url)
        guest.locator("#relay-code").fill(code)
        guest.locator("#relay-join").click()
        expect(guest.locator("#location")).to_contain_text(
            "Door 03 · playtest", timeout=18000
        )
        expect(guest.locator(".viewinfo")).to_contain_text("Survey camp")
        guest.keyboard.down("ArrowRight")
        guest.wait_for_timeout(500)
        guest.keyboard.up("ArrowRight")
        guest.evaluate("document.exitPointerLock?.()")
        guest.locator("#chart").click()
        expect(guest.locator("#chartmap")).to_be_visible()
    finally:
        context.close()
        peer.terminate()
        peer.wait(timeout=6)


if __name__ == "__main__":
    args = argparse.ArgumentParser()
    args.add_argument("url", nargs="?", default="http://127.0.0.1:8571/")
    args.add_argument(
        "--direct",
        action="store_true",
        help="also verify direct WebRTC on a network allowing ICE candidates",
    )
    args.add_argument(
        "--native-runtime",
        type=Path,
        help="also connect Chromium to this native SDL3 library",
    )
    options = args.parse_args()
    url = options.url
    errors = []
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(
            executable_path=os.environ.get("CHROMIUM"),
            args=[
                "--enable-unsafe-swiftshader",
                "--use-angle=swiftshader",
                "--disable-dev-shm-usage",
                "--disable-background-timer-throttling",
                "--disable-renderer-backgrounding",
                "--disable-backgrounding-occluded-windows",
            ],
        )
        desktop(browser, url, errors)
        touch(browser, url, errors)
        timed_trial(browser, url, errors)
        simulated_xr(browser, url, errors)
        browser_clear(browser, url, rehearsal=True)
        browser_clear(browser, url, rehearsal=False)
        relay_script = Path(__file__).resolve().parents[1] / "scripts" / "relay.py"
        relay = subprocess.Popen(
            [sys.executable, str(relay_script), "--port", "0"],
            stdout=subprocess.PIPE,
            text=True,
        )
        try:
            port = int(relay.stdout.readline().strip().split()[-1])
            peers(browser, url, errors, f"ws://127.0.0.1:{port}")
            if options.native_runtime:
                native_host(
                    browser,
                    url,
                    errors,
                    f"ws://127.0.0.1:{port}",
                    options.native_runtime,
                )
        finally:
            relay.terminate()
            relay.wait(timeout=5)
        if options.direct:
            peers(browser, url, errors)
        browser.close()
    assert not errors, errors
    print(
        "Browser acceptance passed: 24 briefings, midnight rollover, split views, mobile touch, simulated stereo XR, tutorial extraction and live campaign entry, and two networked browsers."
    )
