import { makeGuide } from './xr-guide.js';
/* Column-major inverse of the player's horizontal locomotion transform. */
export function bodyView(x, y, z, yaw) {
  const c = Math.cos(yaw),
    s = Math.sin(yaw);
  return [
    c,
    0,
    -s,
    0,
    0,
    1,
    0,
    0,
    s,
    0,
    c,
    0,
    -c * x - s * z,
    -y,
    s * x - c * z,
    1,
  ];
}
function product(a, b) {
  const out = new Float32Array(16);
  for (let c = 0; c < 4; c++)
    for (let r = 0; r < 4; r++)
      for (let k = 0; k < 4; k++) out[c * 4 + r] += a[k * 4 + r] * b[c * 4 + k];
  return out;
}
export async function immersive(M, update, state, finish, caption, settings) {
  const gl = M.GL.currentContext.GLctx;
  const session = await navigator.xr.requestSession('immersive-vr', {
    optionalFeatures: ['local-floor'],
  });
  let ended = false,
    guide;
  session.addEventListener(
    'end',
    () => {
      ended = true;
      guide?.dispose();
      gl.bindFramebuffer(gl.FRAMEBUFFER, null);
      finish();
    },
    { once: true },
  );
  try {
    await gl.makeXRCompatible();
    const layer = new XRWebGLLayer(session, gl, {
      alpha: false,
      antialias: false,
    });
    session.updateRenderState({
      baseLayer: layer,
      depthNear: 0.055,
      depthFar: 100,
    });
    let space,
      spaceName = 'local-floor',
      offset = 0;
    try {
      space = await session.requestReferenceSpace('local-floor');
    } catch {
      spaceName = 'local';
      space = await session.requestReferenceSpace('local');
      offset = 1.55;
    }
    guide = makeGuide(gl);
    let shown = true,
      previousToggle = false,
      nextCaption = 0;
    session.addEventListener('visibilitychange', () => {
      previous = 0;
      if (session.visibilityState === 'hidden') update(0, false);
    });
    let previous = 0,
      snapped = false;
    function draw(time, frame) {
      if (ended) return;
      let walk = 0,
        side = 0,
        turn = 0,
        bits = 0,
        toggle = false;
      for (const source of session.inputSources) {
        const pad = source.gamepad;
        if (!pad) continue;
        const axes = pad.axes,
          ax = axes.length >= 4 ? axes[2] : axes[0] || 0,
          ay = axes.length >= 4 ? axes[3] : axes[1] || 0;
        if (source.handedness === 'left') {
          walk = -ay;
          side = ax;
        } else turn = ax;
        if (pad.buttons[0]?.pressed) bits |= 2;
        if (pad.buttons[1]?.pressed) bits |= 1;
        if (pad.buttons[4]?.pressed) bits |= 4;
        if (pad.buttons[5]?.pressed) toggle = true;
      }
      if (toggle && !previousToggle) shown = !shown;
      previousToggle = toggle;
      const snapNow = Math.abs(turn) > 0.72 && !snapped;
      if (snapNow) {
        M._lh_aim(0, (Math.sign(turn) * settings.turn * Math.PI) / 180, 0);
        snapped = true;
      }
      if (Math.abs(turn) < 0.3) snapped = false;
      const dt = previous ? (time - previous) / 1000 : 0;
      previous = time;
      const pose =
        session.visibilityState === 'hidden'
          ? null
          : frame.getViewerPose(space);
      if (pose) {
        const q = pose.transform.orientation;
        const dx = -2 * (q.x * q.z + q.w * q.y),
          dy = -2 * (q.y * q.z - q.w * q.x),
          dz = 2 * (q.x * q.x + q.y * q.y) - 1;
        const gaze = Math.atan2(dx, -dz),
          pitch = Math.asin(Math.max(-1, Math.min(1, dy)));
        M._lh_input(
          0,
          settings.speed * (walk * Math.cos(gaze) - side * Math.sin(gaze)),
          settings.speed * (side * Math.cos(gaze) + walk * Math.sin(gaze)),
          0,
          0,
          bits,
        );
        M._lh_point(0, gaze, pitch);
      } else M._lh_input(0, 0, 0, 0, 0, 0);
      const actions =
        bits |
        (Math.abs(walk) + Math.abs(side) > 0.1 ? 8 : 0) |
        (snapNow ? 16 : 0) |
        (toggle ? 32 : 0);
      update(dt, !!pose, {
        space: spaceName,
        actions,
        eyes: pose?.views.length || 0,
      });
      if (!pose) previous = 0;
      if (pose) {
        const s = state(),
          body = bodyView(s[10], s[11] + offset, s[12], s[13]);
        if (shown && time >= nextCaption) {
          guide.update(...caption(s));
          nextCaption = time + 150;
        }
        gl.bindFramebuffer(gl.FRAMEBUFFER, layer.framebuffer);
        for (const view of pose.views) {
          const vp = layer.getViewport(view);
          const transform = product(
            view.projectionMatrix,
            product(view.transform.inverse.matrix, body),
          );
          M.HEAPF32.set(transform, M._lh_matrix() / 4);
          M._lh_xr_eye(0, vp.x, vp.y, vp.width, vp.height);
          if (shown)
            guide.draw(
              product(
                view.projectionMatrix,
                product(view.transform.inverse.matrix, pose.transform.matrix),
              ),
            );
        }
      }
      session.requestAnimationFrame(draw);
    }
    session.requestAnimationFrame(draw);
  } catch (e) {
    await session.end();
    throw e;
  }
  return session;
}
