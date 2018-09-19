const Promise = require('bluebird')
const SerialPort = require('serialport')
const EventEmitter = require('events')

const COMMAND_CONNECT = 0x55
const COMMAND_PING = 0x30
const COMMAND_VERSION = 0x31
const COMMAND_STATION_ID = 0x32
const COMMAND_STATION_STATUS = 0x33
const COMMAND_BINDING_PROGRESS = 0x34

/* TODO */
const DEV_PORT = '/dev/ttyACM0'

const port = new SerialPort(DEV_PORT, {
  baudRate: 115200
})

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

/* available events: SPS_DATA, MODE, [COMMAND*] */
const dataParse = new EventEmitter()

port.on('data', (data) => {
  console.log('on data', data)
  /* no data */
  if (!data || !data.length) return
  /* data does not starts with 0x00: SPS data */
  if (data[0] !== 0) {
    dataParse.emit('SPS_DATA', data)
  } else if (data.toString('hex') === '00cc') {
    console.log('BLE is in Bootloader mode, need flash firmware')
    dataParse.emit('MODE', 'sbl')
  } else if (data.toString('hex') === '00aa') {
    console.log('BLE is in application mode')
    dataParse.emit('MODE', 'app')
  } else if (data.length > 4 && validate(data)) { // check data, and resovle data from BLE
    switch (data[3]) {
      case COMMAND_PING:
        dataParse.emit(COMMAND_PING, data)
        break
      case COMMAND_VERSION:
        dataParse.emit(COMMAND_VERSION, data)
        break
      case COMMAND_STATION_ID:
        dataParse.emit(COMMAND_STATION_ID, data)
        break
      case COMMAND_STATION_STATUS:
        dataParse.emit(COMMAND_STATION_STATUS, data)
        break
      case COMMAND_BINDING_PROGRESS:
        dataParse.emit(COMMAND_BINDING_PROGRESS, data)
        break
      default:
        break
    }
  } else console.warn('Get unknown data', data)
})

/* send cmd to BLE */
const write = (cmd, msg, cb) => {
  const timer = setTimeout(() => {
    port.removeAllListeners(cmd)
    console.log('timeout write', cmd, msg)
    const e = new Error('ETIMEOUT')
    cb(e)
  }, 1000)

  dataParse.once(cmd, (data) => {
    clearTimeout(timer)
    if (cmd === 'MODE') cb(null, data)
    else cb(null, data.slice(4, data.length))
  })

  port.write(msg, (err) => {
    if (err) {
      console.log('Error on write: ', err.message)
      cb(err)
    }
  })
}

const writeAsync = Promise.promisify(write)

const cmdConnect = async () => {
  const mode = await writeAsync('MODE', Buffer.from([COMMAND_CONNECT, COMMAND_CONNECT]))
  return mode
}

/* calculate checksum */
const checksum = (arr) => {
  let sum = 0x00
  arr.forEach(a => (sum += a))
  return sum % 256
}

const session = Math.floor(Math.random() * 4294967295 + 1).toString(16)

const cmdPing = async () => {
  const buf = Buffer.concat([Buffer.alloc(4), Buffer.from(session, 'hex')])
  buf[0] = 0x00
  buf[1] = 0x08
  buf[3] = COMMAND_PING
  buf[2] = checksum(buf.slice(3))
  console.log('Ping buf', buf)
  await writeAsync(COMMAND_PING, buf)
}

let pingCount = 0
const heartbeat = () => {
  pingCount += 1
  console.log('pingCount', pingCount)
  cmdPing().then(() => setTimeout(() => heartbeat(), 1000)).catch(e => e && console.error('heartbeat error', e))
}

const cmdVersion = async () => {
  const version = await writeAsync(COMMAND_VERSION, Buffer.from([0x00, 0x04, COMMAND_VERSION, COMMAND_VERSION]))
  return version
}

/* update id */
const cmdStationId = async (id) => {
  // assert Buffer.isBuffer(id) === true
  const size = id.length + 4
  const buf = Buffer.concat([Buffer.alloc(4), id])
  buf[0] = 0x00
  buf[1] = size
  buf[3] = COMMAND_STATION_ID
  buf[2] = checksum(buf.slice(3))
  await writeAsync(COMMAND_STATION_ID, buf)
}

/* unint8_t status */
const cmdStationStatus = async (status) => {
  // assert Buffer.isBuffer(id) === true
  const buf = Buffer.alloc(5)
  buf[0] = 0x00
  buf[1] = 5
  buf[3] = COMMAND_STATION_STATUS
  buf[4] = status
  buf[2] = checksum(buf.slice(3))
  await writeAsync(COMMAND_STATION_STATUS, buf)
}

const initAsync = async () => {
  const mode = await cmdConnect()
  if (mode === 'sbl') return false
  // mode === 'app'
  const version = await cmdVersion()
  console.log('BLE Version:', version)
  await cmdStationId(Buffer.from('stationid123'))
  console.log('Update station id success')
  await cmdStationStatus(0x01)
  console.log('Update station status success')
  return true
}

initAsync().then((isApp) => {
  if (isApp) {
    console.log('\ninit success!')
    heartbeat()
  } else process.exit(0)
}).catch((e) => {
  console.log('\ninit failed', e)
  process.exit(1)
})
