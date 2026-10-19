# /// script
# requires-python = ">=3.13"
# dependencies = [
#     "flask>=3.1.3",
# ]
# ///
from flask import Flask, jsonify, request, render_template_string

app = Flask(__name__)

# Geht bei Container-Neustart wieder auf False zurück.
servo_on = False

HTML = """
<!doctype html>
<html lang="de">
<head>
  <meta charset="utf-8">
  <title>Servo-Steuerung</title>
</head>
<body>
  <h1>Door Lock</h1>
  <p>Current state: <strong id="state">Loading ...</strong></p>
  <button onclick="toggle()">Change state</button>

  <script>
    async function readState() {
      const response = await fetch("/api/state");
      const data = await response.json();
      document.getElementById("state").textContent =
        data.servoOn ? "UNLOCKED" : "LOCKED";
    }

    async function toggle() {
      await fetch("/api/toggle", { method: "POST" });
      readState();
    }

    readState();
    setInterval(readState, 1000);
  </script>
</body>
</html>
"""

@app.get("/")
def index():
    return render_template_string(HTML)

@app.get("/api/state")
def get_state():
    return jsonify({"servoOn": servo_on})

@app.post("/api/toggle")
def toggle():
    global servo_on
    servo_on = not servo_on
    return jsonify({"servoOn": servo_on})

app.run(host="0.0.0.0", port=5000)