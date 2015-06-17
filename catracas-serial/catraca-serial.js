var com          = require("serialport"),
    S            = require('string'),
    EventEmitter = require('events').EventEmitter,
    emitter      = new EventEmitter();

var serialPort = new com.SerialPort("/dev/ttyACM0", {
  baudrate: 9600,
  parser: com.parsers.readline('\r')
});

serialPort.on('open',function() {
  console.log('Connected to ' + this.path + " and listening...");
});

serialPort.on('data', function(data) {
  if ( S(data).contains('VERIFY-UID:')) {
    emitter.emit('verify', cut_uid_from_raw_data(data));
    emitter.emit('response', "print<Hey bitchs>");
  };
  console.log(data);
});

serialPort.on('close',function() {
  console.log('Device disconected and the port is closed');
});

serialPort.on('error',function(err) {
  console.log('Error [' + new Date().toString() + "]:" + err);
});

//Returns a string
function cut_uid_from_raw_data(uid) {
  var pos = uid.indexOf(':');
  return uid.substring(pos + 1, uid.length);
}

// emmiter.on('', function (argument) {
// });

emitter.on('verify', function (uid) {
  console.log("UID[" + uid + "]");
});

/*
  Write something on serial channel
  Data casts to a Buffer, like
    -Buffer([0xFF], 'hex');
    -Buffer(data, 'utf8');
*/
emitter.on('response', function (data) {
  var buff = new Buffer(data, 'ascii');
  serialPort.write(buff);
});

