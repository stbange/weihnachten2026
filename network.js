/* Two browsers exchange descriptions once. The game then travels over DTLS/SCTP. */
export class Link {
  constructor(onOpen, onMessage, onClose) {
    this.onOpen = onOpen;
    this.onMessage = onMessage;
    this.onClose = onClose;
  }
  reset() {
    if (this.ch) {
      this.ch.onclose = null;
      this.ch.onopen = null;
      this.ch.close();
    }
    if (this.pc) {
      this.pc.onconnectionstatechange = null;
      this.pc.close();
    }
    this.ch = null;
    this.pc = null;
    this.role = '';
  }
  peer(role) {
    this.reset();
    this.role = role;
    const pc = new RTCPeerConnection({
      iceServers: [{ urls: 'stun:stun.l.google.com:19302' }],
    });
    this.pc = pc;
    pc.ondatachannel = (e) => this.channel(e.channel);
    pc.onconnectionstatechange = () => {
      if (pc.connectionState === 'failed') this.lost();
    };
    return pc;
  }
  lost() {
    if (!this.pc) return;
    this.reset();
    this.onClose();
  }
  channel(ch) {
    this.ch = ch;
    ch.onopen = () => this.onOpen(this.role);
    ch.onclose = () => this.lost();
    ch.onmessage = (e) => {
      if (typeof e.data !== 'string' || e.data.length > 4000) return;
      try {
        this.onMessage(JSON.parse(e.data));
      } catch {
        /* Ignore truncated frames. */
      }
    };
  }
  send(msg) {
    if (this.ch?.readyState === 'open' && this.ch.bufferedAmount < 64000)
      this.ch.send(JSON.stringify(msg));
  }
  async gathered(pc) {
    if (pc.iceGatheringState === 'complete') return;
    await new Promise((resolve) => {
      const finish = () => {
        clearTimeout(timeout);
        pc.removeEventListener('icegatheringstatechange', update);
        resolve();
      };
      const update = () => {
        if (pc.iceGatheringState === 'complete') finish();
      };
      const timeout = setTimeout(finish, 6000);
      pc.addEventListener('icegatheringstatechange', update);
      update();
    });
  }
  decode(code, type) {
    if (code.length > 100000) throw Error('This invite is too long.');
    let description;
    try {
      description = JSON.parse(atob(code.trim()));
    } catch {
      throw Error('Paste the full code copied by your friend.');
    }
    if (description?.type !== type || typeof description.sdp !== 'string')
      throw Error('Check whether this is an invitation or a reply.');
    return description;
  }
  async host() {
    const pc = this.peer('host');
    this.channel(pc.createDataChannel('lantern-hall-v1'));
    await pc.setLocalDescription(await pc.createOffer());
    await this.gathered(pc);
    if (!pc.localDescription.sdp.includes('a=candidate:'))
      throw Error(
        'No direct network route was available. Open the WebSocket relay option below.',
      );
    return btoa(JSON.stringify(pc.localDescription.toJSON()));
  }
  async join(code) {
    const offer = this.decode(code, 'offer'),
      pc = this.peer('guest');
    await pc.setRemoteDescription(offer);
    await pc.setLocalDescription(await pc.createAnswer());
    await this.gathered(pc);
    return btoa(JSON.stringify(pc.localDescription.toJSON()));
  }
  async accept(code) {
    if (this.role !== 'host' || !this.pc)
      throw Error('Create an invitation first.');
    await this.pc.setRemoteDescription(this.decode(code, 'answer'));
  }
}
