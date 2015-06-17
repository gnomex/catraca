var com = require("serialport");
var S = require('string');

var serialPort = new com.SerialPort("/dev/ttyACM2", {
  baudrate: 9600,
  parser: com.parsers.readline('\r\n')
});

serialPort.on('open',function() {
  console.log('Port open');
  // setTimeout(write_something, 5000);
});

serialPort.on('data', function(data) {
  if ( S(data).contains('VERIFY-UID:')) {
    console.log("Tem um UID para processar, máoe");
    verify_uid(data);
  };
  console.log(data);
});

function verify_uid(uid) {
  var lol = uid.indexOf(':');
  var hehehe = uid.substring(lol + 1, uid.length);

  var n_uid = parseInt(hehehe, 10);
  console.log("UID - " + n_uid);
}

function write_something (argument) {
  serialPort.write(0xFF, function (err, res) {
    console.log('err ' + err);
    console.log('results ' + res);
  });
}

function asserting() {
  console.log('asserting');

  serialPort.write("OMG IT WORKS\r", function (err, res) {
    console.log('err ' + err);
    console.log('results ' + res);
  });
}

function clear() {
  console.log('clearing');

  serialPort.write("clearing\r", function (err, res) {
    console.log('err ' + err);
    console.log('results ' + res);
  });
}

function done() {
  console.log("done resetting");
}
