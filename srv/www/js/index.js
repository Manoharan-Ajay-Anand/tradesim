const createButton = document.getElementById("createButton");
createButton.addEventListener("click", createMarket)

const joinButton = document.getElementById("joinButton");
joinButton.addEventListener("click", joinMarket);

const marketInput = document.getElementById("marketInput");

async function createMarket(elem, event) {
    const marketId = marketInput.value;
    
    const request = new Request("/api/create", { method: "POST", body: marketId });
    const response = await fetch(request);
    const message = await response.text(); 
    if (!response.ok) {
        alert(message);
        return;
    }

    console.log(message);
}

function decToHex(num) {
    return num.toString(16).padStart(2, "0");
}

function getRandomString(len) {
    let arr = new Uint8Array(len / 2);
    window.crypto.getRandomValues(arr);
    return Array.from(arr, decToHex).join("");
}

async function joinMarket(elem, event) {
    const form = { marketId: marketInput.value, traderId: getRandomString(30) };
    const request = new Request("/api/join", { method: "POST", body: JSON.stringify(form) });
    const response = await fetch(request);
    const message = await response.text(); 
    if (!response.ok) {
        alert(message);
        return;
    }

    console.log(message);
}
