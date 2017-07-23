//config
var HOST = '192.168.1.8';
var PORT = 8888;
//Loadd TCP library
var net = require('net');//clients
var client = {
	'socket':0,
	'id':0,
	'ip':0,
	'action':0,
};

var clients = [];

//Wifi on Uno
var WifiOnUno = 0;


//create server
net.createServer(function (socket){
	console.log('connected:'+ socket.remoteAddress+':'+ socket.remotePort);

	//listening upcoming data
	socket.on('data', function(data){
		console.log('From:  '+ socket.remoteAddress+ ':'+ data);

		var ID = String(data);

		//Check incoming data with specific character
		var reg =/[XC]/g;
		if((ID.charAt(0)).match(reg)){

			// Add new client
			newClient(client, socket, String(data), socket.remoteAddress); 

			// set socket Uno to a const
			if('X' == ID.charAt(0)){
				WifiOnUno = socket;	
			}

		} else {
			//action data
// 			console.log('action: '+ WifiOnUno.remoteAddress + ":"+ WifiOnUno.remotePort);
			if(WifiOnUno){
				console.log('send data to WifiOnUno: '+ data);
				WifiOnUno.write(data);
			}
		}
			
	});



	socket.on('close', function(data){
		console.log('Closed:  '+ socket.remoteAddress);
	});
}).listen(PORT, HOST);

console.log('Server listening on '+ HOST+ ':'+ PORT);





/*
 * Add new client
 * @param client
 * @param socket
 * @param id
 * @param ip
 */
function newClient(client, socket, id, ip){
	var num = dupClient(id, clients);	
	if((-1) == num){
			
		client.socket = socket;
		client.id = id;
		client.ip = ip;
		clients.push(client);
	} else {
		clients[num] = socket;
	}
}

/*
 * Check id is duplicated or not
 * @param id  Incoming id
 * @export 
 */
function dupClient(id, clients){

	for(var i = 0; i < clients.length; i++){

		if(clients[i].id == id){
			return i;
		} else {
			return -1;
		}
	}
}

