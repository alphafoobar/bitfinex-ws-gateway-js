const http = require('http');
const options = {
    host: '0.0.0.0',
    port: 8080,
    timeout: 2000,
    path: '/healthcheck'
};

const healthCheck = http.request(options, (res) => {
    if (res.statusCode === 200) {
        process.exit(0);
    } else {
        process.exit(1);
    }
});

healthCheck.on('error', function (err) {
    console.error(`ERROR: ${err}`);
    process.exit(1);
});

healthCheck.end();
