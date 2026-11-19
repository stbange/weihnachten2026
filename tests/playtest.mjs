import assert from 'node:assert/strict';
import { PlaytestLog, firstClearMedians } from '../web/playtest.mjs';
const records = new Map();
const storage = {
  get length() {
    return records.size;
  },
  key(i) {
    return [...records.keys()][i];
  },
  getItem(k) {
    return records.get(k);
  },
  setItem(k, v) {
    records.set(k, v);
  },
};
const log = new PlaytestLog(storage);
log.begin(1, 'Solo', true, 0);
log.sample(60000, 1, true);
log.pause(62000);
log.sample(900000, 1, true);
log.sample(930000, 2, true);
log.sample(990000, 3, true);
log.sample(1050000, 4, true);
log.end('Completed', 1110000);
const [first] = log.rows();
assert.deepEqual(first.sectors, [60, 32, 60, 60, 60]);
assert.equal(first.status, 'Completed');
log.annotate(
  first.id,
  'Laptop',
  'Comfortable',
  'The booth switch was easy to see.',
);
assert.equal(new PlaytestLog(storage).rows()[0].comfort, 'Comfortable');
log.begin(1, 'Solo', false, 0);
log.end('Completed', 300000);
log.begin(1, 'Solo', true, 0);
log.end('Stopped', 360000);
log.begin(1, 'WebXR', true, 0);
log.xrFrame(1 / 72, true, { space: 'local', actions: 3, eyes: 2 });
log.xrFrame(0, false);
log.end('Completed', 120000);
assert.equal(log.rows().find((r) => r.mode === 'WebXR').xrFrames, 1);
assert.deepEqual(
  firstClearMedians(log.rows()).find((r) => r.label === '1 / Solo'),
  { label: '1 / Solo', count: 1, seconds: 272 },
);
const payload = { format: 1, runs: log.rows() };
const other = new PlaytestLog(null);
assert.equal(other.import(payload), 4);
assert.equal(other.import(payload), 0);
assert.throws(() =>
  other.import({ format: 1, runs: [{ ...first, sectors: [-1] }] }),
);
const legacy = { ...first, id: 'prior-layout', edition: 'chambers-1' };
assert.equal(other.import({ format: 1, runs: [legacy] }), 1);
assert.equal(
  firstClearMedians(other.rows()).find((r) => r.label === '1 / Solo').count,
  1,
);
assert.equal(other.rows().length, 5);
console.log(
  'Field timings, paused intervals, first-clear populations, notes and imports passed.',
);
