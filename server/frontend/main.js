window.addEventListener('deviceorientation',handleOrientation, true);

var socket = io();

function handleOrientation(event) {
  $(".alpha").text("Alpha: " + event.alpha);
  $(".beta").text("Beta: " + event.beta);
  $(".gamma").text("Gamma: " + event.gamma);
  var msg = {
    x: -1,
    y: -1
  }
  msg.x = Math.floor(event.gamma * 65535/60 + 65535/2);
  msg.y = Math.floor(event.beta * 65535/60 + 65535/2);
  socket.emit('jsmessage', msg);
}
