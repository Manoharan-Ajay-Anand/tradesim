const createButton = document.getElementById("createButton");
createButton.addEventListener("click", onClickCreate)

const joinButton = document.getElementById("joinButton");
joinButton.addEventListener("click", onClickJoin);

const marketInput = document.getElementById("marketInput");

async function onClickCreate(elem, event) {
    const marketId = marketInput.value;
    
    const request = new Request("/api/create", { method: "POST", body: marketId });
    const response = await fetch(request);
    const message = await response.text(); 
    if (!response.ok) {
        alert(message);
        return;
    }

    await joinMarket(marketId);
}

function decToHex(num) {
    return num.toString(16).padStart(2, "0");
}

function getRandomString(len) {
    let arr = new Uint8Array(len / 2);
    window.crypto.getRandomValues(arr);
    return Array.from(arr, decToHex).join("");
}

function onClickJoin() {
    joinMarket(marketInput.value);
}

async function joinMarket(marketId) {
    const traderId = getRandomString(30);
    const form = { marketId: marketId, traderId: traderId };
    const request = new Request("/api/join", { method: "POST", body: JSON.stringify(form) });
    const response = await fetch(request);
    const message = await response.text(); 
    if (!response.ok) {
        alert(message);
        return;
    }

    window.open("market.html?marketId=" + marketId + "&traderId=" + traderId);
}
