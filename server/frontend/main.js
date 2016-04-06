window.addEventListener('deviceorientation',handleOrientation, true);

var socket = io();

function handleOrientation(event) {
  $(".alpha").text("Alpha: " + event.alpha);
  $(".beta").text("Beta: " + event.beta);
  $(".gamma").text("Gamma: " + event.gamma);
  socket.emit('jsmessage', event.alpha);
}
