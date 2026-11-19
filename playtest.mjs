/* Local, opt-in field notes. One key per run lets two tabs keep independent sessions. */
const prefix = 'lantern-hall.field-notes.v1.';
export const modes = ['Solo', 'Split screen', 'Network host', 'WebXR'];
export const comfortRatings = [
  'Unrated',
  'Comfortable',
  'Mild discomfort',
  'Stopped for discomfort',
];
export const edition = 'floorplans-1';
function valid(row) {
  return (
    row &&
    typeof row.id === 'string' &&
    /^[a-zA-Z0-9-]{1,64}$/.test(row.id) &&
    [edition, 'chambers-1'].includes(row.edition) &&
    Number.isInteger(row.door) &&
    row.door >= 1 &&
    row.door <= 24 &&
    modes.includes(row.mode) &&
    typeof row.firstVisit === 'boolean' &&
    ['In progress', 'Stopped', 'Completed'].includes(row.status) &&
    Array.isArray(row.sectors) &&
    row.sectors.length === 5 &&
    row.sectors.every((n) => Number.isFinite(n) && n >= 0) &&
    typeof row.started === 'string' &&
    Number.isFinite(Date.parse(row.started)) &&
    comfortRatings.includes(row.comfort) &&
    ['device', 'note'].every(
      (k) => typeof row[k] === 'string' && row[k].length <= 300,
    ) &&
    Number.isInteger(row.xrFrames) &&
    row.xrFrames >= 0 &&
    Number.isFinite(row.xrSeconds) &&
    row.xrSeconds >= 0 &&
    Number.isInteger(row.xrActions) &&
    row.xrActions >= 0 &&
    row.xrActions <= 63 &&
    ['', 'local', 'local-floor'].includes(row.xrSpace) &&
    Number.isInteger(row.xrEyes) &&
    row.xrEyes >= 0 &&
    row.xrEyes <= 2 &&
    Number.isInteger(row.trackingLosses) &&
    row.trackingLosses >= 0
  );
}
export function firstClearMedians(rows) {
  const groups = new Map();
  for (const r of rows.filter(
    (r) => r.edition === edition && r.firstVisit && r.status === 'Completed',
  )) {
    const key = r.door + ' / ' + r.mode;
    if (!groups.has(key)) groups.set(key, []);
    groups.get(key).push(r.sectors.reduce((a, b) => a + b, 0));
  }
  return Array.from(groups, ([label, ts]) => {
    ts.sort((a, b) => a - b);
    return {
      label,
      count: ts.length,
      seconds: (ts[(ts.length - 1) >> 1] + ts[ts.length >> 1]) / 2,
    };
  });
}
export class PlaytestLog {
  constructor(storage) {
    this.storage = storage;
    this.memory = new Map();
    this.active = null;
    this.running = false;
  }
  rows() {
    const rows = new Map();
    try {
      for (let i = 0; i < this.storage.length; i++) {
        const key = this.storage.key(i);
        if (key?.startsWith(prefix)) {
          try {
            const row = JSON.parse(this.storage.getItem(key));
            if (valid(row)) rows.set(row.id, row);
          } catch {}
        }
      }
    } catch {}
    for (const [id, row] of this.memory) rows.set(id, row);
    return Array.from(rows.values()).sort((a, b) =>
      b.started.localeCompare(a.started),
    );
  }
  save(r) {
    this.memory.set(r.id, r);
    try {
      this.storage.setItem(prefix + r.id, JSON.stringify(r));
    } catch {}
  }
  begin(door, mode, firstVisit, now) {
    this.end('Stopped', now);
    this.active = {
      id:
        crypto.randomUUID?.() ||
        Array.from(crypto.getRandomValues(new Uint8Array(16)), (v) =>
          v.toString(16).padStart(2, '0'),
        ).join(''),
      edition,
      door,
      mode,
      firstVisit,
      status: 'In progress',
      started: new Date().toISOString(),
      sectors: [0, 0, 0, 0, 0],
      device: '',
      note: '',
      comfort: 'Unrated',
      xrFrames: 0,
      xrSeconds: 0,
      xrActions: 0,
      xrSpace: '',
      xrEyes: 0,
      trackingLosses: 0,
    };
    this.wasTracked = false;
    this.last = this.saved = now;
    this.area = 0;
    this.running = true;
    this.save(this.active);
  }
  sample(now, area, running) {
    if (!this.active) return;
    if (this.running)
      this.active.sectors[this.area] += Math.max(0, now - this.last) / 1000;
    this.last = now;
    this.running = running;
    if (area !== this.area || now - this.saved >= 10000) {
      this.save(this.active);
      this.saved = now;
    }
    this.area = area;
  }
  pause(now) {
    this.sample(now, this.area, false);
  }
  xrFrame(dt, tracked, device) {
    const r = this.active;
    if (!r || r.mode !== 'WebXR') return;
    if (!tracked && this.wasTracked) r.trackingLosses++;
    this.wasTracked = tracked;
    if (!tracked || !device) return;
    r.xrActions |= device.actions;
    r.xrEyes = Math.max(r.xrEyes, device.eyes);
    r.xrSpace = device.space;
    if (dt > 0 && this.running) {
      r.xrFrames++;
      r.xrSeconds += dt;
    }
  }
  end(status, now) {
    if (!this.active) return;
    this.pause(now);
    this.active.status = status;
    this.save(this.active);
    this.active = null;
  }
  annotate(id, device, comfort, note) {
    const row = this.rows().find((r) => r.id === id);
    if (!row || !comfortRatings.includes(comfort)) return;
    row.device = device.slice(0, 300);
    row.note = note.slice(0, 300);
    row.comfort = comfort;
    this.save(row);
  }
  import(payload) {
    if (
      payload?.format !== 1 ||
      !Array.isArray(payload.runs) ||
      payload.runs.length > 1000 ||
      !payload.runs.every(valid)
    )
      throw Error('Choose a Lantern Hall playtest export from this edition.');
    const ids = new Set(this.rows().map((r) => r.id));
    let added = 0;
    for (const row of payload.runs)
      if (!ids.has(row.id)) {
        this.save(row);
        ids.add(row.id);
        added++;
      }
    return added;
  }
}
