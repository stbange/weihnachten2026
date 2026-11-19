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
    context = browser.new_context(viewport={"width":960, "height":600})
    page = visit(context, url, 1796079599, errors)
    page.locator("#book").click()
    expect(page.locator(".day")).to_have_count(24)
    expect(page.locator(".day.ready")).to_have_count(0)
    for day in range(1, 25):
        page.locator(f'[data-day="{day}"]').click()
        expect(page.locator("#panelbody li")).to_have_count(3)
        expect(page.locator("#panelbody h2")).not_to_be_empty()
        page.locator("#backbook").click()
    page.locator("#close").click()
    page.locator("#enter").click()
    expect(page.locator("#location")).to_contain_text("0/24 open")
    page.evaluate("Date.now = () => 1796079600 * 1000")
    expect(page.locator("#location")).to_contain_text("1/24 open",timeout=7000)
    page.evaluate("Date.now = () => (1796079600 + 23 * 86400) * 1000")
    expect(page.locator("#location")).to_contain_text("24/24 open",timeout=7000)
    home(page)
    page.locator("#together").click()
    page.locator("#splits").click()
    expect(page.locator(".view")).to_have_count(2)
    expect(page.locator("#location")).to_contain_text("rehearsal")
    expect(page.locator(".viewinfo").nth(1)).to_contain_text("PLAYER 2")
    context.close()

def touch(browser, url, errors):
    context = browser.new_context(viewport={"width":390, "height":740},has_touch=True,is_mobile=True)
    page = visit(context,url,1796166000,errors)
    page.locator("#practice").tap()
    expect(page.locator("#touch")).to_be_visible()
    expect(page.locator(".viewinfo")).to_contain_text("tap Pulse")
    page.locator('[data-bit="2"]').tap()
    page.locator("#menu").tap()
    expect(page.locator("#panelbody")).to_contain_text("Controls")
    context.close()

def simulated_xr(browser,url,errors):
    context=browser.new_context(viewport={"width":800,"height":480})
    # Supply only the device boundary. The page runs its production XR code,
    # Wasm core and real GL renderer for both eyes.
    context.add_init_script("""(() => {
      const identity=new Float32Array([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1]);
      window.eyePasses=new Set();
      WebGLRenderingContext.prototype.makeXRCompatible=async()=>{};
      window.XRWebGLLayer=class {
        constructor(){this.framebuffer=null;}
        getViewport(v){window.eyePasses.add(v.eye);return {x:v.eye==='left'?0:200,y:0,width:200,height:200};}
      };
      class Session extends EventTarget {
        inputSources=[]; ended=false;
        updateRenderState(){}
        async requestReferenceSpace(){return {};}
        requestAnimationFrame(fn){return requestAnimationFrame(t=>{
          if(this.ended)return;
          const views=['left','right'].map(eye=>({eye,projectionMatrix:identity,transform:{inverse:{matrix:identity}}}));
          fn(t,{getViewerPose:()=>({views,transform:{orientation:{x:0,y:0,z:0,w:1}}})});
        });}
        async end(){this.ended=true;this.dispatchEvent(new Event('end'));}
      }
      Object.defineProperty(navigator,'xr',{value:{isSessionSupported:async()=>true,
        requestSession:async()=>window.testHeadset=new Session()}});
    })()""")
    page=visit(context,url,1796079600,errors)
    page.locator("#xr").click()
    page.wait_for_function("window.eyePasses.size === 2")
    page.evaluate("window.testHeadset.end()")
    expect(page.locator("#hud")).to_be_visible()
    context.close()

def connect_screen(page):
    page.locator("#together").click()
    page.locator("#net").click()

def peers(browser, url, errors, relay_url=None):
    first = browser.new_context(viewport={"width":760,"height":480})
    second = browser.new_context(viewport={"width":760,"height":480})
    host = visit(first,url,1796079600,errors)
    guest = visit(second,url,1796079599,errors)
    connect_screen(host)
    connect_screen(guest)
    if relay_url:
        for page in [host,guest]:
            page.locator("#relay-details summary").click()
            page.locator("#relay-url").fill(relay_url)
        host.locator("#relay-create").click()
        host.wait_for_function("document.getElementById('relay-code').value.length === 8")
        guest.locator("#relay-code").fill(host.locator("#relay-code").input_value())
        guest.locator("#relay-join").click()
    else:
        host.locator("#host").click()
        host.wait_for_function("document.getElementById('codeout').value.length > 0", timeout=18000)
        guest.locator("#codein").fill(host.locator("#codeout").input_value())
        guest.locator("#join").click()
        guest.wait_for_function("document.getElementById('codeout').value.length > 0", timeout=18000)
        host.locator("#codein").fill(guest.locator("#codeout").input_value())
        host.locator("#accept").click()
    for page in [host,guest]:
        expect(page.locator("#location")).to_contain_text("rehearsal",timeout=18000)
        expect(page.locator(".view")).to_have_count(1)
    open_menu(guest)
    guest.locator("#browse").click()
    expect(guest.locator(".day.ready")).to_have_count(1)
    guest.locator("#close").click()
    # Host and guest see a single shared target count. Strafing left crosses the first usher’s
    # sightline. Repeated pulses tolerate differences in software GL frame rate.
    guest.bring_to_front()
    guest.keyboard.down("a")
    try:
        for _ in range(18):
            guest.keyboard.down("f")
            guest.wait_for_timeout(85)
            guest.keyboard.up("f")
            guest.wait_for_timeout(55)
            if "1/2" in host.locator(".viewinfo").inner_text():
                break
    finally:
        guest.keyboard.up("a")
    expect(host.locator(".viewinfo")).to_contain_text("1/2",timeout=7000)
    expect(guest.locator(".viewinfo")).to_contain_text("1/2",timeout=7000)
    first.close()
    second.close()

if __name__ == "__main__":
    args=argparse.ArgumentParser()
    args.add_argument("url",nargs="?",default="http://127.0.0.1:8571/")
    args.add_argument("--direct", action="store_true", help="also verify direct WebRTC on a network allowing ICE candidates")
    options=args.parse_args()
    url=options.url
    errors=[]
    with sync_playwright() as playwright:
        browser=playwright.chromium.launch(executable_path=os.environ.get("CHROMIUM"),
            args=["--enable-unsafe-swiftshader","--use-angle=swiftshader","--disable-dev-shm-usage",
                  "--disable-background-timer-throttling","--disable-renderer-backgrounding",
                  "--disable-backgrounding-occluded-windows"])
        desktop(browser,url,errors)
        touch(browser,url,errors)
        simulated_xr(browser,url,errors)
        relay_script=Path(__file__).resolve().parents[1]/"scripts"/"relay.py"
        relay=subprocess.Popen([sys.executable,str(relay_script),"--port","0"],stdout=subprocess.PIPE,text=True)
        try:
            port=int(relay.stdout.readline().strip().split()[-1])
            peers(browser,url,errors,f"ws://127.0.0.1:{port}")
        finally:
            relay.terminate()
            relay.wait(timeout=5)
        if options.direct:
            peers(browser,url,errors)
        browser.close()
    assert not errors,errors
    print("Browser acceptance passed: 24 briefings, midnight rollover, split views, mobile touch, simulated stereo XR and two networked browsers.")
