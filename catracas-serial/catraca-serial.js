// executar monitor serial
// Troca de eventos entre monitor e server express (Rest ou socket.io ???)

var com = require("serialport");
var S = require('string');
var EventEmitter = require('events').EventEmitter;
var emitter = new EventEmitter();

var serialPort = new com.SerialPort("/dev/ttyACM0", {
  baudrate: 9600,
  parser: com.parsers.readline('\r')
});

serialPort.on('open',function() {
  console.log('Connected to ' + this.path + " and listening...");
});

serialPort.on('data', function(data) {
  //Criar um string processor
  if ( S(data).contains('VERIFY-UID:')) {
    emitter.emit('verify', cut_uid_from_raw_data(data));
    emitter.emit('response');
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
  console.log("From event: " + uid);
});

emitter.on('response', function (err, data) {
  if (err)  {
    console.log("Deu merda!" + err);
    return;
  }

  // var buff = new Buffer([0xFF], 'hex');
  // serialPort.write(buff, function (err, res) {
  // serialPort.write("print<Hey bitchs>", function (err, res) {
  serialPort.write(data, function (err, res) {
    console.log('err ' + err);
    console.log('results ' + res);
  });
});
