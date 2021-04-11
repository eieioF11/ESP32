const url = 'ws://192.168.30.4:81';
var connection = new WebSocket(url, ['arduino']);
connection.onopen = function () {
	connection.send('Message from Browser to ESP8266 yay its Working!! ' + new Date()); 
	connection.send('ping');
    connection.send('Time: ' + new Date()); 
};
connection.onerror = function (error) {
	console.log('WebSocket Error ', error);
};

connection.binaryType = "arraybuffer";



