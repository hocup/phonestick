var express =require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var fs = require('fs');

fs.writeFile("/proc/phonestick", "34\0", function (err) {
  if(err) {
    return console.log("error: ", err);
  }
  console.log("File saved");
});

app.use('/',express.static('frontend'));

http.listen(3000, function() {
  console.log('Server listening on port 3000');
});

io.on('connection', function(socket) {
  console.log('a user has connected');
  socket.on('disconnect', function() {
    console.log('A user has disconnected');
  });

  socket.on('jsmessage', function(msg) {
    console.log("Recieved a joystick message from client: ", msg);

    if(msg.x != -1) {
      setAxis("x", msg.x);
    }
    if(msg.y != -1) {
      setAxis("y", msg.y);
    }
  });
});

function setAxis(axis, value) {
  fs.writeFile("/proc/phonestick", axis + value.toString(16) + "\0", function (err) {});
}
