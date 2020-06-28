const WebSocket = require('ws');
const bitfinexClient = require('./bitfinex');

const wss = new WebSocket.Server({port: 8080});


wss.on('connection', function connection(ws) {
    ws.send(JSON.stringify({m: 'hello, world!'}));
});

bitfinexClient.on('message', function incoming(data) {
    console.log('incoming message ' + data);
    wss.clients.forEach(function each(client) {
        if (client.readyState === WebSocket.OPEN) {
            client.send(data);
        }
    });
});
