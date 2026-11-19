"""Replay through actual keyboard events while advancing only the browser frame clock."""


def browser_clear(browser, url, rehearsal):
    # A touch-capable device with a keyboard keeps the replay independent of
    # OS mouse warps when a browser enters relative pointer mode.
    context = browser.new_context(
        viewport={"width": 650, "height": 390}, has_touch=True
    )
    context.add_init_script("Date.now=()=>1796079600*1000")
    context.add_init_script("""(() => {
        let time=1000, pending=[];
        window.requestAnimationFrame=fn=>{pending.push(fn);return pending.length;};
        window.replayClock={
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
    page.wait_for_function(
        "!document.getElementById('practice').disabled", polling=50, timeout=35000
    )
    page.evaluate("replayClock.advance(2)")
    page.locator("#practice" if rehearsal else "#enter").focus()
    page.keyboard.press("Enter")
    page.evaluate("replayClock.advance(5)")
    status = page.locator(".viewinfo")
    assert (
        "0/2" if rehearsal else "runner"
    ) in status.inner_text(), status.inner_text()

    def hold(key, ticks):
        page.keyboard.down(key)
        page.evaluate("n=>replayClock.advance(n)", ticks)
        page.keyboard.up(key)
        page.evaluate("replayClock.advance(2)")

    if not rehearsal:
        hold("w", 28)
        hold("a", 95)
        assert "Door 01" in status.inner_text(), status.inner_text()
        hold("e", 3)
        assert "live calendar" in page.locator("#location").inner_text().lower()
        assert "Tram terminus" in status.inner_text(), status.inner_text()
        assert "1/5" in status.inner_text()
        context.close()
        return
    hold("a", 31)
    hold("f", 3)
    assert "1/2" in status.inner_text(), status.inner_text()
    hold("d", 64)
    hold("w", 55)
    hold("f", 3)
    assert "projector" in status.inner_text(), status.inner_text()
    hold("w", 182)
    hold("a", 105)
    hold("Space", 55)
    hold("e", 3)
    assert "ticket" in status.inner_text(), status.inner_text()
    hold("d", 72)
    hold("s", 247)
    hold("e", 3)
    assert "lantern hall" in page.locator("#location").inner_text().lower()
    assert page.evaluate("localStorage.getItem('lantern-hall.ticket.v1')") == (
        None if rehearsal else "1"
    )
    context.close()
