const express =require('express');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);

const fs = require('fs');

const PROC_FILE_PATH = "/proc/phonestick";
const ALLOWED_AXES = ['x', 'y', 'z', 'r', 'p', 't'];

fs.writeFile(PROC_FILE_PATH, "34\0", function (err) {
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

    // if(msg.x != -1) {
    //   setAxis("x", msg.x);
    // }
    // if(msg.y != -1) {
    //   setAxis("y", msg.y);
    // }

    Object.keys(msg).map(
      (axis) => {
        
        if(ALLOWED_AXES.find((a) => {return axis === a}) && msg[axis] !== -1) {
          setAxis(axis, Math.floor(msg[axis]));
        }
      }
    );
  });
});

function setAxis(axis, value) {
  console.log("setting axis " + axis + " " + value.toString(16));
  fs.writeFile(PROC_FILE_PATH, axis + value.toString(16) + "\0", function (err) {});
}
