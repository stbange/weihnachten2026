"""Replay through actual keyboard events while advancing only the browser frame clock."""

from playwright.sync_api import expect


def browser_clear(browser, url, rehearsal):
    context = browser.new_context(viewport={"width": 650, "height": 390})
    context.add_init_script("Date.now=()=>1796079600*1000")
    context.add_init_script("""(() => {
        const original=window.requestAnimationFrame.bind(window);
        let manual=false, time=0, pending=[];
        window.requestAnimationFrame=fn=>{
            if(manual){pending.push(fn);return pending.length;}
            return original(t=>{if(manual)pending.push(fn);else fn(t);});
        };
        window.replayClock={
            freeze:()=>new Promise(resolve=>original(t=>{time=t;manual=true;original(resolve);})),
            advance:ticks=>{
                while(ticks>0){
                    const n=Math.min(5,ticks);ticks-=n;time+=n*1000/60;
                    const ready=pending;pending=[];ready.forEach(fn=>fn(time));
                }
            }
        };
    })()""")
    page = context.new_page()
    page.goto(url)
    expect(page.locator("#practice")).to_be_enabled(timeout=35000)
    page.locator("#practice" if rehearsal else "#enter").click()
    expect(page.locator(".viewinfo")).to_be_visible()
    page.evaluate("window.replayClock.freeze()")

    def hold(key, ticks):
        page.keyboard.down(key)
        page.evaluate("n=>replayClock.advance(n)", ticks)
        page.keyboard.up(key)
        page.evaluate("replayClock.advance(2)")

    if not rehearsal:
        hold("w", 28)
        hold("a", 95)
        assert "Door 01" in page.locator(".viewinfo").inner_text()
        hold("e", 3)
        assert "live calendar" in page.locator("#location").inner_text().lower()
    hold("a", 31)
    hold("f", 3)
    assert "1/2" in page.locator(".viewinfo").inner_text()
    hold("d", 64)
    hold("w", 55)
    hold("f", 3)
    assert "projector" in page.locator(".viewinfo").inner_text()
    hold("w", 182)
    hold("a", 105)
    hold("Space", 55)
    hold("e", 3)
    assert "ticket" in page.locator(".viewinfo").inner_text()
    hold("d", 72)
    hold("s", 247)
    hold("e", 3)
    assert "lantern hall" in page.locator("#location").inner_text().lower()
    assert page.evaluate("localStorage.getItem('lantern-hall.ticket.v1')") == (
        None if rehearsal else "1"
    )
    context.close()
