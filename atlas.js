export function missionAtlas({ cards, day, modal, esc, engine, back, play }) {
  const $ = (id) => document.getElementById(id);
  modal(
    'MISSION ATLAS',
    `<h2>Floor plans</h2>
    <label>Door <select id="atlasDoor">${cards.map((c) => `<option value="${c.day}">${c.day} · ${esc(c.title)}</option>`).join('')}</select></label>
    <label>Sector <select id="atlasSector"></select></label>
    <p id="atlasReference" class="source"></p>
    <canvas id="atlasPlan" width="420" height="420" style="width:min(100%,420px);height:auto"
      aria-label="Sector floor plan"></canvas>
    <p class="source">IN and OUT mark the entrance and exit. Numbers mark the encounters below. Brighter ground sits higher.</p>
    <label><input type="checkbox" id="atlasOpen"> Show commissioned routes</label>
    <p id="atlasBrief"></p><p id="atlasFeature"></p><ol id="atlasBeats"></ol>
    <div class="buttons"><button class="choice" id="atlasBack">Mission briefing</button>
    ${play ? '<button class="primary" id="atlasPlay">Play this door</button>' : ''}</div>`,
  );
  const door = $('atlasDoor'),
    sector = $('atlasSector'),
    opened = $('atlasOpen');
  door.value = String(day);
  function draw() {
    const card = cards[Number(door.value) - 1],
      area = Number(sector.value);
    const field = card.sectors[area],
      extent = 17 + 4 * card.stride;
    const canvas = $('atlasPlan'),
      ctx = canvas.getContext('2d'),
      scale = canvas.width / extent;
    const raw = document.createElement('canvas');
    raw.width = raw.height = extent;
    const raster = raw.getContext('2d'),
      pixels = raster.createImageData(extent, extent);
    const colours = [
      [28, 44, 46],
      [86, 108, 100],
      [115, 131, 109],
      [133, 100, 65],
      [215, 165, 96],
    ];
    for (let z = 0; z < extent; z++)
      for (let x = 0; x < extent; x++) {
        const value = engine._lh_blueprint(
          card.day,
          area,
          Number(opened.checked),
          x,
          z,
        );
        const surface = value % 8,
          shade = surface ? Math.floor(value / 8) * 2 : 0;
        const i = (z * extent + x) * 4;
        for (let c = 0; c < 3; c++)
          pixels.data[i + c] = colours[surface][c] + shade;
        pixels.data[i + 3] = 255;
      }
    raster.putImageData(pixels, 0, 0);
    ctx.imageSmoothingEnabled = false;
    ctx.drawImage(raw, 0, 0, canvas.width, canvas.height);
    const tags = new Map();
    function tag(anchor, text) {
      tags.set(anchor, [...(tags.get(anchor) || []), text]);
    }
    tag(field.start, 'IN');
    field.encounters.forEach((beat, i) => tag(beat.at, String(i + 1)));
    tag(field.exit, 'OUT');
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.font = 'bold 12px sans-serif';
    for (const [letter, labels] of tags) {
      const n = letter.charCodeAt(0) - 97,
        label = labels.join('·');
      const x = (8.5 + (n % 5) * card.stride) * scale,
        y = (8.5 + Math.floor(n / 5) * card.stride) * scale;
      const w = ctx.measureText(label).width + 9;
      ctx.fillStyle = '#202d34';
      ctx.fillRect(x - w / 2, y - 9, w, 18);
      ctx.fillStyle = '#ffe2a3';
      ctx.fillText(label, x, y);
    }
    canvas.setAttribute('aria-label', field.name + '. ' + field.brief);
    $('atlasReference').textContent = card.reference;
    $('atlasBrief').textContent = field.brief;
    $('atlasFeature').textContent = card.feature + ' · ' + card.featureHint;
    $('atlasBeats').innerHTML = field.encounters
      .map(
        (beat) =>
          '<li><strong>' +
          esc(beat.name) +
          '</strong> · ' +
          esc(beat.instruction) +
          '</li>',
      )
      .join('');
  }
  function selectDoor() {
    sector.innerHTML = cards[Number(door.value) - 1].sectors
      .map(
        (s, i) =>
          '<option value="' +
          i +
          '">' +
          (i + 1) +
          ' · ' +
          esc(s.name) +
          '</option>',
      )
      .join('');
    draw();
  }
  door.onchange = selectDoor;
  sector.onchange = draw;
  opened.onchange = draw;
  $('atlasBack').onclick = () => back(Number(door.value));
  if (play) $('atlasPlay').onclick = () => play(Number(door.value));
  selectDoor();
}
