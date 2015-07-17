// saveLatestRestMsg.js
// Author: Bryce Arden
// July 2015

// Code for the SaveLatestRestMsg function block on the Node-Red server
msg.payload = context.global.latestMessage;
return msg;
