#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Lukas - iPhone";
const char* password = "ESP32pripojeni";

#define LED_PIN 2

WebServer server(80);

// ====== HTML STRÁNKA ======
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="cs">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>RC Auto - Ovladac</title>
<style>
  * { box-sizing: border-box; }
  body {
    margin: 0;
    font-family: -apple-system, 'Segoe UI', Roboto, sans-serif;
    background: linear-gradient(160deg, #0f172a, #1e293b);
    color: #f1f5f9;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 24px 16px 60px;
  }
  h1 {
    font-size: 1.3rem;
    margin: 10px 0 4px;
  }
  p.sub {
    color: #94a3b8;
    font-size: 0.85rem;
    margin: 0 0 28px;
  }
  .status {
    display: inline-flex;
    align-items: center;
    gap: 8px;
    background: #1e293b;
    border: 1px solid #334155;
    border-radius: 999px;
    padding: 6px 16px;
    font-size: 0.8rem;
    margin-bottom: 28px;
  }
  .dot {
    width: 9px; height: 9px; border-radius: 50%;
    background: #475569;
    transition: background 0.2s;
  }
  .dot.on { background: #22c55e; box-shadow: 0 0 8px #22c55e; }

  .section {
    width: 100%;
    max-width: 380px;
    background: #1e293b;
    border: 1px solid #334155;
    border-radius: 18px;
    padding: 20px;
    margin-bottom: 18px;
  }
  .section h2 {
    font-size: 0.95rem;
    margin: 0 0 14px;
    color: #cbd5e1;
    display: flex;
    align-items: center;
    gap: 8px;
  }
  .btn-row {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 12px;
  }
  button {
    border: none;
    border-radius: 14px;
    padding: 18px 10px;
    font-size: 1rem;
    font-weight: 700;
    cursor: pointer;
    transition: transform 0.08s, filter 0.15s;
    -webkit-tap-highlight-color: transparent;
  }
  button:active { transform: scale(0.96); filter: brightness(0.9); }

  .btn-on {
    background: linear-gradient(135deg, #22c55e, #16a34a);
    color: white;
  }
  .btn-off {
    background: linear-gradient(135deg, #ef4444, #dc2626);
    color: white;
  }
  .btn-disabled {
    background: #334155;
    color: #64748b;
    cursor: not-allowed;
  }
  .placeholder {
    color: #64748b;
    font-size: 0.8rem;
    text-align: center;
    padding: 10px 0;
  }
</style>
</head>
<body>

  <h1>🚗 RC Auto</h1>
  <p class="sub">Ovládací panel ESP32</p>

  <div class="status">
    <span class="dot" id="statusDot"></span>
    <span id="statusText">LED: ?</span>
  </div>

  <div class="section">
    <h2>💡 LED</h2>
    <div class="btn-row">
      <button class="btn-on" onclick="send('/led/on')">ZAPNOUT</button>
      <button class="btn-off" onclick="send('/led/off')">VYPNOUT</button>
    </div>
  </div>

  <!-- Sem pridej dalsi sekce az budes rozsirovat (motory, servo...) -->
  <div class="section">
    <h2>🔧 Pohon (zatím neaktivní)</h2>
    <div class="btn-row">
      <button class="btn-disabled" disabled>VPŘED</button>
      <button class="btn-disabled" disabled>VZAD</button>
    </div>
    <p class="placeholder">Přidáme, až budeme mít zapojené motory</p>
  </div>

  <script>
    function send(path) {
      fetch(path)
        .then(r => r.text())
        .then(text => {
          const isOn = text.includes('ON');
          document.getElementById('statusDot').classList.toggle('on', isOn);
          document.getElementById('statusText').textContent = 'LED: ' + (isOn ? 'zapnuto' : 'vypnuto');
        })
        .catch(err => console.log('Chyba:', err));
    }
  </script>
</body>
</html>
)rawliteral";

// ====== HANDLERY ======
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleOn() {
  digitalWrite(LED_PIN, HIGH);
  server.send(200, "text/plain", "LED ON");
}

void handleOff() {
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "LED OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Pripojuji se k hotspotu");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Pripojeno! IP adresa ESP32: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led/on", handleOn);
  server.on("/led/off", handleOff);
  server.begin();
}

void loop() {
  server.handleClient();
}