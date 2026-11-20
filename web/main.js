import { PlaytestLog } from './playtest.mjs';
import { missionAtlas } from './atlas.js';
import { fieldGuide } from './field-guide.js';
import { Link } from './network.js';
import { Relay } from './relay.js';
import { immersive } from './xr.js';
const $ = (id) => document.getElementById(id),
  stage = $('stage'),
  canvas = $('canvas'),
  dialog = $('panel'),
  body = $('panelbody');
let storage;
try {
  storage = window.localStorage;
} catch {}
const fieldLog = new PlaytestLog(storage);
let resultsDue = null;
function showFieldLog(initialDay = 1) {
  fieldGuide({
    log: fieldLog,
    focusId: resultsDue?.id,
    cards,
    modal,
    esc,
    xrSupported,
    initialDay: Number.isInteger(initialDay) ? initialDay : 1,
    start: (day, mode, firstVisit) => {
      start(false, mode === 'Split screen', '', mode !== 'WebXR', day, true);
      if (mode === 'WebXR')
        goVR(() => fieldLog.begin(day, mode, firstVisit, performance.now()));
      else fieldLog.begin(day, mode, firstVisit, performance.now());
    },
  });
  resultsDue = null;
}
addEventListener('pagehide', (e) => {
  if (e.persisted) fieldLog.pause(performance.now());
  else fieldLog.end('Stopped', performance.now());
});
document.addEventListener('visibilitychange', () => {
  if (document.hidden && !xrActive) fieldLog.pause(performance.now());
});
const sources = 'https://github.com/stbange/weihnachten2026/blob/main/';
let M,
  cards = [],
  playing = false,
  split = false,
  role = '',
  open = 0,
  sound = false,
  audio,
  xrActive = false;
let friendsRehearse = true,
  remote = { v: [0, 0, 0, 0, 0], at: 0, pulses: 0 },
  last = 0,
  lastSend = 0,
  lastClock = 0,
  pendingLook = [0, 0],
  lastHud = '',
  toastUntil = 0;
const keys = new Set(),
  touch = { f: 0, s: 0, bits: 0 },
  taps = [0, 0];
let focusSeat = 0;
const coarse =
  matchMedia('(pointer:coarse)').matches || navigator.maxTouchPoints > 0;
const state = () =>
  Array.from(M.HEAPF32.subarray(M._lh_state() / 4, M._lh_wire() / 4 + 39));
const currentDay = () => M._lh_today(Date.now() / 1000);
const persisted = () => {
  try {
    return (
      (Number(localStorage.getItem('lantern-hall.keepsakes.v2')) & 0xffffff) |
      Number(localStorage.getItem('lantern-hall.ticket.v1') === '1')
    );
  } catch {
    return 0;
  }
};
const persist = (bits) => {
  try {
    localStorage.setItem(
      'lantern-hall.keepsakes.v2',
      String(bits | persisted()),
    );
  } catch {
    /* private session */
  }
};
const esc = (s) =>
  String(s).replace(
    /[&<>"']/g,
    (k) =>
      ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' })[
        k
      ],
  );
function chime(notes = [392, 494, 587]) {
  if (!sound) return;
  try {
    audio ??= new AudioContext();
    audio.resume();
    notes.forEach((n, i) => {
      const o = audio.createOscillator(),
        v = audio.createGain(),
        t = audio.currentTime + 0.085 * i;
      o.type = 'sine';
      o.frequency.value = n;
      v.gain.setValueAtTime(0.0001, t);
      v.gain.exponentialRampToValueAtTime(0.07, t + 0.012);
      v.gain.exponentialRampToValueAtTime(0.0001, t + 0.42);
      o.connect(v);
      v.connect(audio.destination);
      o.start(t);
      o.stop(t + 0.43);
    });
  } catch {
    /* Audio is optional. */
  }
}
$('sound').onclick = () => {
  sound = !sound;
  $('sound').textContent = sound ? 'Sound on' : 'Sound off';
  $('sound').setAttribute('aria-pressed', sound);
  chime();
};
function resetInput() {
  keys.clear();
  touch.f = touch.s = touch.bits = 0;
  taps.fill(0);
  pendingLook = [0, 0];
  $('stick').firstElementChild.style.transform = '';
}
function modal(section, html) {
  if (document.pointerLockElement) document.exitPointerLock();
  if (role !== 'host') fieldLog.pause(performance.now());
  resetInput();
  $('section').textContent = section;
  body.innerHTML = html;
  if (!dialog.open) dialog.showModal();
  dialog.scrollTop = 0;
}
function closePanel() {
  dialog.close();
  resetInput();
  if (playing) canvas.focus();
}
$('close').onclick = closePanel;
dialog.addEventListener('cancel', (e) => {
  e.preventDefault();
  closePanel();
});
function notify(s) {
  $('toast').textContent = s;
  toastUntil = performance.now() + 4200;
}
function locationUI(active) {
  playing = active;
  for (const id of ['shade', 'mast', 'welcome', 'strip']) $(id).hidden = active;
  $('hud').hidden = !active;
  $('touch').hidden = !(active && coarse);
  stage.classList.toggle('touching', active && coarse);
  $('views').innerHTML = Array.from(
    { length: split ? 2 : 1 },
    () =>
      '<div class="view"><i class="ret"></i><div class="viewinfo"></div></div>',
  ).join('');
  lastHud = '';
  resetInput();
}
function start(
  rehearsal = false,
  localSplit = false,
  online = '',
  lock = true,
  preview = 0,
  trial = false,
) {
  if (!M) return;
  fieldLog.end('Stopped', performance.now());
  if (!online) {
    link.reset();
    relay.reset();
  }
  remote = { v: [0, 0, 0, 0, 0], at: 0, pulses: 0 };
  lastSend = 0;
  visitSeconds = 0;
  timingDoor = 0;
  role = online;
  split = localSplit;
  focusSeat = role === 'guest' ? 1 : 0;
  open = currentDay();
  if (dialog.open) closePanel();
  M._lh_start(split || role ? 2 : 1, open, rehearsal ? 1 : 0, persisted());
  if (trial) M._lh_trial(preview);
  else if (preview) M._lh_preview(preview);
  locationUI(true);
  canvas.focus();
  chime([294, 370, 440]);
  last = 0;
  if (lock && !online && !coarse)
    canvas.requestPointerLock?.()?.catch?.(() => {});
}
function home() {
  fieldLog.end('Stopped', performance.now());
  link.reset();
  relay.reset();
  role = '';
  split = false;
  focusSeat = 0;
  if (dialog.open) closePanel();
  M._lh_start(1, currentDay(), 0, persisted());
  locationUI(false);
  $('enter').focus();
  if (document.pointerLockElement) document.exitPointerLock();
}
function hallStory(bits) {
  const guests = Array.from({ length: 24 }, (_, i) => (bits >> i) & 1).reduce(
    (a, b) => a + b,
    0,
  );
  if (guests === 24)
    return 'The second recording plays. Someone behind the camera whispers: start anyway. The clock strikes twelve.';
  if (bits & (1 << 23))
    return 'The last place card is blank. Pull up a chair for the friend who arrives after midnight.';
  if ((bits & 0x3f000) === 0x3f000)
    return 'A late tram rings outside. Someone sets another place.';
  if ((bits & 0xfc0) === 0xfc0)
    return 'Across the table, familiar voices are beginning to answer each other.';
  if ((bits & 0x3f) === 0x3f)
    return 'The first guests have taken their coats off. Candles burn beside their keepsakes.';
  return 'There is room at the table for twenty-four memories.';
}
function almanac() {
  if (!cards.length) return;
  const count = M ? (role === 'guest' ? state()[2] : currentDay()) : 0;
  const bits = playing ? state()[6] : persisted();
  const grid = cards
    .map(
      (c) =>
        `<button class="day ${c.day <= count ? 'ready' : ''}" data-day="${c.day}" aria-label="Door ${c.day}, ${esc(c.title)}"><strong>${String(c.day).padStart(2, '0')}</strong><small>${bits & (1 << (c.day - 1)) ? 'returned' : c.day <= count ? 'open' : 'sealed'}</small></button>`,
    )
    .join('');
  modal(
    'PORTAL ALMANAC',
    `<h2>Twenty-four evenings</h2><p>One door opens at midnight in Warsaw each day, December 1–24. Select a door to playtest its route. Calendar clears return keepsakes to the hall.</p><p>${esc(hallStory(bits))}</p><div class="grid">${grid}</div><p><button id="fieldnotes" class="choice">Playtest log</button></p><a class="booklink" href="${sources}design/lantern-hall.md" target="_blank" rel="noopener">Read the campaign design →</a> &nbsp; <a class="booklink" href="${sources}ART-CREDITS.md" target="_blank" rel="noopener">Art credits</a>`,
  );
  $('fieldnotes').onclick = showFieldLog;
  body
    .querySelectorAll('[data-day]')
    .forEach((b) => (b.onclick = () => briefing(+b.dataset.day)));
}
function openAtlas(day) {
  missionAtlas({
    cards,
    day,
    modal,
    esc,
    engine: M,
    back: briefing,
    play: role ? null : (n) => start(false, split, '', true, n),
  });
}
function briefing(day) {
  const c = cards[day - 1];
  if (!c) return;
  modal(
    'DOOR ' +
      String(day).padStart(2, '0') +
      '  /  ' +
      String(day).padStart(2, '0') +
      ' DECEMBER',
    `<h2>${esc(c.title)}</h2><div class="source">${esc(c.reference)}</div><p>${esc(c.mission)}</p><ul>${c.encounters.map((beat) => '<li><strong>' + esc(beat.name) + '</strong> · ' + esc(beat.instruction) + '</li>').join('')}</ul><details><summary>Twenty-minute mission design</summary><p>${esc(c.place)}</p><p>${esc(c.route)}</p><p>${esc(c.coop)}</p></details><div class="buttons"><button id="backbook" class="choice">All 24 doors</button><button id="atlas" class="choice">View floor plans</button>${!role ? '<button id="trydoor" class="primary">Playtest mission →</button>' : ''}</div><p class="source">Five areas · practice is available before opening day.</p><p>${persisted() & (1 << (day - 1)) ? esc(c.gift) : ''}</p>`,
  );
  $('backbook').onclick = almanac;
  $('atlas').onclick = () => openAtlas(day);
  if ($('trydoor'))
    $('trydoor').onclick = () => start(false, split, '', true, day);
}
function controlsText() {
  return '<p><b>Player 1</b> · WASD move, mouse or arrows look, F or click pulses, E uses, Space lifts. Drag to look if your browser releases the mouse.</p><p><b>Player 2</b> · IJKL move, U/O turn, Y pulses, H uses, N lifts. On a touchscreen the first seat uses the on-screen controls.</p>';
}
function menu() {
  modal(
    'LANTERN HALL',
    `<h2>At the landing</h2><div class="buttons"><button id="resume" class="primary">Resume</button><button id="browse" class="choice">Portal almanac</button><button id="notebook" class="choice">Playtest log</button><button id="leave" class="choice">Return to title</button></div><details open><summary>Controls</summary>${controlsText()}</details>${!role && !split && xrSupported ? vrOptions() + '<button id="xrhere" class="choice">Enter VR</button>' : ''}`,
  );
  $('notebook').onclick = showFieldLog;
  $('resume').onclick = closePanel;
  $('browse').onclick = almanac;
  $('leave').onclick = home;
  if ($('xrhere')) {
    $('xrhere').onclick = goVR;
    for (const [id, key] of [
      ['vrspeed', 'speed'],
      ['vrturn', 'turn'],
    ])
      $(id).onchange = (e) => {
        vrPreferences[key] = Number(e.target.value);
        try {
          storage?.setItem('lantern-hall.vr.v1', JSON.stringify(vrPreferences));
        } catch {}
      };
  }
}
$('enter').onclick = () => start();
$('practice').onclick = () => start(true);
$('featured').onclick = () => start(false, false, '', true, 1);
$('book').onclick = almanac;
$('menu').onclick = menu;
$('chart').onclick = () => {
  const s = state();
  if (s[1] !== 2) return;
  modal(
    cards[s[8] - 1].title,
    '<h2>Sector map</h2><p>' +
      esc(M.UTF8ToString(M._lh_sector_brief())) +
      '</p><canvas id="chartmap" width="350" height="350" aria-label="Sector plan with player positions and objective" style="max-width:100%;height:auto"></canvas><p>Gold marks the objective. Copper marks the special route and its two stations. Mint marks the optional ward room and its return line. Green and blue mark the two player positions. The arrow shows which way you face.</p><button id="backgame" class="primary">Resume</button>',
  );
  const ctx = $('chartmap').getContext('2d'),
    extent = M._lh_extent(),
    cell = 350 / extent;
  for (let z = 0; z < extent; z++)
    for (let x = 0; x < extent; x++) {
      const tile = M._lh_chart(x, z);
      ctx.fillStyle = [
        '#566d60',
        '#162c2b',
        '#f3c366',
        '#e59e7a',
        '#967661',
        '#7ef1d8',
        '#4bb5b0',
      ][tile];
      ctx.fillRect(x * cell, z * cell, Math.ceil(cell), Math.ceil(cell));
    }
  for (let p = 0; p < s[9]; p++) {
    const k = 10 + p * 6;
    ctx.save();
    ctx.translate((s[k] / 4 + 0.5) * cell, (s[k + 2] / 4 + 0.5) * cell);
    ctx.rotate(s[k + 3]);
    ctx.fillStyle = p ? '#9dbeef' : '#95f1be';
    ctx.beginPath();
    ctx.moveTo(0, -8);
    ctx.lineTo(5, 5);
    ctx.lineTo(-5, 5);
    ctx.closePath();
    ctx.fill();
    ctx.restore();
  }
  $('backgame').onclick = closePanel;
};
function connected(newRole) {
  const timed = newRole === 'host' && friendsDay && networkTrial;
  start(friendsRehearse, false, newRole, true, friendsDay, timed);
  if (timed)
    fieldLog.begin(friendsDay, 'Network host', networkFirst, performance.now());
  notify('Connected · two seats at the table');
}
function incoming(msg) {
  if (!Array.isArray(msg) || !msg.every(Number.isFinite)) return;
  if (role === 'host' && msg.length === 7) {
    const [f, s, t, p, b, yaw, pitch] = msg;
    if (
      [f, s, t, p].some((x) => Math.abs(x) > 1) ||
      b < 0 ||
      b > 7 ||
      !Number.isInteger(b)
    )
      return;
    remote = {
      v: [f, s, t, p, b],
      at: performance.now(),
      pulses: remote.pulses | (b & ~remote.v[4] & 3),
    };
    M._lh_aim(
      1,
      Math.max(-1, Math.min(1, yaw)),
      Math.max(-1, Math.min(1, pitch)),
    );
  } else if (role === 'guest' && (msg.length === 39 || msg.length === 32)) {
    if (msg.length !== 39 || msg[0] !== M.HEAPF32[M._lh_state() / 4]) {
      home();
      modal(
        'MULTIPLAYER',
        '<h2>Game versions differ</h2><p>Reload both clients or update the native build, then reconnect.</p>',
      );
      return;
    }
    M.HEAPF32.set(msg, M._lh_wire() / 4);
    if (M._lh_receive()) events();
  }
}
function disconnected() {
  if (playing) {
    home();
    modal(
      'MULTIPLAYER',
      '<h2>Connection closed</h2><p>Your browser has returned to the title. Any calendar ticket already earned is saved on this device.</p>',
    );
  }
}
const link = new Link(connected, incoming, disconnected),
  relay = new Relay(connected, incoming, disconnected);
let transport = link,
  friendsDay = 0,
  networkTrial = false,
  networkFirst = false;
function multiplayer() {
  if (!M) return;
  modal(
    'MULTIPLAYER',
    `<h2>Pull up a chair</h2><p>Choose where to meet, then share a screen or connect two browsers.</p><label><input id="meetpractice" type="checkbox" ${friendsRehearse ? 'checked' : ''}> Meet in the short cinema rehearsal</label><label>Campaign playtest <select id="meetday"><option value="0">Calendar hall or rehearsal</option>${cards.map((c) => '<option value="' + c.day + '" ' + (friendsDay === c.day ? 'selected' : '') + '>' + c.day + ' · ' + esc(c.title) + '</option>').join('')}</select></label><details><summary>Record a network playtest</summary><p>Select a campaign door above. The host records one timed run for the party.</p><label><input id="nettrial" type="checkbox" ${networkTrial ? 'checked' : ''}> Start from sector one and record</label><label><input id="netfirst" type="checkbox" ${networkFirst ? 'checked' : ''}> Everyone is new to this door</label></details><div class="buttons"><button id="splits" class="primary">Start split screen</button><button id="net" class="choice">Connect browsers</button></div><details><summary>Two-player controls</summary>${controlsText()}</details>`,
  );
  $('nettrial').onchange = (e) => (networkTrial = e.target.checked);
  $('netfirst').onchange = (e) => (networkFirst = e.target.checked);
  $('meetpractice').onchange = (e) => (friendsRehearse = e.target.checked);
  $('meetday').onchange = (e) => (friendsDay = Number(e.target.value));
  $('splits').onclick = () =>
    start(friendsRehearse, true, '', true, friendsDay);
  $('net').onclick = netPanel;
}
function netPanel() {
  modal(
    'DIRECT WEB CO-OP',
    `<h2>Connect browsers</h2><p>The host creates an invitation. A friend pastes it below and sends back a reply. Paste that reply on the host to connect. Codes contain connection details, so share them privately.</p><label for="codein">Invitation or reply from your friend</label><textarea id="codein" autocomplete="off" spellcheck="false"></textarea><div class="buttons"><button class="choice" id="host">Create invitation</button><button class="choice" id="join">Make reply</button><button class="choice" id="accept">Connect with reply</button></div><label for="codeout">Your code to share</label><textarea id="codeout" readonly></textarea><button class="choice" id="copy">Copy code</button><p id="net-status" role="status">Direct WebRTC uses a public STUN service. On restrictive networks, use the relay option below.</p><details id="relay-details"><summary>Connect through a WebSocket relay</summary><p>Both players enter the same relay address. The host creates a room and sends its eight-character code to the guest.</p><label for="relay-url">Relay address</label><input id="relay-url" type="url" placeholder="wss://your-relay.example"><label for="relay-code">Room code</label><input id="relay-code" maxlength="8" autocomplete="off"><div class="buttons"><button class="choice" id="relay-create">Create room</button><button class="choice" id="relay-join">Join room</button></div><p role="status" id="relay-status">Use a relay run by you or a friend.</p></details>`,
  );
  const relayStatus = (message, code) => {
    const status = $('relay-status');
    if (status) status.textContent = message;
    if (code && $('relay-code')) $('relay-code').value = code;
  };
  const relayConnect = (join) => {
    link.reset();
    transport = relay;
    const code = $('relay-code').value.trim();
    if (join && code.length !== 8) {
      relayStatus('Enter the eight-character code from the host.');
      return;
    }
    try {
      relayStatus('Contacting the relay…');
      relay.open($('relay-url').value.trim(), join ? code : '', relayStatus);
    } catch (e) {
      relayStatus(e.message);
    }
  };
  $('relay-create').onclick = () => relayConnect(false);
  $('relay-join').onclick = () => relayConnect(true);
  if (!window.RTCPeerConnection) {
    $('net-status').textContent = 'Use the relay option in this browser.';
    return;
  }
  async function run(fn, msg) {
    relay.reset();
    transport = link;
    const status = $('net-status'),
      out = $('codeout');
    status.textContent = 'Preparing connection…';
    try {
      const code = await fn();
      if (code) out.value = code;
      status.textContent = msg;
    } catch (e) {
      status.textContent = e.message;
    }
  }
  $('host').onclick = () =>
    run(
      () => link.host(),
      'Send your invitation. Paste your friend’s reply in the first box.',
    );
  $('join').onclick = () =>
    run(
      () => link.join($('codein').value),
      'Send your reply to the host and leave this window open.',
    );
  $('accept').onclick = () =>
    run(
      () => link.accept($('codein').value),
      'Connecting to the other browser…',
    );
  $('copy').onclick = async () => {
    const area = $('codeout');
    area.select();
    try {
      await navigator.clipboard.writeText(area.value);
      $('net-status').textContent = 'Code copied.';
    } catch {
      $('net-status').textContent =
        'The code is selected. Copy it with your device’s copy command.';
    }
  };
}
$('together').onclick = multiplayer;
const bindings = new Set([
  'KeyW',
  'KeyA',
  'KeyS',
  'KeyD',
  'ArrowLeft',
  'ArrowRight',
  'ArrowUp',
  'ArrowDown',
  'KeyF',
  'KeyE',
  'Space',
  'KeyI',
  'KeyK',
  'KeyJ',
  'KeyL',
  'KeyU',
  'KeyO',
  'KeyH',
  'KeyY',
  'KeyN',
]);
const oneShot = { KeyE: [0, 1], KeyF: [0, 2], KeyH: [1, 1], KeyY: [1, 2] };
addEventListener(
  'keydown',
  (e) => {
    if (!playing || dialog.open) return;
    if (e.code === 'Escape') {
      e.preventDefault();
      menu();
      return;
    }
    if (!bindings.has(e.code)) return;
    e.preventDefault();
    keys.add(e.code);
    if (!e.repeat && oneShot[e.code]) {
      const [i, b] = oneShot[e.code];
      taps[i] |= b;
    }
  },
  true,
);
addEventListener('keyup', (e) => keys.delete(e.code), true);
addEventListener('blur', resetInput);
let dragging = false;
canvas.onpointerdown = (e) => {
  if (!playing || dialog.open || e.pointerType === 'touch') return;
  dragging = true;
  canvas.setPointerCapture?.(e.pointerId);
  taps[0] |= 2;
  if (!document.pointerLockElement)
    canvas.requestPointerLock?.()?.catch?.(() => {});
};
addEventListener('pointerup', () => (dragging = false));
function aim(x, y) {
  if (role === 'guest') {
    pendingLook[0] += x;
    pendingLook[1] += y;
  } else M._lh_aim(0, x, y);
}
addEventListener('mousemove', (e) => {
  if (
    playing &&
    !dialog.open &&
    (document.pointerLockElement === canvas || dragging)
  )
    aim(e.movementX * 0.0032, -e.movementY * 0.0032);
});
function pointerArea(el, begin, move, end) {
  let id = null;
  el.onpointerdown = (e) => {
    if (id !== null) return;
    id = e.pointerId;
    el.setPointerCapture(id);
    e.preventDefault();
    begin(e);
  };
  el.onpointermove = (e) => {
    if (e.pointerId === id) move(e);
  };
  el.onpointerup = el.onpointercancel = (e) => {
    if (e.pointerId === id) {
      id = null;
      end(e);
    }
  };
}
let origin = [0, 0];
const stick = $('stick');
function joystick(e) {
  const x = e.clientX - origin[0],
    y = e.clientY - origin[1],
    d = Math.max(30, Math.hypot(x, y));
  touch.s = x / d;
  touch.f = -y / d;
  stick.firstElementChild.style.transform = `translate(${touch.s * 28}px,${-touch.f * 28}px)`;
}
pointerArea(
  stick,
  (e) => {
    const r = stick.getBoundingClientRect();
    origin = [r.x + r.width / 2, r.y + r.height / 2];
    joystick(e);
  },
  joystick,
  () => {
    touch.s = touch.f = 0;
    stick.firstElementChild.style.transform = '';
  },
);
let finger = [0, 0];
pointerArea(
  $('lookpad'),
  (e) => (finger = [e.clientX, e.clientY]),
  (e) => {
    aim((e.clientX - finger[0]) * 0.008, (finger[1] - e.clientY) * 0.008);
    finger = [e.clientX, e.clientY];
  },
  () => {},
);
document.querySelectorAll('[data-bit]').forEach((b) => {
  const v = +b.dataset.bit;
  pointerArea(
    b,
    () => {
      touch.bits |= v;
      taps[0] |= v;
    },
    () => {},
    () => (touch.bits &= ~v),
  );
});
function input(seat) {
  const k = (code) => Number(keys.has(code));
  if (dialog.open) return [0, 0, 0, 0, 0];
  if (seat === 1)
    return [
      k('KeyI') - k('KeyK'),
      k('KeyL') - k('KeyJ'),
      k('KeyO') - k('KeyU'),
      0,
      k('KeyH') | (k('KeyY') * 2) | (k('KeyN') * 4) | taps[1],
    ];
  return [
    Math.max(-1, Math.min(1, k('KeyW') - k('KeyS') + touch.f)),
    Math.max(-1, Math.min(1, k('KeyD') - k('KeyA') + touch.s)),
    k('ArrowRight') - k('ArrowLeft'),
    k('ArrowUp') - k('ArrowDown'),
    k('KeyE') |
      ((k('KeyF') || Number(dragging && !!document.pointerLockElement)) * 2) |
      (k('Space') * 4) |
      touch.bits |
      taps[0],
  ];
}
const tips = [
  '',
  'Paper usher folded',
  'The projector is running',
  'Ticket returned to the hall',
  '',
  'Entered mission',
  'This portal is sealed',
  'Playtest complete · back at the hall',
  'Objective complete',
];
function events() {
  const event = M._lh_event();
  if (!event) return;
  const s = state();
  if (event === 3 || event === 7) {
    if (event === 7 && fieldLog.active && s[8] === fieldLog.active.door) {
      resultsDue = { id: fieldLog.active.id, day: s[8] };
      fieldLog.end('Completed', performance.now());
      if (!xrActive) showFieldLog(s[8]);
    }
    if (event === 3 && s[6]) persist(1 << (s[8] - 1));
    chime([392, 494, 587, 784]);
  }
  if (event === 9) {
    const recordings = [
      [
        'I counted twenty-four plates. Who was holding the camera?',
        'The ticket has a crooked mark. It matches the one on my chair.',
        'The candle in the window was for the last tram.',
        'Listen. Somebody is unfolding the spare table.',
        'Leave one place free. We said it before the lights went out.',
      ],
      [
        'This cable goes past the hall and back under the street.',
        'We addressed every envelope. None of them left the building.',
        'The relay kept asking for one more name.',
        'The same instruction is on every routing slip.',
        'Hold until everyone is here. That was our handwriting.',
      ],
      [
        'We used to start the match while the kettle boiled.',
        'There was always a way back in after the first round.',
        'Someone wrote a correction in the margin.',
        'It says: start anyway.',
        'We saved the seat. The machine saved the whole evening.',
      ],
      [
        'We can still send the invitations ourselves.',
        'A light has come on beyond the front door.',
        'The friends inside the recordings are answering.',
        'That last address is for whoever comes late.',
        'The clock can keep going now.',
      ],
    ];
    chime([330, 440, 554]);
    notify('Recording · ' + recordings[Math.floor((s[8] - 1) / 6)][s[22]]);
    return;
  }
  if (event === 2 || event === 1)
    chime(event === 1 ? [523, 659] : [330, 440, 554]);
  if ((event === 4 || event === 6) && !role && !xrActive) {
    briefing(s[8]);
    return;
  }
  if ((event === 3 || event === 7) && visitSeconds > 0) {
    notify(
      (event === 3 ? cards[s[8] - 1].gift : 'Playtest complete.') +
        ' · This visit ' +
        duration(visitSeconds),
    );
    visitSeconds = 0;
  } else if (event === 3) notify(cards[s[8] - 1].gift);
  else if (tips[event]) notify(tips[event]);
}
function hud(s) {
  const near = Array.from({ length: split ? 2 : 1 }, (_, i) =>
    M._lh_near(split ? i : focusSeat),
  );
  $('chart').hidden = s[1] !== 2;
  const prompts =
    s[1] === 2
      ? near.map((_, i) => M.UTF8ToString(M._lh_prompt(split ? i : focusSeat)))
      : [];
  const hash = [
    ...prompts,
    s[8],
    s[22],
    s[23],
    s[1],
    s[2],
    s[3],
    s[4],
    s[5],
    s[6],
    ...near,
    Math.round(s[15] * 20),
    Math.round(s[21] * 20),
  ].join('/');
  if (hash === lastHud) return;
  lastHud = hash;
  $('location').textContent = s[1]
    ? 'Door ' +
      String(s[8]).padStart(2, '0') +
      (s[3]
        ? s[1] === 2
          ? ' · playtest'
          : ' · rehearsal'
        : ' · live calendar')
    : 'Lantern Hall · ' + s[2] + '/24 open';
  document.querySelectorAll('.viewinfo').forEach((info, i) => {
    const nearDay = near[i],
      p = split ? i : focusSeat,
      n = (s[4] & 1) + ((s[4] >> 1) & 1);
    const label = split
      ? 'PLAYER ' + (i + 1)
      : s[1]
        ? s[1] === 2
          ? esc(cards[s[8] - 1].title.toUpperCase())
          : 'HOUSE LIGHTS'
        : 'THE GREAT HALL';
    let goal, sub;
    if (!s[1]) {
      goal = nearDay
        ? 'Door ' +
          String(nearDay).padStart(2, '0') +
          ' · ' +
          cards[nearDay - 1].title
        : 'Walk the runner and approach a numbered arch.';
      sub = nearDay
        ? (nearDay > s[2] ? 'Sealed · opens December ' + nearDay : 'Ready') +
          (nearDay > s[2] ? ' · Use for briefing' : ' · Walk through')
        : coarse
          ? 'Left stick moves · drag right to look · tap Use'
          : 'WASD moves · mouse or arrows look · E inspects';
    } else if (s[1] === 2) {
      const [area, tool, action, ...extra] = prompts[i].split(' | ');
      goal = s[22] + 1 + '/5 · ' + area + ' · ' + tool;
      sub = [action, ...extra].join(' · ');
    } else if (n < 2) {
      goal = 'Fold the two paper ushers · ' + n + '/2';
      sub = coarse
        ? 'Aim and tap Pulse'
        : split && i === 1
          ? 'Aim at the cardboard figures · Y pulses'
          : 'Aim at the cardboard figures · F or click to pulse';
    } else if (!s[5]) {
      goal = 'Start the projector on the left balcony.';
      sub =
        (coarse
          ? 'Hold Lift, then tap Use'
          : 'Hold ' +
            (p ? 'N' : 'Space') +
            ' to boost, then ' +
            (p ? 'H' : 'E') +
            ' to use') +
        ' · Fuel ' +
        Math.round(s[15 + p * 6] * 100) +
        '%';
    } else {
      goal = 'Bring the ticket to the green entrance door.';
      sub = coarse
        ? 'Tap Use at the door'
        : 'Press ' + (p ? 'H' : 'E') + ' at the entrance';
    }
    info.innerHTML =
      '<small>' + label + '</small>' + esc(goal) + '<i>' + esc(sub) + '</i>';
  });
}
let visitSeconds = 0,
  timingDoor = 0;
function timeVisit(s, elapsed) {
  if (s[1] !== 2) {
    timingDoor = 0;
    return;
  }
  if (timingDoor !== s[8]) {
    timingDoor = s[8];
    visitSeconds = 0;
  }
  if (!document.hidden && !dialog.open) visitSeconds += elapsed;
}
function duration(seconds) {
  return (
    Math.floor(seconds / 60) +
    'm ' +
    String(Math.floor(seconds % 60)).padStart(2, '0') +
    's'
  );
}
let xrSupported = false;
const vrPreferences = { speed: 0.75, turn: 30 };
try {
  const saved = JSON.parse(storage?.getItem('lantern-hall.vr.v1') || '{}');
  if ([0.5, 0.75, 1].includes(saved.speed)) vrPreferences.speed = saved.speed;
  if ([15, 30, 45].includes(saved.turn)) vrPreferences.turn = saved.turn;
} catch {}
function vrOptions() {
  const select = (id, values, key, unit) =>
    '<select id="' +
    id +
    '">' +
    values
      .map(
        (value) =>
          '<option value="' +
          value +
          '" ' +
          (vrPreferences[key] === value ? 'selected' : '') +
          '>' +
          Math.round(value * (key === 'speed' ? 100 : 1)) +
          unit +
          '</option>',
      )
      .join('') +
    '</select>';
  return (
    '<details><summary>VR comfort controls</summary><label>Movement speed ' +
    select('vrspeed', [0.5, 0.75, 1], 'speed', '%') +
    '</label><label>Snap turn ' +
    select('vrturn', [15, 30, 45], 'turn', '°') +
    '</label><p>Right stick turns. Trigger pulses, grip uses and the primary face button lifts. The secondary face button hides or shows the reading panel.</p></details>'
  );
}
async function goVR(onReady) {
  if (!M || role || split) return;
  if (!playing) start(false, false, '', false);
  if (dialog.open) closePanel();
  xrActive = true;
  if (fieldLog.active) fieldLog.end('Stopped', performance.now());
  try {
    await immersive(
      M,
      (dt, tracked, device) => {
        const s = state();
        if (fieldLog.active) {
          fieldLog.sample(performance.now(), s[22], tracked);
          fieldLog.xrFrame(dt, tracked, device);
        }
        timeVisit(s, tracked ? dt : 0);
        M._lh_tick(dt, tracked ? 1 : 0);
        events();
      },
      state,
      () => {
        if (fieldLog.active?.mode === 'WebXR') {
          resultsDue = { id: fieldLog.active.id, day: fieldLog.active.door };
          fieldLog.end('Stopped', performance.now());
        }
        xrActive = false;
        last = 0;
        lastVisitFrame = 0;
        resetInput();
        if (resultsDue) showFieldLog(resultsDue.day);
      },
      (s) => {
        if (s[1] === 2) {
          const [area, tool, ...steps] = M.UTF8ToString(M._lh_prompt(0)).split(
            ' | ',
          );
          const recent =
            performance.now() < toastUntil &&
            $('toast').textContent.startsWith('Recording');
          return [
            s[8] + ' · ' + area + ' · ' + tool,
            recent ? $('toast').textContent : steps.join(' · '),
          ];
        }
        hud(s);
        return [
          $('location').textContent,
          document.querySelector('.viewinfo')?.innerText.replace(/\n/g, ' ') ||
            'Walk to a numbered arch',
        ];
      },
      vrPreferences,
    );
    if (typeof onReady === 'function') onReady();
  } catch (e) {
    xrActive = false;
    notify('VR session could not start · ' + e.message);
  }
}
$('xr').onclick = goVR;
let lastVisitFrame = 0;
function render(t) {
  requestAnimationFrame(render);
  if (!M || xrActive) return;
  const dt = last ? Math.min((t - last) / 1000, 0.1) : 0;
  last = t;
  if (t - lastClock > 750) {
    open = currentDay();
    if (role !== 'guest') M._lh_open(open);
    lastClock = t;
  }
  M._lh_resize(canvas.clientWidth, canvas.clientHeight);
  if (playing) {
    const v = input(0);
    if (role === 'guest') {
      if (t - lastSend > 32 || v[4] || pendingLook.some(Boolean)) {
        transport.send([...v, ...pendingLook]);
        pendingLook = [0, 0];
        lastSend = t;
        taps[0] = 0;
      }
    } else {
      M._lh_input(0, ...v);
      if (split) M._lh_input(1, ...input(1));
      else if (role === 'host') {
        const v = t - remote.at < 400 ? [...remote.v] : [0, 0, 0, 0, 0];
        v[4] |= remote.pulses;
        M._lh_input(1, ...v);
      }
    }
    const steps = M._lh_tick(dt, role !== 'guest' && (!dialog.open || !!role));
    const s = state();
    if (fieldLog.active && s[1] === 2)
      fieldLog.sample(
        performance.now(),
        s[22],
        !document.hidden && (!dialog.open || role === 'host'),
      );
    timeVisit(s, lastVisitFrame ? Math.min(1, (t - lastVisitFrame) / 1000) : 0);
    if (role === 'host' && (t - lastSend > 50 || s[7])) {
      transport.send(s);
      lastSend = t;
    }
    if (role !== 'guest') events();
    hud(s);
    if (t > toastUntil) $('toast').textContent = '';
    if (steps) {
      taps[0] = taps[1] = remote.pulses = 0;
    }
  } else M._lh_tick(dt, 0);
  lastVisitFrame = t;
  M._lh_draw(!playing, split, focusSeat);
}
async function boot() {
  try {
    const result = await Promise.all([
      createLantern({ canvas }),
      fetch('almanac.json').then((r) => r.json()),
    ]);
    const engine = result[0];
    cards = result[1];
    if (!engine._lh_init()) throw Error('WebGL could not open.');
    M = engine;
    open = currentDay();
    M._lh_start(1, open, 0, persisted());
    $('loading').textContent = '';
    $('enter').disabled = $('practice').disabled = false;
    requestAnimationFrame(render);
    if (navigator.xr)
      try {
        xrSupported = await navigator.xr.isSessionSupported('immersive-vr');
        $('xr').hidden = !xrSupported;
      } catch {}
    const seedDoor = Number(
      new URLSearchParams(location.search).get('playtest'),
    );
    if (Number.isInteger(seedDoor) && seedDoor >= 1 && seedDoor <= 24)
      showFieldLog(seedDoor);
    else {
      const atlasDoor = Number(
        new URLSearchParams(location.search).get('atlas'),
      );
      if (Number.isInteger(atlasDoor) && atlasDoor >= 1 && atlasDoor <= 24)
        openAtlas(atlasDoor);
    }
  } catch (e) {
    $('loading').textContent = 'Game could not start · ' + e.message;
    console.error(e);
  }
}
boot();
