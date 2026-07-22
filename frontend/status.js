const urlParams = new URLSearchParams(window.location.search);
const sessionCode = urlParams.get("session");
const statusSubtitle = document.getElementById("status-subtitle");

const typeBadge = document.getElementById("type-badge");
const timeRemaining = document.getElementById("time-remaining");
const markedCountEl = document.getElementById("marked-count");
let secondsRemaining = 5;
const markedCount= 52;
const sessionType = "face_to_face";
let pollInterval;

function checkSession() {
    if (!sessionCode) {
        statusSubtitle.textContent = "No session code was found in this link";
        document.querySelectorAll(".stats-card").forEach(card => card.classList.add("hidden"));
        typeBadge.classList.add("hidden");
    }
    else {
        statusSubtitle.textContent = `Session ${sessionCode} · live attendance`;
    }
}


checkSession();


function showSessionEnded() {
    statusSubtitle.textContent = "Session ended";
    typeBadge.textContent = "Session closed";
    clearInterval(pollInterval);
}


function updateStatusDisplay(secondsRemaining, marked, sessionType) {
    const minutes = Math.floor(secondsRemaining / 60);
    const seconds = secondsRemaining % 60;
    
    timeRemaining.textContent = `${String(minutes).padStart(2, "0")}:${String(seconds).padStart(2, "0")}`;
    markedCountEl.textContent = marked;
    typeBadge.textContent = (sessionType === "online") ? "Online session" : "face-to-face session in progress";
}


function pollStatus() {
    if (secondsRemaining <= 0) {
        secondsRemaining = 0;
        showSessionEnded();
        return; 
    }

    secondsRemaining -= 1;

    updateStatusDisplay(secondsRemaining, markedCount, sessionType);     
}

if (sessionCode) {
    pollStatus();
    pollInterval = setInterval(pollStatus, 1000);
}