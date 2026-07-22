const urlParams = new URLSearchParams(window.location.search);
const sessionCode = urlParams.get("session");
const sessionSubtitle = document.getElementById("session-subtitle");
const attendanceForm = document.getElementById("mark-attendance-form");
const markMessage = document.getElementById("mark-message");

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

        markMessage.classList.remove("hidden");
        markMessage.textContent = `Attendance marked, ${name}.`;
        markMessage.classList.add("success");
        attendanceForm.classList.add("hidden");
    }
});

const locationText = document.getElementById("location-text");
const sessionType = "face_to_face";

function updateLocationText() {
    if (sessionType === "online") {
        locationText.classList.add("hidden");
    }
    else {
        locationText.classList.remove("hidden");
    }
};

updateLocationText();




// update the text in the mark-message div when 
// the user marks attendance and then disable the form