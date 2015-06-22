"use strict";

var com          = require("serialport"),
    S            = require('string'),
    EventEmitter = require('events').EventEmitter;

var emitter    = new EventEmitter();
var serialPort = new com.SerialPort("/dev/ttyACM0", {
  baudrate: 38400,
  parity: 'odd',
  parser: com.parsers.readline('\r')
});

serialPort.on('open',function() {
  console.log('Connected to ' + this.path + " and listening...");
   setTimeout(lol, 1000);
});

function lol()  {
  emitter.emit('response', "outofservice");
}

serialPort.on('data', function(data) {
  if ( S(data).contains('VERIFY-UID:')) {
    emitter.emit('verify', cut_uid_from_raw_data(data));
    emitter.emit('authorized', "auth<RR>");
  } else {
    console.log(data);
  }
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

// emitter.on('', function (argument) {
// });

emitter.on('verify', function (uid) {
  console.log("UID[" + uid + "]");
});

emitter.on('authorized', function (direction) {
  var buff = new Buffer(direction, 'ascii');
  serialPort.write(buff);
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

emitter.on('keepalive', function () {
  var buff = new Buffer("keepalive", 'ascii');
  serialPort.write(buff);
});

