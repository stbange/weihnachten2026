import { Link } from './network.js';
import { Relay } from './relay.js';
import { immersive } from './xr.js';
const $ = (id) => document.getElementById(id),
  stage = $('stage'),
  canvas = $('canvas'),
  dialog = $('panel'),
  body = $('panelbody');
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
  Array.from(M.HEAPF32.subarray(M._lh_state() / 4, M._lh_wire() / 4 + 22));
const currentDay = () => M._lh_today(Date.now() / 1000);
const persisted = () => {
  try {
    return localStorage.getItem('lantern-hall.ticket.v1') === '1' ? 1 : 0;
  } catch {
    return 0;
  }
};
const persist = () => {
  try {
    localStorage.setItem('lantern-hall.ticket.v1', '1');
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
) {
  if (!M) return;
  if (!online) {
    link.reset();
    relay.reset();
  }
  remote = { v: [0, 0, 0, 0, 0], at: 0, pulses: 0 };
  lastSend = 0;
  role = online;
  split = localSplit;
  focusSeat = role === 'guest' ? 1 : 0;
  open = currentDay();
  if (dialog.open) closePanel();
  M._lh_start(split || role ? 2 : 1, open, rehearsal ? 1 : 0, persisted());
  locationUI(true);
  canvas.focus();
  chime([294, 370, 440]);
  last = 0;
  if (lock && !online && !coarse)
    canvas.requestPointerLock?.()?.catch?.(() => {});
}
function home() {
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
function almanac() {
  if (!cards.length) return;
  const count = M ? (role === 'guest' ? state()[2] : currentDay()) : 0;
  const grid = cards
    .map(
      (c) =>
        `<button class="day ${c.day <= count ? 'ready' : ''}" data-day="${c.day}" aria-label="Door ${c.day}, ${esc(c.title)}"><strong>${String(c.day).padStart(2, '0')}</strong><small>${c.day <= count ? 'open' : 'sealed'}</small></button>`,
    )
    .join('');
  modal(
    'PORTAL ALMANAC',
    `<h2>Twenty-four evenings</h2><p>One door opens at midnight in Warsaw each day, December 1–24. Every mission briefing is available to read now.</p><div class="grid">${grid}</div><a class="booklink" href="${sources}design/lantern-hall.md" target="_blank" rel="noopener">Read the campaign design →</a> &nbsp; <a class="booklink" href="${sources}ART-CREDITS.md" target="_blank" rel="noopener">Art credits</a>`,
  );
  body
    .querySelectorAll('[data-day]')
    .forEach((b) => (b.onclick = () => briefing(+b.dataset.day)));
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
    `<h2>${esc(c.title)}</h2><div class="source">${esc(c.reference)}</div><p>${esc(c.mission)}</p><ul>${c.kit.map((s) => '<li>' + esc(s) + '</li>').join('')}</ul><details><summary>Twenty-minute mission design</summary><p>${esc(c.place)}</p><p>${esc(c.route)}</p><p>${esc(c.coop)}</p></details><div class="buttons"><button id="backbook" class="choice">All 24 doors</button>${day === 1 && !role ? '<button id="trydoor" class="primary">Rehearse Door 01</button>' : ''}</div><p class="source">${day === 1 ? 'A short playable rehearsal is available.' : 'Campaign design · full mission in development.'}</p>`,
  );
  $('backbook').onclick = almanac;
  if ($('trydoor')) $('trydoor').onclick = () => start(true, split);
}
function controlsText() {
  return '<p><b>Player 1</b> · WASD move, mouse or arrows look, F or click pulses, E uses, Space lifts. Drag to look if your browser releases the mouse.</p><p><b>Player 2</b> · IJKL move, U/O turn, Y pulses, H uses, N lifts. On a touchscreen the first seat uses the on-screen controls.</p>';
}
function menu() {
  modal(
    'LANTERN HALL',
    `<h2>At the landing</h2><div class="buttons"><button id="resume" class="primary">Resume</button><button id="browse" class="choice">Portal almanac</button><button id="leave" class="choice">Return to title</button></div><details open><summary>Controls</summary>${controlsText()}</details>${!role && !split && xrSupported ? '<button id="xrhere" class="choice">Enter VR</button>' : ''}`,
  );
  $('resume').onclick = closePanel;
  $('browse').onclick = almanac;
  $('leave').onclick = home;
  if ($('xrhere')) $('xrhere').onclick = goVR;
}
$('enter').onclick = () => start();
$('practice').onclick = () => start(true);
$('featured').onclick = () => briefing(1);
$('book').onclick = almanac;
$('menu').onclick = menu;
function connected(newRole) {
  start(friendsRehearse, false, newRole);
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
  } else if (role === 'guest' && msg.length === 22) {
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
let transport = link;
function multiplayer() {
  if (!M) return;
  modal(
    'MULTIPLAYER',
    `<h2>Pull up a chair</h2><p>Choose where to meet, then share a screen or connect two browsers.</p><label><input id="meetpractice" type="checkbox" ${friendsRehearse ? 'checked' : ''}> Meet in the Door 01 rehearsal</label><div class="buttons"><button id="splits" class="primary">Start split screen</button><button id="net" class="choice">Connect browsers</button></div><details><summary>Two-player controls</summary>${controlsText()}</details>`,
  );
  $('meetpractice').onchange = (e) => (friendsRehearse = e.target.checked);
  $('splits').onclick = () => start(friendsRehearse, true);
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
    k('KeyE') | (k('KeyF') * 2) | (k('Space') * 4) | touch.bits | taps[0],
  ];
}
const tips = [
  '',
  'Paper usher folded',
  'The projector is running',
  'Ticket returned to the hall',
  '',
  'Entered House Lights',
  'This portal is sealed',
  'Rehearsal complete · ticket returned',
];
function events() {
  const event = M._lh_event();
  if (!event) return;
  const s = state();
  if (event === 3 || event === 7) {
    if (event === 3 && s[6]) persist();
    chime([392, 494, 587, 784]);
  }
  if (event === 2 || event === 1)
    chime(event === 1 ? [523, 659] : [330, 440, 554]);
  if ((event === 4 || event === 6) && !role && !xrActive) {
    briefing(s[8]);
    return;
  }
  if (tips[event]) notify(tips[event]);
}
function hud(s) {
  const near = Array.from({ length: split ? 2 : 1 }, (_, i) =>
    M._lh_near(split ? i : focusSeat),
  );
  const hash = [
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
    ? s[3]
      ? 'Door 01 · rehearsal'
      : 'Door 01 · live calendar'
    : 'Lantern Hall · ' + s[2] + '/24 open';
  document.querySelectorAll('.viewinfo').forEach((info, i) => {
    const nearDay = near[i],
      p = split ? i : focusSeat,
      n = (s[4] & 1) + ((s[4] >> 1) & 1);
    const label = split
      ? 'PLAYER ' + (i + 1)
      : s[1]
        ? 'HOUSE LIGHTS'
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
          ' · Use for briefing'
        : coarse
          ? 'Left stick moves · drag right to look · tap Use'
          : 'WASD moves · mouse or arrows look · E inspects';
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
let xrSupported = false;
async function goVR() {
  if (!M || role || split) return;
  if (!playing) start(false, false, '', false);
  if (dialog.open) closePanel();
  xrActive = true;
  try {
    await immersive(
      M,
      (dt) => {
        M._lh_tick(dt, 1);
        events();
      },
      state,
      () => {
        xrActive = false;
        resetInput();
      },
    );
  } catch (e) {
    xrActive = false;
    notify('VR session could not start · ' + e.message);
  }
}
$('xr').onclick = goVR;
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
  const factor = Math.min(1, 840 / Math.max(innerWidth, innerHeight));
  M._lh_resize(
    Math.max(160, Math.round(innerWidth * factor)),
    Math.max(140, Math.round(innerHeight * factor)),
  );
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
  } catch (e) {
    $('loading').textContent = 'Game could not start · ' + e.message;
    console.error(e);
  }
}
boot();
