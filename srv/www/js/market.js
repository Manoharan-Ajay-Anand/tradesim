const priceTable = document.getElementById("priceTable");

const bidRows = [];
const askRows = [];

for (let i = 1; i <= 13; ++i) {
    const row = document.createElement("tr");

    const bidSize = document.createElement("td");
    bidSize.classList.add("invisible", "cell");
    bidSize.textContent = "0";

    const bid = document.createElement("td");
    bid.classList.add("invisible", "right-border", "cell");
    bid.textContent = "0";

    const ask = document.createElement("td");
    ask.classList.add("invisible", "cell");
    ask.textContent = "0";

    const askSize = document.createElement("td");
    askSize.classList.add("invisible", "cell");
    askSize.textContent = "0";
    
    row.append(bidSize, bid, document.createElement("td"), ask, askSize);
    priceTable.append(row);

    bidRows.push({ priceElem: bid, sizeElem: bidSize});
    askRows.push({ priceElem: ask, sizeElem: askSize});
}

const searchParams = new URLSearchParams(window.location.search);

const marketId = searchParams.get("marketId");
const traderId = searchParams.get("traderId");

if (!marketId || !traderId) {
    alert("MarketId and TraderId not specified in query string");
}

const bids = new Map();
const asks = new Map();

const evtSource = new EventSource("/api/tradesim/stream/" + marketId + "/" + traderId);

function findIndex(arr, elem, compareFn) {
    let start = 0;
    let end = arr.length;
    while (start < end) {
        let mid = Math.floor((start + end) / 2);
        if (compareFn(elem, arr[mid]) <= 0) {
            end = mid;
        } else {
            start = mid + 1;
        }
    }
    return start;
}

function removeElem(arr, elem, compareFn) {
    let index = findIndex(arr, elem, compareFn);
    if (index === arr.length || arr[index] !== elem) {
        return;
    }
    for (let i = index; i < arr.length - 1; ++i) {
        arr[i] = arr[i + 1];
    }
    arr.pop();
}

function insertElem(arr, elem, compareFn) {
    let index = findIndex(arr, elem, compareFn);
    if (index === arr.length) {
        arr.push(elem);
        return;
    } else if (arr[index] === elem) {
        return;
    }
    arr.push(-1);
    for (let i = arr.length - 1; i > index; --i) {
        arr[i] = arr[i - 1];
    }
    arr[index] = elem;
}

function update(arr, map, price, size, compareFn) {
    if (size === 0) {
        map.delete(price);
        removeElem(arr, price, compareFn);
    } else {
        map.set(price, size);
        insertElem(arr, price, compareFn);
    }
}

function updateRows(rows, map, arr) {
    for (let i = 0; i < rows.length; ++i) {
        let priceElem = rows[i].priceElem;
        let sizeElem = rows[i].sizeElem;

        if (i >= arr.length) {
            priceElem.classList.add("invisible");
            sizeElem.classList.add("invisible");    
        } else {
            priceElem.textContent = arr[i].toString();
            sizeElem.textContent = map.get(arr[i]).toString();
            priceElem.classList.remove("invisible");
            sizeElem.classList.remove("invisible");
        }
    }
}

const orderedBids = [];
const orderedAsks = [];

evtSource.addEventListener("pricePoint", (event) => {
    const pp = JSON.parse(event.data);
    
    update(orderedBids, bids, pp.price, pp.bids, (a, b) => b - a);
    update(orderedAsks, asks, pp.price, pp.asks, (a, b) => a - b);

    updateRows(bidRows, bids, orderedBids);
    updateRows(askRows, asks, orderedAsks);
});

const positionValueElem = document.getElementById("positionValue");
const pnlValueElem = document.getElementById("pnlValue");

evtSource.addEventListener("account", (event) => {
    const account = JSON.parse(event.data);
    positionValueElem.textContent = account.count.toString();
    pnlValueElem.textContent = account.pnl.toString();
});

const priceValueElem = document.getElementById("priceValue");

evtSource.addEventListener("trade", (event) => {
    const trade = JSON.parse(event.data);
    priceValueElem.textContent = trade.price.toString();
});

evtSource.addEventListener("duplicate", () => {
    alert("Only one stream allowed per trader");
    evtSource.close();
});

const orderPillMap = new Map();

const pendingOrderBox = document.getElementById("pendingOrderBox");

function getOrderPillText(update) {
    let t = "Bid: ";
    if (update.orderType === "ask") {
        t = "Ask: ";
    }
    t += `${update.remaining} @ ${update.price}`;
    return t;
}

async function cancelOrder(orderId) {
    const form = { marketId, traderId, orderId }
    const request = new Request("/api/tradesim/cancel", { method: "POST", body: JSON.stringify(form) });

    const response = await fetch(request);

    if (!response.ok) {
        alert(await response.text());
    }
}

evtSource.addEventListener("orderSubmitted", (event) => {
    const update = JSON.parse(event.data);
    
    const orderPill = document.createElement("div");
    orderPill.classList.add("order-pill");
    
    const orderText = document.createElement("span");
    orderText.textContent = getOrderPillText(update);

    const orderCancel = document.createElement("div");
    orderCancel.innerHTML = "&#x00d7;";
    orderCancel.classList.add("order-cancel");
    orderCancel.addEventListener("click", () => cancelOrder(update.orderId));

    orderPill.append(orderText, orderCancel);
    pendingOrderBox.append(orderPill);

    orderPillMap.set(update.orderId, { pill: orderPill, span: orderText });
});

evtSource.addEventListener("orderExecuted", (event) => {
    const update = JSON.parse(event.data);

    if (orderPillMap.has(update.orderId)) {
        const orderElement = orderPillMap.get(update.orderId);
        orderElement.span.textContent = getOrderPillText(update);
        if (update.remaining === 0) {
            pendingOrderBox.removeChild(orderElement.pill);
            orderPillMap.delete(update.orderId);
        }
    }
});

evtSource.addEventListener("orderCancelled", (event) => {
    const update = JSON.parse(event.data);

    if (orderPillMap.has(update.orderId)) {
        pendingOrderBox.removeChild(orderPillMap.get(update.orderId).pill);
        orderPillMap.delete(update.orderId);
    }
});

const submitButton = document.getElementById("submitButton");
submitButton.addEventListener("click", onClickSubmit);

const priceInput = document.getElementById("priceInput");
const quantityInput = document.getElementById("quantityInput");

async function onClickSubmit(elem, event) {
    const price = parseInt(priceInput.value);
    const quantity = parseInt(quantityInput.value);
    const type = document.querySelector('input[name="orderType"]:checked').value;

    const form = { price, quantity, type, marketId, traderId }
    const request = new Request("/api/tradesim/order", { method: "POST", body: JSON.stringify(form) });
    const response = await fetch(request);

    if (!response.ok) {
        alert(await response.text());
    }
} 
