/* Stereo text panel at reading distance. The ordinary SDL scene is drawn first. */
export function makeGuide(gl) {
  const canvas = document.createElement('canvas');
  canvas.width = 768;
  canvas.height = 158;
  const ctx = canvas.getContext('2d');
  const program = gl.createProgram();
  const shaders = [];
  let buffer, texture;
  function dispose() {
    gl.deleteTexture(texture);
    gl.deleteBuffer(buffer);
    gl.deleteProgram(program);
    shaders.forEach((s) => gl.deleteShader(s));
  }
  try {
    for (const [type, source] of [
      [
        gl.VERTEX_SHADER,
        'attribute vec2 xy; uniform mat4 eye; varying vec2 uv; void main(){uv=vec2(xy.x,1.-xy.y);gl_Position=eye*vec4((xy.x-.5)*1.46,(xy.y-.5)*.30-.49,-1.9,1.);}',
      ],
      [
        gl.FRAGMENT_SHADER,
        'precision mediump float; varying vec2 uv; uniform sampler2D label; void main(){gl_FragColor=texture2D(label,uv);}',
      ],
    ]) {
      const shader = gl.createShader(type);
      shaders.push(shader);
      gl.shaderSource(shader, source);
      gl.compileShader(shader);
      gl.attachShader(program, shader);
    }
    gl.linkProgram(program);
    if (!gl.getProgramParameter(program, gl.LINK_STATUS))
      throw Error('Could not build the stereo guide.');
    const xy = gl.getAttribLocation(program, 'xy'),
      eye = gl.getUniformLocation(program, 'eye');
    buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.bufferData(
      gl.ARRAY_BUFFER,
      new Float32Array([0, 0, 1, 0, 0, 1, 1, 1]),
      gl.STATIC_DRAW,
    );
    texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    let previous = '';
    return {
      dispose,
      update(heading, instruction) {
        const text = heading + '\n' + instruction;
        if (text === previous) return;
        previous = text;
        ctx.fillStyle = '#17302c';
        ctx.fillRect(0, 0, 768, 158);
        ctx.fillStyle = '#edc880';
        ctx.font = 'bold 25px sans-serif';
        ctx.fillText(heading, 19, 33, 730);
        ctx.fillStyle = '#fff4d5';
        ctx.font = '25px sans-serif';
        const lines = [];
        let line = '';
        for (const word of instruction.split(/\s+/)) {
          if (line && ctx.measureText(line + ' ' + word).width > 730) {
            lines.push(line);
            line = '';
          }
          line += (line ? ' ' : '') + word;
        }
        lines.push(line);
        lines
          .slice(0, 3)
          .forEach((l, i) => ctx.fillText(l, 19, 69 + i * 34, 730));
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.texImage2D(
          gl.TEXTURE_2D,
          0,
          gl.RGBA,
          gl.RGBA,
          gl.UNSIGNED_BYTE,
          canvas,
        );
      },
      draw(matrix) {
        gl.useProgram(program);
        gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
        gl.enableVertexAttribArray(xy);
        gl.vertexAttribPointer(xy, 2, gl.FLOAT, false, 0, 0);
        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.uniformMatrix4fv(eye, false, matrix);
        gl.disable(gl.DEPTH_TEST);
        gl.depthMask(false);
        gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
        gl.depthMask(true);
        gl.enable(gl.DEPTH_TEST);
        gl.disableVertexAttribArray(xy);
      },
    };
  } catch (error) {
    dispose();
    throw error;
  }
}
