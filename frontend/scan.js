const API_BASE = (window.location.hostname === "localhost" || window.location.hostname === "127.0.0.1")
    ? "http://localhost:8080/api"
    : "https://nadd-attendance-marker.onrender.com/api";

const urlParams = new URLSearchParams(window.location.search);
const sessionCode = urlParams.get("session");
const sessionSubtitle = document.getElementById("session-subtitle");
const attendanceForm = document.getElementById("mark-attendance-form");
const markMessage = document.getElementById("mark-message");

let sessionType = null;
const locationText = document.getElementById("location-text");



function checkSession() {
    if (!sessionCode) {
        sessionSubtitle.textContent = "No session code was found in this link";
        attendanceForm.classList.add("hidden");
    }
    else {
        sessionSubtitle.textContent = `Session ${sessionCode}`;
    }
}

checkSession();  


let deviceID = localStorage.getItem("device_ID");
if (!deviceID) {
    deviceID = crypto.randomUUID();
    localStorage.setItem("device_ID", deviceID);
}


function updateLocationText() {
    if (sessionType === "online") {
        locationText.classList.add("hidden");
    }
    else {
        locationText.classList.remove("hidden");
    }
};


async function loadSessionType() {
    if (!sessionCode) return;

    try {
        const res = await fetch(`${API_BASE}/session/${sessionCode}/status`);
        const data = await res.json();

        if (res.ok) {
            sessionType = data.sessionType;
            updateLocationText();

            if (data.closed) {
                sessionSubtitle.textContent = `Session ${sessionCode} has ended`;
                attendanceForm.classList.add("hidden");
            }
        }
        else {
            sessionSubtitle.textContent = "This session could not be found";
            attendanceForm.classList.add("hidden");
        }
    }
    catch (error) {
        sessionSubtitle.textContent = "Couldn't reach the server";
        attendanceForm.classList.add("hidden");
    }
}


loadSessionType();


attendanceForm.addEventListener("submit", (e) => {
    e.preventDefault();

    const name = attendanceForm.querySelector("#student-name").value.trim();
    const id = attendanceForm.querySelector("#student-id").value.trim();
    const errorMsgs = attendanceForm.querySelectorAll(".error-msg");
    const nameError = attendanceForm.querySelector(".name-error");
    const idError = attendanceForm.querySelector(".id-error");
    const markBttn = attendanceForm.querySelector("#mark-bttn");
    let allValid = true;

    if (!name) {
        allValid = false;
        nameError.textContent = "Name field cannot be empty";
    }

    if (!id) {
        allValid = false;
        idError.textContent = "ID field cannot be empty";
    }

    if (allValid) {
        errorMsgs.forEach(error => {
            error.textContent = "";
        });
        markBttn.disabled = true;
        markBttn.textContent = "Marking...";

        markMessage.classList.add("hidden");
        markMessage.textContent = "";

        const requestBody = {sessionCode, name, studentID: id, deviceID};

        if (sessionType === "face_to_face") {
            navigator.geolocation.getCurrentPosition(async (position) => {
                requestBody.lat = position.coords.latitude;
                requestBody.lng = position.coords.longitude;
                await submitAttendance(requestBody, markBttn);
            },
            () => {
                markBttn.disabled = false;
                markBttn.textContent = "Mark attendance";
                markMessage.classList.remove("hidden");
                markMessage.textContent = "Unable to get your location. Enable location and try again";
            });
        }
        else {
            submitAttendance(requestBody, markBttn);
        }
    }
});


async function submitAttendance(requestBody, markBttn) {
    try {
        const res = await fetch(`${API_BASE}/attendance/mark`, {
            method : "POST",
            headers : {"Content-Type": "application/json"},
            body : JSON.stringify(requestBody)
        });

        const data = await res.json();

        if (data.status === "success") {
            markMessage.classList.remove("hidden");
            markMessage.textContent = `${requestBody.name}, your attendance has been marked successfully.`;
            markMessage.classList.add("success");
            attendanceForm.classList.add("hidden");
        }
        else {
            markBttn.disabled = false;
            markBttn.textContent = "Mark attendance";
            markMessage.classList.remove("hidden");
            markMessage.textContent = data.reason;
        }
    }
    catch (error) {
        markBttn.disabled = false;
        markBttn.textContent = "Mark attendance";
        markMessage.classList.remove("hidden");
        markMessage.textContent = "Couldn't reach the server.";
    }
}