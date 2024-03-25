const searchParams = new URLSearchParams(window.location.search);

const marketId = searchParams.get("marketId");
const traderId = searchParams.get("traderId");

if (!marketId || !traderId) {
    alert("MarketId and TraderId not specified in query string");
}

const evtSource = new EventSource("/api/stream/" + marketId + "/" + traderId);

evtSource.addEventListener("pricePoint", (event) => {
    const pp = JSON.parse(event.data);
    console.log(pp);
});

evtSource.addEventListener("duplicate", () => {
    alert("Only one stream allowed per trader");
    evtSource.close();
});

const bidForm = { marketId: marketId, traderId: traderId, type: "bid", price: 100, quantity: 10 };
const askForm = { marketId: marketId, traderId: traderId, type: "ask", price: 100, quantity: 10 };

async function sendOrders() {
    const bidReq = new Request("/api/order", { method: "POST", body: JSON.stringify(bidForm) });
    const bidRes = await fetch(bidReq);
    console.log(bidRes);
    const askReq = new Request("/api/order", { method: "POST", body: JSON.stringify(askForm) });
    const askRes = await fetch(askReq);
}

setTimeout(sendOrders, 3000);
