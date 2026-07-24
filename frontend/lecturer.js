
const API_BASE = (window.location.hostname === "localhost" || window.location.hostname === "127.0.0.1")
    ? "http://localhost:8080/api"
    : "https://nadd-attendance-marker.onrender.com/api";
let token = null;

const signUpForm = document.getElementById("sign-up-form");
const loginForm = document.getElementById("login-form");
const signUpBttns = document.querySelectorAll(".sign-up-bttn");
const loginBttns = document.querySelectorAll(".login-bttn");

signUpBttns.forEach(bttn => {
    bttn.addEventListener("click", () => {
        signUpForm.classList.remove("hidden");
        loginForm.classList.add("hidden");
    });
});

loginBttns.forEach(bttn => {
    bttn.addEventListener("click", () => {
        loginForm.classList.remove("hidden");
        signUpForm.classList.add("hidden");
    });
});

// sign up password fields
const passwordField = document.querySelector(".password-field");
const confirmPasswordField = document.querySelector(".confirm-password-field");
const password = signUpForm.querySelector("#lecturer-password");
const confirmPassword = signUpForm.querySelector("#lecturer-confirm-password");


passwordField.querySelector(".show-password").addEventListener("click", () => {
    passwordField.querySelector(".show-password").classList.remove("show");
    passwordField.querySelector(".hide-password").classList.add("show");
    password.type = "text";
});
passwordField.querySelector(".hide-password").addEventListener("click", () => {
    passwordField.querySelector(".hide-password").classList.remove("show");
    passwordField.querySelector(".show-password").classList.add("show");
    password.type = "password"; 
});

confirmPasswordField.querySelector(".show-password").addEventListener("click", () => {
    confirmPasswordField.querySelector(".show-password").classList.remove("show");
    confirmPasswordField.querySelector(".hide-password").classList.add("show");
    confirmPassword.type = "text";
});
confirmPasswordField.querySelector(".hide-password").addEventListener("click", () => {
    confirmPasswordField.querySelector(".hide-password").classList.remove("show");
    confirmPasswordField.querySelector(".show-password").classList.add("show");
    confirmPassword.type = "password";
});

loginForm.querySelector(".show-password").addEventListener("click", () => {
    loginForm.querySelector(".show-password").classList.remove("show");
    loginForm.querySelector(".hide-password").classList.add("show");
    loginForm.querySelector("#login-password").type = "text";
});

loginForm.querySelector(".hide-password").addEventListener("click", () => {
    loginForm.querySelector(".hide-password").classList.remove("show");
    loginForm.querySelector(".show-password").classList.add("show");
    loginForm.querySelector("#login-password").type = "password";
});



signUpForm.addEventListener("submit", async (e) => {
    e.preventDefault();
    let allValid = true;

    // input fields
    const name = signUpForm.querySelector("#lecturer-name").value.trim();
    const email = signUpForm.querySelector("#lecturer-email").value.trim();
    const repEmail = signUpForm.querySelector("#rep-email").value.trim();
    const courseCode = signUpForm.querySelector("#course-code").value.trim();
    const password = signUpForm.querySelector("#lecturer-password").value;
    const confirmPassword = signUpForm.querySelector("#lecturer-confirm-password").value;

    // error messages
    const errorMsgs = signUpForm.querySelectorAll(".error-msg");
    const nameError = signUpForm.querySelector(".name-error");
    const emailError = signUpForm.querySelector(".email-error");
    const repEmailError = signUpForm.querySelector(".rep-email-error");
    const courseCodeError = signUpForm.querySelector(".course-code-error");
    const passwordError = signUpForm.querySelector(".password-error");
    const confirmPasswordError = signUpForm.querySelector(".confirm-password-error");

    // Field validation
    if (!(/^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/.test(email))) {
        emailError.textContent = "Invalid email";
        allValid = false;
    }

    if (!(/^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/.test(repEmail))) {
        repEmailError.textContent = "Invalid email";
        allValid = false;
    }

    if (!(/\d/.test(password))) {
        passwordError.textContent = "Password must contain at least a number";
        allValid = false;
    }

    if (!(/[!@#$%^&*()_+\-=\[\]{};':"\\|,.<>\/?`~]/.test(password))) {
        passwordError.textContent = "Password must contain at least a symbol";
        allValid = false;
    }

    if (confirmPassword != password) {
        confirmPasswordError.textContent = "Password mismatch";
        allValid = false;
    }

    if (!name) {
        nameError.textContent = "Name field must be filled" ;
        allValid = false;
    } 

    if (!email) {
        emailError.textContent = "Email cannot be empty";
        allValid = false;
    } 

    if (!repEmail) {
        repEmailError.textContent = "Course rep's email cannot be left empty";
        allValid = false;
    }

    if (!password) {
        passwordError.textContent = "Password cannot be empty";
        allValid = false;
    }

    if (!confirmPassword) {
        confirmPasswordError.textContent = "You forgot to confirm password";
        allValid = false;
    }

    if (!courseCode) {
        courseCodeError.textContent = "Course code field cannot be empty";
        allValid = false;
    }


    if (allValid) {
        errorMsgs.forEach(error => {
            error.textContent = "";
        });

        const authMsg = document.querySelector(".auth-message");

        try {
            const res = await fetch(
                `${API_BASE}/lecturer/signup`,
                {
                    method : "POST",
                    headers : {"Content-Type" : "application/json"},
                    body : JSON.stringify({name, email, password, repEmail, courseCode})
                });
            const data = await res.json();

            if (data.status === "success") {
                signUpForm.classList.add("hidden");
                loginForm.classList.remove("hidden");
                authMsg.textContent = "";
                authMsg.classList.add("hidden");
            }
            else {
                authMsg.classList.remove("hidden");
                authMsg.textContent = data.reason;
            }

        }
        catch (error) {
            authMsg.classList.remove("hidden");
            authMsg.textContent = "Couldn't reach the server.";
        }
    }
});

loginForm.addEventListener("submit", async (e) => {
    e.preventDefault();
    let allValid = true;

    // input fields
    const loginEmail = loginForm.querySelector("#login-email").value.trim();
    const loginPassword = loginForm.querySelector("#login-password").value;
    const emailError = loginForm.querySelector(".email-error");
    const passwordError = loginForm.querySelector(".password-error");
    const errorMsgs = loginForm.querySelectorAll(".error-msg");

    if (!loginEmail) {
        emailError.textContent = "Email field cannot be empty";
        allValid = false;
    }

    if (!loginPassword) {
        passwordError.textContent = "Password field cannot be left empty";
        allValid = false;
    }

    if (allValid) {
        errorMsgs.forEach(error => {
            error.textContent = "";
        });
        const authMsg = document.querySelector(".auth-message");

        try {
            const res = await fetch(
                `${API_BASE}/lecturer/login`,
                {
                    method : "POST",
                    headers : {"Content-Type" : "application/json"},
                    body : JSON.stringify({email : loginEmail, password : loginPassword})
                });
            const data = await res.json();

            if (data.status === "success") {
                token = data.token;
                loginForm.classList.add("hidden");
                document.getElementById("start-session-form").classList.remove("hidden");
                authMsg.textContent = "";
                authMsg.classList.add("hidden");
            }
            else {
                const authMsg = document.querySelector(".auth-message");
                authMsg.classList.remove("hidden");
                authMsg.textContent = data.reason;
            }
        }
        catch (error) {
            const authMsg = document.querySelector(".auth-message");
            authMsg.classList.remove("hidden");
            authMsg.textContent = "Couldn't reach server";
        }
    }
});

// Start session form validation

const startSessionForm = document.getElementById("start-session-form");

startSessionForm.addEventListener("submit", (e) => {
    e.preventDefault();

    const faceToFaceRadio = startSessionForm.querySelector("#face-to-face-session");
    const onlineRadio = startSessionForm.querySelector("#online-session");
    const authMsg = document.querySelector(".auth-message");
    let allValid = true;
    if (!faceToFaceRadio.checked && !onlineRadio.checked) {
        authMsg.classList.remove("hidden");
        authMsg.textContent = "Select one of the session types to continue";
        allValid = false;
    }

    if (allValid) {
        authMsg.classList.add("hidden");
        authMsg.textContent = "";
        startSessionForm.classList.add("hidden");
        document.querySelector(".live-session").classList.remove("hidden");
    }

})

// Live session manual attendance marker form validation

const manualMarkForm = document.getElementById("manual-mark-form");

manualMarkForm.addEventListener("submit", (e) => {
    e.preventDefault();

    const name = manualMarkForm.querySelector("#student-name").value.trim();
    const id = manualMarkForm.querySelector("#student-id").value.trim();
    const errorMsgs = manualMarkForm.querySelectorAll(".error-msg");
    const nameError = manualMarkForm.querySelector(".name-error");
    const idError = manualMarkForm.querySelector(".id-error");
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
            
    }
});


document.querySelector(".start-session-bttn").addEventListener("click", () => {
    document.querySelector(".closed-session").classList.add("hidden");
    document.getElementById("start-session-form").classList.remove("hidden");
});