var express =require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var fs = require('fs');

fs.writeFile("/proc/phonestick", "a", function (err) {
  if(err) {
    return console.log("error: ", err);
  }
  console.log("File saved");
});

app.use('/',express.static('frontend'));
//app.get('/', function(req, res) {
//  res.sendfile('frontend/index.html');
//});

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
    if(msg > 90) {
      console.log("writing a");
      fs.writeFile("/proc/phonestick","a",function(err){});
    } else {
      console.log("writing b");
      fs.writeFile("/proc/phonestick","b",function(err){});
    } 
  });
});
