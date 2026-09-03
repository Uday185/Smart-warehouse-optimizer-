// Tab Switcher
function switchTab(tabId) {
    document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
    document.querySelectorAll('.tab-btn').forEach(el => el.classList.remove('active'));
    
    document.getElementById(tabId).classList.add('active');
    event.currentTarget.classList.add('active');

    if (tabId === 'routeTab') {
        drawWarehouseMap();
    }
}

function appendLog(msg) {
    const consoleLog = document.getElementById('consoleLog');
    if (!consoleLog) return;

    const time = new Date().toLocaleTimeString();
    const line = document.createElement('div');
    line.className = 'terminal-line';
    line.innerHTML = `<span class="terminal-prompt">[${time}] $</span> ${msg}`;
    consoleLog.appendChild(line);
    consoleLog.scrollTop = consoleLog.scrollHeight;
}

function getInventoryTableBody() {
    return document.getElementById('inventoryTableBody') || document.getElementById('inventoryTable');
}

function formatINR(value) {
    return new Intl.NumberFormat('en-IN', {
        style: 'currency',
        currency: 'INR',
        maximumFractionDigits: 2
    }).format(Number(value || 0));
}

const cityNames = {
    0: 'Bangalore',
    1: 'Chennai',
    2: 'Kolkata',
    3: 'Delhi',
    4: 'Mumbai'
};

const supplierMap = {
    101: 'TechHub Electronics',
    102: 'Metro Gadgets',
    103: 'Prime Office Solutions',
    104: 'Vision Displays',
    105: 'PowerCore Suppliers',
    106: 'DeskPro Studio',
    107: 'CamBridge Systems',
    108: 'SoundWave Traders'
};

let inventoryData = [];
let salesOrders = [
    { id: 'SO-1001', customer: 'Aarav Retail', item: 'Mechanical Keyboard', qty: 3, status: 'Packed', total: 4799.97 },
    { id: 'SO-1002', customer: 'Nexa Services', item: 'UPS Battery', qty: 2, status: 'Dispatched', total: 9000.00 }
];

function getCityName(cityId) {
    return cityNames[cityId] || cityNames[0];
}

function getSupplierName(item) {
    return item.supplier || supplierMap[item.id] || 'Unassigned Supplier';
}

function populateInventorySelectors() {
    const editSelect = document.getElementById('editItemSelect');
    const deleteSelect = document.getElementById('deleteItemSelect');
    const orderSelect = document.getElementById('orderItemSelect');
    if (!editSelect || !deleteSelect || !orderSelect) return;

    editSelect.innerHTML = inventoryData.map(item => `<option value="${item.id}">${item.name}</option>`).join('');
    deleteSelect.innerHTML = inventoryData.map(item => `<option value="${item.id}">${item.name}</option>`).join('');
    orderSelect.innerHTML = inventoryData.map(item => `<option value="${item.id}">${item.name}</option>`).join('');

    if (inventoryData.length) {
        const first = inventoryData[0];
        editSelect.value = String(first.id);
        deleteSelect.value = String(first.id);
        orderSelect.value = String(first.id);
    }
}

function renderInventorySummary() {
    const totalUnits = inventoryData.reduce((sum, item) => sum + Number(item.quantity || 0), 0);
    const totalValue = inventoryData.reduce((sum, item) => sum + Number(item.quantity || 0) * Number(item.price || 0) * (1 + (Number(item.gst || 18) / 100)), 0);
    const cityMap = {};
    const supplierMapSummary = {};

    inventoryData.forEach(item => {
        const city = getCityName(item.aisle)
        cityMap[city] = (cityMap[city] || 0) + Number(item.quantity || 0);
        const supplier = getSupplierName(item);
        supplierMapSummary[supplier] = (supplierMapSummary[supplier] || 0) + Number(item.quantity || 0);
    });

    const totalUnitsEl = document.getElementById('totalUnits');
    const totalValueEl = document.getElementById('inventoryValue');
    const supplierCountEl = document.getElementById('supplierCount');
    const citySummaryEl = document.getElementById('citySummary');
    const supplierSummaryEl = document.getElementById('supplierSummary');

    if (totalUnitsEl) totalUnitsEl.textContent = totalUnits;
    if (totalValueEl) totalValueEl.textContent = formatINR(totalValue);
    if (supplierCountEl) supplierCountEl.textContent = Object.keys(supplierMapSummary).length;

    if (citySummaryEl) {
        citySummaryEl.innerHTML = Object.entries(cityMap)
            .map(([city, units]) => `<div class="mini-row"><span>${city}</span><strong>${units}</strong></div>`)
            .join('') || '<div class="mini-row"><span>No city data</span><strong>0</strong></div>';
    }

    if (supplierSummaryEl) {
        supplierSummaryEl.innerHTML = Object.entries(supplierMapSummary)
            .map(([supplier, units]) => `<div class="mini-row"><span>${supplier}</span><strong>${units}</strong></div>`)
            .join('') || '<div class="mini-row"><span>No supplier data</span><strong>0</strong></div>';
    }
}

function renderSalesOrders() {
    const tbody = document.getElementById('salesOrderTableBody');
    if (!tbody) return;
    tbody.innerHTML = salesOrders.map(order => `
        <tr>
            <td><strong>${order.id}</strong></td>
            <td>${order.customer}</td>
            <td>${order.item}</td>
            <td>${order.qty}</td>
            <td><span class="badge">${order.status}</span></td>
        </tr>
    `).join('');
}

// 1. INVENTORY MANAGER
async function fetchInventory() {
    appendLog('Fetching live stock ledger from inventory service...');
    try {
        const res = await fetch('/api/inventory');
        const json = await res.json();
        if (json.status === 'success') {
            inventoryData = json.data.map(item => ({
                ...item,
                gst: item.gst ?? 18,
                supplier: supplierMap[item.id] || 'Prime Trade'
            }));
            renderInventoryTable(inventoryData);
            renderInventorySummary();
            populateInventorySelectors();
            renderSalesOrders();
            appendLog(`Loaded ${inventoryData.length} product records into the stock ledger.`);
        }
    } catch (e) {
        appendLog(`Error fetching inventory: ${e.message}`);
    }
}

function renderInventoryTable(items) {
    const tbody = getInventoryTableBody();
    if (!tbody) {
        appendLog('Inventory table element not found.');
        return;
    }

    tbody.innerHTML = '';
    items.forEach(item => {
        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td><strong>#${item.id}</strong></td>
            <td>${item.name}</td>
            <td><span class="badge">${item.category}</span></td>
            <td>${item.quantity} units</td>
            <td>${formatINR(item.price)}</td>
            <td>${item.weight} kg</td>
            <td>${getCityName(item.aisle)}</td>
        `;
        tbody.appendChild(tr);
    });
}

function addInventoryItem() {
    const name = document.getElementById('addItemName').value.trim();
    const category = document.getElementById('addItemCategory').value.trim();
    const supplier = document.getElementById('addItemSupplier').value.trim() || 'Prime Trade';
    const city = document.getElementById('addItemCity').value;
    const price = Number(document.getElementById('addItemPrice').value || 0);
    const gst = Number(document.getElementById('addItemGst').value || 18);
    const qty = Number(document.getElementById('addItemQty').value || 0);

    if (!name || !category) {
        appendLog('Item name and category are required.');
        return;
    }

    const newItem = {
        id: Date.now() % 100000,
        name,
        category,
        quantity: qty,
        price,
        weight: 1.0,
        gst,
        supplier,
        aisle: city === 'Bangalore' ? 0 : city === 'Chennai' ? 1 : city === 'Kolkata' ? 2 : city === 'Delhi' ? 3 : 4
    };

    inventoryData.push(newItem);
    renderInventoryTable(inventoryData);
    renderInventorySummary();
    populateInventorySelectors();
    appendLog(`New item added: ${name} (${qty} units) in ${city}.`);
}

function updateInventoryItem() {
    const itemId = Number(document.getElementById('editItemSelect').value);
    const qty = Number(document.getElementById('editItemQty').value || 0);
    const price = Number(document.getElementById('editItemPrice').value || 0);
    const gst = Number(document.getElementById('editItemGst').value || 18);
    const index = inventoryData.findIndex(item => Number(item.id) === itemId);
    if (index === -1) return;

    inventoryData[index].quantity = qty;
    inventoryData[index].price = price;
    inventoryData[index].gst = gst;
    renderInventoryTable(inventoryData);
    renderInventorySummary();
    appendLog(`Updated item #${itemId}.`);
}

function deleteInventoryItem() {
    const itemId = Number(document.getElementById('deleteItemSelect').value);
    inventoryData = inventoryData.filter(item => Number(item.id) !== itemId);
    renderInventoryTable(inventoryData);
    renderInventorySummary();
    populateInventorySelectors();
    appendLog(`Deleted item #${itemId}.`);
}

function createSalesOrder() {
    const itemId = Number(document.getElementById('orderItemSelect').value);
    const item = inventoryData.find(product => Number(product.id) === itemId);
    const qty = Number(document.getElementById('orderQty').value || 1);
    const customer = document.getElementById('orderCustomer').value.trim() || 'Walk-In Customer';

    if (!item) return;
    if (qty > item.quantity) {
        appendLog(`Order rejected: insufficient stock for ${item.name}.`);
        return;
    }

    item.quantity -= qty;
    const total = item.price * qty * (1 + (Number(item.gst || 18) / 100));
    const newOrder = {
        id: `SO-${Date.now() % 100000}`,
        customer,
        item: item.name,
        qty,
        status: 'Pending',
        total
    };

    salesOrders.unshift(newOrder);
    renderInventoryTable(inventoryData);
    renderInventorySummary();
    renderSalesOrders();
    appendLog(`Sales order created for ${customer}: ${qty} x ${item.name}.`);
}

// 2. STACK PRICING EVALUATOR
async function evaluateExpression() {
    const expr = document.getElementById('exprInput').value;
    appendLog(`Executing C Stack Expression Evaluator: "${expr}"`);
    try {
        const res = await fetch(`/api/eval_expr?expr=${encodeURIComponent(expr)}`);
        const json = await res.json();
        if (json.status === 'success') {
            document.getElementById('exprResult').innerText = `Calculated Price: ${formatINR(json.result)}`;
            appendLog(`Pricing calculation output = ${formatINR(json.result)}`);
        }
    } catch (e) {
        appendLog(`Error evaluating expression: ${e.message}`);
    }
}

// 3. STOCK INDEX
function runBstTraversal(type) {
    const out = document.getElementById('treeOutput');
    appendLog(`Reviewing stock index view: ${type.toUpperCase()}...`);
    out.innerHTML = `Reviewing inventory sort view: ${type.toUpperCase()}...\n\n`;
    
    setTimeout(() => {
        let text = "";
        if (type === 'inorder') {
            text += "[SORTED PRODUCT CATALOG]\n";
            text += "ID: 101 | Keyboard | Stock: 35 | Price: ₹1,599.00\n";
            text += "ID: 102 | Mouse | Stock: 60 | Price: ₹799.00\n";
            text += "ID: 103 | Printer | Stock: 18 | Price: ₹2,490.00\n";
            text += "ID: 104 | Monitor | Stock: 12 | Price: ₹18,999.00\n";
            text += "ID: 105 | UPS | Stock: 22 | Price: ₹4,500.00\n";
        } else if (type === 'preorder') {
            text += "[BACKUP STOCK VIEW]\n";
            text += "ID: 101 (Root) -> L: 102 -> R: 105 -> L: 103 -> R: 104\n";
        } else {
            text += "[AUDIT STOCK REVIEW]\n";
            text += "ID: 104 -> ID: 103 -> ID: 102 -> ID: 105 -> ID: 101 (Audit complete)\n";
        }
        out.innerText = text;
        appendLog(`Inventory index review complete.`);
    }, 300);
}

// 4. CITY ROUTE MAP
const graphNodes = [
    { id: 0, name: "Bangalore", x: 120, y: 190 },
    { id: 1, name: "Chennai",   x: 320, y: 100 },
    { id: 2, name: "Kolkata",   x: 320, y: 280 },
    { id: 3, name: "Delhi",     x: 520, y: 190 },
    { id: 4, name: "Mumbai",    x: 700, y: 190 }
];

const graphEdges = [
    { src: 0, dest: 1, weight: 15 },
    { src: 0, dest: 2, weight: 25 },
    { src: 1, dest: 3, weight: 10 },
    { src: 2, dest: 3, weight: 20 },
    { src: 3, dest: 4, weight: 12 },
    { src: 1, dest: 4, weight: 30 }
];

let activePath = [];

function drawWarehouseMap() {
    const canvas = document.getElementById('warehouseCanvas');
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Draw edges
    graphEdges.forEach(edge => {
        const n1 = graphNodes[edge.src];
        const n2 = graphNodes[edge.dest];

        const isHighlighted = activePath.length > 1 && 
            activePath.some((v, idx) => idx < activePath.length - 1 && 
                ((v === edge.src && activePath[idx+1] === edge.dest) || (v === edge.dest && activePath[idx+1] === edge.src)));

        ctx.beginPath();
        ctx.moveTo(n1.x, n1.y);
        ctx.lineTo(n2.x, n2.y);
        ctx.strokeStyle = isHighlighted ? '#06b6d4' : 'rgba(255, 255, 255, 0.15)';
        ctx.lineWidth = isHighlighted ? 5 : 2;
        if (isHighlighted) {
            ctx.shadowColor = '#06b6d4';
            ctx.shadowBlur = 10;
        } else {
            ctx.shadowBlur = 0;
        }
        ctx.stroke();

        // Edge Weight label
        const midX = (n1.x + n2.x) / 2;
        const midY = (n1.y + n2.y) / 2;
        ctx.fillStyle = '#9ca3af';
        ctx.font = '12px Inter';
        ctx.fillText(`${edge.weight}m`, midX - 8, midY - 6);
    });

    // Draw nodes
    graphNodes.forEach(node => {
        const isInPath = activePath.includes(node.id);

        ctx.shadowBlur = isInPath ? 15 : 0;
        ctx.shadowColor = '#3b82f6';

        ctx.beginPath();
        ctx.arc(node.x, node.y, 22, 0, Math.PI * 2);
        ctx.fillStyle = isInPath ? '#3b82f6' : '#111827';
        ctx.fill();
        ctx.strokeStyle = isInPath ? '#06b6d4' : 'rgba(255, 255, 255, 0.3)';
        ctx.lineWidth = 3;
        ctx.stroke();

        // Node ID Text
        ctx.fillStyle = '#ffffff';
        ctx.font = 'bold 13px Outfit';
        ctx.textAlign = 'center';
        ctx.fillText(`Z${node.id}`, node.x, node.y + 4);

        // Node Name Label
        ctx.fillStyle = '#f3f4f6';
        ctx.font = '12px Inter';
        ctx.fillText(node.name, node.x, node.y + 40);
    });
}

async function calculateBestRoute() {
    const src = document.getElementById('srcZone').value;
    const dest = document.getElementById('destZone').value;

    appendLog(`Calculating fastest delivery route from ${graphNodes[src].name} to ${graphNodes[dest].name}...`);
    try {
        const res = await fetch(`/api/shortest_path?src=${src}&dest=${dest}`);
        const json = await res.json();
        if (json.status === 'success') {
            activePath = json.path;
            drawWarehouseMap();

            const pathNames = json.path.map(id => graphNodes[id].name).join(' ➔ ');
            document.getElementById('routeResult').innerHTML = `
                <div style="color: var(--accent-emerald); font-weight: 600;">✓ Best delivery route found!</div>
                <div style="margin-top: 0.4rem;"><strong>Total Distance:</strong> ${json.distance} km</div>
                <div style="margin-top: 0.2rem; color: var(--accent-cyan); font-size: 0.9rem;">${pathNames}</div>
            `;
            appendLog(`Best route found: Path = [${json.path.join(', ')}], Distance = ${json.distance} km`);
        }
    } catch (e) {
        appendLog(`Error calculating best route: ${e.message}`);
    }
}

function triggerGraphTraversal(type) {
    appendLog(`Reviewing city network map: ${type.toUpperCase()}...`);
    activePath = type === 'bfs' ? [0, 1, 2, 3, 4] : [0, 1, 3, 4, 2];
    drawWarehouseMap();
    appendLog(`City network view refreshed.`);
}

// 5. LOAD OPTIMIZER
async function runLoadOptimization() {
    const cap = document.getElementById('capacitySlider').value;
    appendLog(`Optimizing delivery load plan for ${cap} kg...`);

    try {
        const res = await fetch(`/api/knapsack?capacity=${cap}`);
        const json = await res.json();
        if (json.status === 'success') {
            document.getElementById('dpRes').innerHTML = `
                <strong>Best shipment value:</strong> <span style="color: var(--accent-emerald); font-size: 1.1rem;">${formatINR(json.dp.value)}</span><br>
                <strong>Load used:</strong> ${json.dp.weight.toFixed(1)} / ${json.capacity} kg<br>
                <strong>Products packed:</strong> ${json.dp.count} items
            `;
            document.getElementById('greedyRes').innerHTML = `
                <strong>Best shipment value:</strong> <span style="color: var(--accent-emerald); font-size: 1.1rem;">${formatINR(json.greedy.value)}</span><br>
                <strong>Load used:</strong> ${json.greedy.weight.toFixed(1)} / ${json.capacity} kg<br>
                <strong>Products packed:</strong> ${json.greedy.count} items
            `;
            appendLog(`Load plan completed. Best shipment value = ${formatINR(json.dp.value)}`);
        }
    } catch (e) {
        appendLog(`Error running load optimizer: ${e.message}`);
    }
}

// 6. PERFORMANCE REPORT
async function runBenchmark() {
    appendLog('Generating sales and inventory performance report...');
    try {
        const res = await fetch('/api/benchmark');
        const json = await res.json();
        if (json.status === 'success') {
            renderBenchmarkTable(json.results);
            appendLog(`Performance report generated across ${json.results.length} processes.`);
        }
    } catch (e) {
        appendLog(`Error running performance report: ${e.message}`);
    }
}

function renderBenchmarkTable(results) {
    const tbody = document.getElementById('benchmarkTableBody');
    tbody.innerHTML = '';
    const friendlyNames = {
        'QuickSort (O(N log N))': 'Fast Product Sort',
        'MergeSort (O(N log N))': 'City Stock Merge',
        'InsertionSort (O(N^2))': 'Daily Sales Insert',
        'Linear Search (O(N))': 'Store Item Search',
        'Binary Search (O(log N))': 'Quick Item Lookup'
    };

    results.forEach(r => {
        const friendlyName = friendlyNames[r.name] || r.name;
        const isSort = /sort|merge|insert/i.test(r.name);
        const complexity = r.name.includes('O(') ? r.name.match(/\((.*)\)/)[1] : 'N/A';

        const tr = document.createElement('tr');
        tr.innerHTML = `
            <td><strong>${friendlyName}</strong></td>
            <td><span class="badge">${isSort ? 'Sorting' : 'Searching'}</span></td>
            <td>${complexity}</td>
            <td>${r.comparisons} ops</td>
            <td>${r.swaps} swaps</td>
            <td><strong style="color: var(--accent-emerald);">${r.time_ms.toFixed(4)} ms</strong></td>
        `;
        tbody.appendChild(tr);
    });
}

// Initial Load
window.addEventListener('DOMContentLoaded', () => {
    fetchInventory();
    drawWarehouseMap();
});
