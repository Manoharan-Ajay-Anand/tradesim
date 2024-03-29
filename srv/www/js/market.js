const searchParams = new URLSearchParams(window.location.search);

const marketId = searchParams.get("marketId");
const traderId = searchParams.get("traderId");

if (!marketId || !traderId) {
    alert("MarketId and TraderId not specified in query string");
}

// const evtSource = new EventSource("/api/stream/" + marketId + "/" + traderId);

// evtSource.addEventListener("pricePoint", (event) => {
//     const pp = JSON.parse(event.data);
//     console.log(pp);
// });

// evtSource.addEventListener("duplicate", () => {
//     alert("Only one stream allowed per trader");
//     evtSource.close();
// });

const priceTable = document.getElementById("priceTable");
for (let i = 1; i <= 50; ++i) {
    const row = document.createElement("tr");

    const bid = document.createElement("td");
    bid.classList.add("invisible", "cell");
    bid.textContent = "0";

    const price = document.createElement("td");
    price.classList.add("cell");
    price.textContent = i.toString();

    const ask = document.createElement("td");
    ask.classList.add("invisible", "cell");
    
    row.append(bid, price, ask);
    priceTable.prepend(row);
}

