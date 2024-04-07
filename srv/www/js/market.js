const priceTable = document.getElementById("priceTable");

const bidRows = [];
const askRows = [];

for (let i = 1; i <= 17; ++i) {
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
    priceTable.prepend(row);

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

const orderedBids = [];
const orderedAsks = [];

evtSource.addEventListener("pricePoint", (event) => {
    const pp = JSON.parse(event.data);
    console.log(pp);
    
    update(orderedBids, bids, pp.price, pp.bids, (a, b) => b - a);
    update(orderedAsks, asks, pp.price, pp.asks, (a, b) => a - b);

    console.log(orderedBids);
    console.log(orderedAsks);
});

evtSource.addEventListener("duplicate", () => {
    alert("Only one stream allowed per trader");
    evtSource.close();
});
