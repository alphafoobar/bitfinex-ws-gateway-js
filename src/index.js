const http = require('http');
const WebSocket = require('ws');
const bitfinexClient = require('./bitfinex');

const server = http.createServer(function (req, res) {
    switch(req.url) {
        case "/bitfinex-js-gateway/ping" :
            res.writeHead(200);
            res.end();
            return;
        case "/healthcheck" :
            res.setHeader("Content-Type", "application/json");
            res.writeHead(200);
            res.end(`{"m": "OK"}`);
            return;
    }
    res.writeHead(404);
    res.end();
});
const wss = new WebSocket.Server({ server });

wss.on('connection', function connection(ws) {
    ws.send(JSON.stringify({m: 'hello, world!'}));
});

bitfinexClient.serviceListener = function incoming(data) {
    wss.clients.forEach(function each(client) {
        if (client.readyState === WebSocket.OPEN) {
            client.send(data);
        }
    });
};

server.listen(8080);
