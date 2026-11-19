/* Optional TLS WebSocket transport for networks where direct ICE is unavailable. */
export class Relay {
  constructor(ready, message, closed) {
    this.ready = ready;
    this.message = message;
    this.closed = closed;
  }
  reset() {
    const ws = this.ws;
    this.ws = null;
    if (ws) {
      ws.onclose = null;
      ws.onerror = null;
      ws.close();
    }
  }
  open(address, code, update) {
    this.reset();
    const url = new URL(address);
    const loopback = ['localhost', '127.0.0.1', '[::1]'].includes(url.hostname);
    if (url.protocol !== 'wss:' && !(url.protocol === 'ws:' && loopback))
      throw Error(
        'Use a wss:// relay address, or ws://localhost for local testing.',
      );
    const ws = new WebSocket(url);
    this.ws = ws;
    let connected = false;
    ws.onopen = () =>
      ws.send(
        JSON.stringify(code ? { join: code.toUpperCase() } : { create: true }),
      );
    ws.onmessage = (e) => {
      if (typeof e.data !== 'string' || e.data.length > 4096) return;
      let msg;
      try {
        msg = JSON.parse(e.data);
      } catch {
        return;
      }
      if (Array.isArray(msg)) {
        if (connected) this.message(msg);
        return;
      }
      if (msg.room)
        update(
          'Share the relay address and room code with your friend.',
          msg.room,
        );
      if (msg.error) update(msg.error, '');
      if (msg.ready === 'host' || msg.ready === 'guest') {
        connected = true;
        this.ready(msg.ready);
      }
    };
    ws.onerror = () =>
      update(
        'Could not reach this relay. Check its address and certificate.',
        '',
      );
    ws.onclose = () => {
      if (this.ws === ws) {
        this.ws = null;
        if (connected) this.closed();
      }
    };
  }
  send(value) {
    if (
      this.ws?.readyState === WebSocket.OPEN &&
      this.ws.bufferedAmount < 64000
    )
      this.ws.send(JSON.stringify(value));
  }
}
