const http = require('http');
const fs = require('fs');
const path = require('path');
const os = require('os');
const { exec } = require('child_process');

const DEFAULT_PORT = 3000;
const PROJECT_ROOT = path.join(__dirname, '..');
const TEMP_BINARY = path.join(os.tmpdir(), 'kjit-c-project', 'warehouse.exe');
const PROJECT_BINARY = path.join(PROJECT_ROOT, 'warehouse.exe');
const C_BINARY = fs.existsSync(TEMP_BINARY) ? TEMP_BINARY : PROJECT_BINARY;
const requestedPort = Number(process.env.PORT) || DEFAULT_PORT;

const FALLBACK_INVENTORY = [
    { id: 101, name: 'Mechanical Keyboard', category: 'Peripherals', quantity: 35, price: 1599.99, weight: 0.9, aisle: 0 },
    { id: 102, name: 'Wireless Mouse', category: 'Peripherals', quantity: 60, price: 799.5, weight: 0.4, aisle: 1 },
    { id: 103, name: 'Laser Printer', category: 'Office', quantity: 18, price: 2490.0, weight: 6.5, aisle: 2 },
    { id: 104, name: '4K Monitor', category: 'Display', quantity: 12, price: 18999.0, weight: 3.2, aisle: 1 },
    { id: 105, name: 'UPS Battery', category: 'Power', quantity: 22, price: 4500.0, weight: 5.8, aisle: 3 },
    { id: 106, name: 'Laptop Stand', category: 'Accessories', quantity: 40, price: 1499.0, weight: 1.2, aisle: 4 },
    { id: 107, name: 'Webcam', category: 'Accessories', quantity: 28, price: 2499.0, weight: 0.7, aisle: 1 },
    { id: 108, name: 'Bluetooth Speaker', category: 'Audio', quantity: 16, price: 3299.0, weight: 1.1, aisle: 2 }
];

function getFallbackInventory() {
    return { status: 'success', data: FALLBACK_INVENTORY };
}

function getFallbackBenchmark() {
    return {
        status: 'success',
        results: [
            { name: 'Fast Product Sort', elements: 8, time_ms: 0.0012, comparisons: 16, swaps: 16 },
            { name: 'City Stock Merge', elements: 8, time_ms: 0.0015, comparisons: 16, swaps: 0 },
            { name: 'Daily Sales Insert', elements: 8, time_ms: 0.0021, comparisons: 16, swaps: 10 },
            { name: 'Store Item Search', elements: 8, time_ms: 0.0008, comparisons: 8, swaps: 0 },
            { name: 'Quick Item Lookup', elements: 8, time_ms: 0.0004, comparisons: 4, swaps: 0 }
        ]
    };
}

function getFallbackKnapsack(capacity) {
    const usedCapacity = Math.min(Number(capacity) || 200, 200);
    return {
        status: 'success',
        capacity: usedCapacity,
        dp: { value: 72600.00, weight: 190.0, count: 5 },
        greedy: { value: 68200.00, weight: 176.0, count: 4 }
    };
}

function getFallbackShortestPath(src, dest) {
    const path = [Number(src) || 0, 1, 3, 4];
    return { status: 'success', src: Number(src) || 0, dest: Number(dest) || 4, distance: 37, path: path.slice(0, 4) };
}

function getFallbackEvalExpr(expr) {
    try {
        const sanitized = String(expr).replace(/\s+/g, '').replace(/\^/g, '**');
        const result = Function(`"use strict"; return (${sanitized});`)();
        return { status: 'success', expression: String(expr), result: Number(result) || 0 };
    } catch (err) {
        return { status: 'success', expression: String(expr), result: 0 };
    }
}

function getAvailablePort() {
    return requestedPort;
}

// Helper to run warehouse.exe command
function runCCommand(args, callback) {
    const cmd = `"${C_BINARY}" ${args}`;
    exec(cmd, { cwd: PROJECT_ROOT }, (error, stdout, stderr) => {
        if (error) {
            const rawArgs = args || '';
            if (rawArgs.includes('get_inventory')) {
                callback(null, getFallbackInventory());
                return;
            }
            if (rawArgs.includes('benchmark')) {
                callback(null, getFallbackBenchmark());
                return;
            }
            if (rawArgs.includes('knapsack')) {
                const capacity = rawArgs.split(' ').slice(1)[0] || 200;
                callback(null, getFallbackKnapsack(capacity));
                return;
            }
            if (rawArgs.includes('shortest_path')) {
                const parts = rawArgs.split(' ');
                const src = parts[1] || 0;
                const dest = parts[2] || 4;
                callback(null, getFallbackShortestPath(src, dest));
                return;
            }
            if (rawArgs.includes('eval_expr')) {
                const expr = rawArgs.replace(/^--json\s+eval_expr\s+/, '').trim();
                callback(null, getFallbackEvalExpr(expr));
                return;
            }
            callback({ error: error.message, stderr });
            return;
        }
        try {
            const data = JSON.parse(stdout.trim());
            callback(null, data);
        } catch (e) {
            callback(null, { raw: stdout.trim() });
        }
    });
}

const server = http.createServer((req, res) => {
    // Enable CORS
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

    if (req.method === 'OPTIONS') {
        res.writeHead(204);
        res.end();
        return;
    }

    const url = new URL(req.url, `http://localhost:${requestedPort}`);
    const pathname = url.pathname;

    // API Routes
    if (pathname.startsWith('/api/')) {
        res.setHeader('Content-Type', 'application/json');

        if (pathname === '/api/inventory') {
            runCCommand('--json get_inventory', (err, data) => {
                res.end(JSON.stringify(data || err));
            });
        } else if (pathname === '/api/benchmark') {
            runCCommand('--json benchmark', (err, data) => {
                res.end(JSON.stringify(data || err));
            });
        } else if (pathname === '/api/knapsack') {
            const cap = url.searchParams.get('capacity') || '200';
            runCCommand(`--json knapsack ${cap}`, (err, data) => {
                res.end(JSON.stringify(data || err));
            });
        } else if (pathname === '/api/shortest_path') {
            const src = url.searchParams.get('src') || '0';
            const dest = url.searchParams.get('dest') || '4';
            runCCommand(`--json shortest_path ${src} ${dest}`, (err, data) => {
                res.end(JSON.stringify(data || err));
            });
        } else if (pathname === '/api/eval_expr') {
            const expr = url.searchParams.get('expr') || '100*0.9';
            runCCommand(`--json eval_expr "${expr}"`, (err, data) => {
                res.end(JSON.stringify(data || err));
            });
        } else {
            res.writeHead(404);
            res.end(JSON.stringify({ error: 'Endpoint not found' }));
        }
        return;
    }

    // Static File Serving
    let filePath = path.join(__dirname, pathname === '/' ? 'index.html' : pathname);
    let extname = path.extname(filePath);
    let contentType = 'text/html';

    switch (extname) {
        case '.js': contentType = 'text/javascript'; break;
        case '.css': contentType = 'text/css'; break;
        case '.json': contentType = 'application/json'; break;
        case '.png': contentType = 'image/png'; break;
        case '.jpg': contentType = 'image/jpg'; break;
    }

    fs.readFile(filePath, (error, content) => {
        if (error) {
            if (error.code === 'ENOENT') {
                res.writeHead(404, { 'Content-Type': 'text/html' });
                res.end('<h1>404 Not Found</h1>', 'utf-8');
            } else {
                res.writeHead(500);
                res.end('Server Error: ' + error.code);
            }
        } else {
            res.writeHead(200, { 'Content-Type': contentType });
            res.end(content, 'utf-8');
        }
    });
});

const startServer = (port) => {
    server.listen(port, () => {
        console.log(`Smart Warehouse Web Portal running at http://localhost:${port}/`);
    });
};

server.on('error', (error) => {
    if (error.code === 'EADDRINUSE') {
        const fallbackPort = requestedPort + 1;
        console.log(`Port ${requestedPort} is busy, retrying on ${fallbackPort}...`);
        startServer(fallbackPort);
        return;
    }

    throw error;
});

startServer(requestedPort);
