import { comfortRatings, firstClearMedians } from './playtest.mjs';

export function fieldGuide({
  log,
  cards,
  modal,
  esc,
  start,
  xrSupported,
  initialDay = 1,
  focusId,
}) {
  const $ = (id) => document.getElementById(id);
  const rows = log.rows();
  const time = (seconds) =>
    Math.floor(seconds / 60) +
    'm ' +
    String(Math.floor(seconds % 60)).padStart(2, '0') +
    's';
  const options = cards
    .map(
      (c) =>
        '<option value="' +
        c.day +
        '">' +
        c.day +
        ' · ' +
        esc(c.title) +
        '</option>',
    )
    .join('');
  const results = rows
    .map(
      (r) =>
        (r.id === focusId ? '<details open>' : '<details>') +
        '<summary>Door ' +
        r.door +
        ' · ' +
        esc(r.mode) +
        ' · ' +
        esc(r.status) +
        ' · ' +
        time(r.sectors.reduce((a, b) => a + b, 0)) +
        '</summary><p>' +
        esc(r.edition) +
        ' · ' +
        esc(r.started.slice(0, 16).replace('T', ' ')) +
        ' UTC · ' +
        (r.firstVisit ? 'First visit' : 'Repeat') +
        '</p><p>Sector times · ' +
        r.sectors.map(time).join(' / ') +
        '</p>' +
        (r.xrSeconds
          ? '<p>XR frame callbacks · ' +
            (r.xrFrames / r.xrSeconds).toFixed(1) +
            ' fps average · ' +
            r.xrEyes +
            ' views · ' +
            r.xrSpace +
            ' · ' +
            r.trackingLosses +
            ' tracking interruptions</p><p>Observed controls · ' +
            (['use', 'pulse', 'lift', 'move', 'turn', 'guide']
              .filter((a, i) => r.xrActions & (1 << i))
              .join(', ') || 'none') +
            '</p>'
          : '') +
        '<label>Device or headset <input maxlength="300" data-device value="' +
        esc(r.device) +
        '"></label><label>Comfort <select data-comfort>' +
        comfortRatings
          .map(
            (v) =>
              '<option ' +
              (r.comfort === v ? 'selected' : '') +
              '>' +
              v +
              '</option>',
          )
          .join('') +
        '</select></label>' +
        '<label>Where did you get lost or need help? <textarea maxlength="300" data-note>' +
        esc(r.note) +
        '</textarea></label><button class="choice" data-save="' +
        esc(r.id) +
        '">Save notes</button></details>',
    )
    .join('');
  modal(
    'FIELD TEST',
    '<h2>Playtest log</h2><p>Start at the first sector. Active time and five sector splits are saved on this device. Your regular checkpoint stays available.</p><label>Door <select id="trialday">' +
      options +
      '</select></label><label>Party <select id="trialmode"><option>Solo</option><option>Split screen</option>' +
      (xrSupported ? '<option>WebXR</option>' : '') +
      '</select></label><label><input id="firstvisit" type="checkbox"> Everyone playing is new to this door</label><button class="primary" id="begintrial">Start timed playtest</button><h3>Sessions</h3>' +
      (results || '<p>Your recorded sessions will appear here.</p>') +
      firstClearMedians(rows)
        .map(
          (m) =>
            '<p>Door ' +
            m.label +
            ' · first-clear median ' +
            time(m.seconds) +
            ' (' +
            m.count +
            ' ' +
            (m.count === 1 ? 'party' : 'parties') +
            ')</p>',
        )
        .join('') +
      '<div class="buttons"><button id="exportlog" class="choice">Export JSON</button><button id="importlog" class="choice">Import friends’ results</button><input id="logfile" type="file" accept=".json,application/json" hidden></div><p id="logstatus" role="status"></p>',
  );
  $('trialday').value = String(initialDay);
  if (focusId)
    document
      .querySelector('#panelbody details[open]')
      ?.scrollIntoView({ block: 'nearest' });
  $('begintrial').onclick = () =>
    start(
      Number($('trialday').value),
      $('trialmode').value,
      $('firstvisit').checked,
    );
  for (const button of document.querySelectorAll('[data-save]'))
    button.onclick = () => {
      const d = button.closest('details');
      log.annotate(
        button.dataset.save,
        d.querySelector('[data-device]').value,
        d.querySelector('[data-comfort]').value,
        d.querySelector('[data-note]').value,
      );
      $('logstatus').textContent = 'Notes saved.';
    };
  $('exportlog').onclick = () => {
    const url = URL.createObjectURL(
      new Blob(
        [
          JSON.stringify(
            {
              format: 1,
              runs: log.rows().filter((r) => r.status !== 'In progress'),
            },
            null,
            2,
          ),
        ],
        {
          type: 'application/json',
        },
      ),
    );
    const a = document.createElement('a');
    a.href = url;
    a.download = 'lantern-hall-playtests.json';
    a.click();
    setTimeout(() => URL.revokeObjectURL(url), 1000);
  };
  $('importlog').onclick = () => $('logfile').click();
  $('logfile').onchange = async () => {
    try {
      const file = $('logfile').files[0];
      if (!file) return;
      if (file.size > 2000000) throw Error('File exceeds 2 MB.');
      const count = log.import(JSON.parse(await file.text()));
      fieldGuide({ log, cards, modal, esc, start, xrSupported });
      $('logstatus').textContent = count + ' sessions imported.';
    } catch (error) {
      $('logstatus').textContent = error.message;
    }
  };
}
