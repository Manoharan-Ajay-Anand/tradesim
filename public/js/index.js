const createButton = document.getElementById("createButton");

createButton.addEventListener("click", createMarket)

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
