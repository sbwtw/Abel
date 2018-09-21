const Promise = require('bluebird')
const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const EventEmitter = require('events')

const COMMAND_CONNECT = 0x55
const COMMAND_PING = 0x30
const COMMAND_VERSION = 0x31
const COMMAND_STATION_ID = 0x32
const COMMAND_STATION_STATUS = 0x33
const COMMAND_BINDING_PROGRESS = 0x34

/* validate response data */
const validate = (data) => {
  /* no data */
  if (!data || !data.length) return false
  /* wrong length */
  if (data[1] !== data.length) return false
  /* check sum */
  let sum = -data[2]
  data.slice(3, data.length).forEach(a => (sum += a))
  return !(sum % 256)
}

/* calculate checksum */
const checksum = (arr) => {
  let sum = 0x00
  arr.forEach(a => (sum += a))
  return sum % 256
}

/* available events: SPS_DATA, MODE, [COMMAND*] */
class Bled extends EventEmitter {
  constructor (devPort) {
    super()
    this.port = new SerialPort(devPort, { baudRate: 115200 })

    this.parser = this.port.pipe(new Readline({ delimiter: '\0\0', encoding: '' }))

    this.parser.on('data', (data) => {
      console.log('uart receive raw data', data)
      /* no data */
      if (!data || !data.length) return
      /* data does not starts with 0x00: SPS data */
      if (data[0] !== 0) {
        this.emit('SPS_DATA', data)
      } else if (data.toString('hex') === '00cc') {
        console.log('BLE is in Bootloader mode, need flash firmware')
        this.emit('MODE', 'sbl')
      } else if (data.toString('hex') === '00aa') {
        console.log('BLE is in application mode')
        this.emit('MODE', 'app')
      } else if (data.length === 3 && data[1] === data[2] && [0x20, 0x21, 0x22, 0x23].includes(data[2])) { // BLE status
        switch (data[2]) {
          case 0x20:
            console.log('BLE init')
            this.emit('BLE_INIT')
            break
          case 0x21:
            console.log('BLE start advertising')
            this.emit('BLE_ADVERTISING')
            break
          case 0x22:
            console.log('BLE connect to client')
            this.emit('BLE_CONNECTED_CLIENT')
            break
          case 0x23:
            console.log('BLE disconnected with client')
            this.emit('BLE_DISCONNECTED_CLIENT')
            break
          default:
            break
        }
      } else if (data.length > 4 && validate(data)) { // check data, and resovle data from BLE
        switch (data[3]) {
          case COMMAND_PING:
            this.emit(COMMAND_PING, data)
            break
          case COMMAND_VERSION:
            this.emit(COMMAND_VERSION, data)
            break
          case COMMAND_STATION_ID:
            this.emit(COMMAND_STATION_ID, data)
            break
          case COMMAND_STATION_STATUS:
            this.emit(COMMAND_STATION_STATUS, data)
            break
          case COMMAND_BINDING_PROGRESS:
            this.emit(COMMAND_BINDING_PROGRESS, data)
            break
          default:
            break
        }
      } else console.warn('Get unknown data', data)
    })

    this.dataArray = []

    this.on('SPS_DATA', (data) => {
      this.dataArray.push(data)
      if (data.length && data[data.length - 1] === 0x0A) { // data end with \n
        const pack = Buffer.concat(this.dataArray).toString().trim()
        console.log('Get pack:', pack)
        if (pack === 'come') this.emit('CMD_COME', pack)
        this.dataArray.length = 0
      }
    })
  }

  init () {
    this.session = Math.floor(Math.random() * 4294967295 + 1).toString(16)
    this.pingCount = 0
    this.writeQuene = []
    this.state = 'Idle'
  }

  schedule () {
    // console.log('schedule', this.writeQuene.length, this.state)
    if (!this.writeQuene.length || this.state !== 'Idle') return
    this.state = 'Writing'
    const { msg, cb } = this.writeQuene.shift()
    this.port.write(msg)

    /* prevent message from being combined to one data */
    this.port.drain((error) => {
      cb(error)
      this.state = 'Idle'
      this.schedule()
    })
  }

  writebByQuene (msg, cb) {
    this.writeQuene.push({ msg, cb })
    this.schedule()
  }

  /* send cmd to BLE */
  write (cmd, msg, cb) {
    const timer = setTimeout(() => {
      this.port.removeAllListeners(cmd)
      console.log('timeout write', cmd, msg)
      const e = new Error('ETIMEOUT')
      cb(e)
    }, 1000)

    this.once(cmd, (data) => {
      clearTimeout(timer)
      if (cmd === 'MODE') cb(null, data)
      else cb(null, data.slice(4, data.length))
    })

    this.writebByQuene(msg, (err) => {
      if (err) {
        console.log('Error on write: ', err.message)
        cb(err)
      }
    })
  }

  async writeAsync (cmd, msg) {
    return Promise.promisify(this.write).bind(this)(cmd, msg)
  }

  connect (cb) {
    this.write('MODE', Buffer.from([COMMAND_CONNECT, COMMAND_CONNECT]), cb)
  }

  async connectAsync () {
    return Promise.promisify(this.connect).bind(this)()
  }

  ping (cb) {
    const buf = Buffer.concat([Buffer.alloc(4), Buffer.from(this.session, 'hex')])
    buf[0] = 0x00
    buf[1] = 0x08
    buf[3] = COMMAND_PING
    buf[2] = checksum(buf.slice(3))
    this.write(COMMAND_PING, buf, cb)
  }

  async pingAsync () {
    return Promise.promisify(this.ping).bind(this)()
  }

  heartbeat () {
    this.pingCount += 1
    console.log('pingCount', this.pingCount)
    this.pingAsync().then(() => setTimeout(() => this.heartbeat(), 0)).catch(e => e && console.error('heartbeat error', e))
  }

  getVersion (cb) {
    this.write(COMMAND_VERSION, Buffer.from([0x00, 0x04, COMMAND_VERSION, COMMAND_VERSION]), cb)
  }

  async getVersionAsync () {
    return Promise.promisify(this.getVersion).bind(this)()
  }

  setStationId (id, cb) {
    if (!Buffer.isBuffer(id)) {
      const e = new Error('Id not Buffer')
      cb(e)
    } else {
      const size = id.length + 4
      const buf = Buffer.concat([Buffer.alloc(4), id])
      buf[0] = 0x00
      buf[1] = size
      buf[3] = COMMAND_STATION_ID
      buf[2] = checksum(buf.slice(3))
      this.write(COMMAND_STATION_ID, buf, cb)
    }
  }

  async setStationIdAsync (id) {
    return Promise.promisify(this.setStationId).bind(this)(id)
  }

  /* unint8_t status */
  setStationStatus (status, cb) {
    const buf = Buffer.alloc(5)
    buf[0] = 0x00
    buf[1] = 5
    buf[3] = COMMAND_STATION_STATUS
    buf[4] = status
    buf[2] = checksum(buf.slice(3))
    this.write(COMMAND_STATION_STATUS, buf, cb)
  }

  async setStationStatusAsync (status) {
    return Promise.promisify(this.setStationStatus).bind(this)(status)
  }

  sendMsg (msg, cb) {
    this.writebByQuene(`${JSON.stringify(msg)}\n`, cb)
  }

  async sendMsgAsync (msg) {
    return Promise.promisify(this.sendMsg).bind(this)(msg)
  }
}

const res = {
  session: 'session-blabla',
  op: 'binding',
  awsalb: 'awsalb-blabla',
  essid: 'wifi-essid',
  password: 'wifi-password',
  csr: 'csr-blabla-loooooooog-wooooooooooooord'
}

const DEV_PORT = '/dev/ttyACM0'

const initAsync = async () => {
  const bled = new Bled(DEV_PORT)
  bled.init()
  // const mode = await bled.connectAsync()
  // if (mode === 'sbl') return false
  // mode === 'app'
  bled.heartbeat()

  bled.on('CMD_COME', pack => bled.sendMsg(res, e => e && console.error('send message via SPS error', e)))

  const version = await bled.getVersionAsync()
  console.log('BLE Version:', version)
  await bled.setStationIdAsync(Buffer.from('stationid123'))
  console.log('Update station id success')
  await bled.setStationStatusAsync(0x01)
  console.log('Update station status success')
  return true
}

initAsync().then((isApp) => {
  if (isApp) {
    console.log('\ninit success!')
  } else process.exit(0)
}).catch((e) => {
  console.log('\ninit failed', e)
  process.exit(1)
})
