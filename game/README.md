# Seagull Flapper Game

A smooth Flappy Bird-style game controlled by blinking via an HID joystick device.

## How to Play

1. **Connect Your HID Joystick**
   - Click the "Connect Joystick" button
   - Select your HID joystick device from the browser prompt
   - Wait for the "✓ Joystick connected!" message

2. **Start the Game**
   - Click "Start Game" once your joystick is connected
   - Blink to make the seagull fly upward!
   - Navigate through the pipes without hitting them

3. **Scoring**
   - Score increases by 1 for each pipe you successfully pass
   - Game speed gradually increases as you progress

4. **Fallback Controls**
   - You can also use the **Spacebar** key as a fallback if the HID joystick isn't connected

## Technical Details

The game uses the WebHID API to communicate with your blink-controlled HID joystick. The joystick sends reports with the following format:
- **X** (int8_t): Horizontal joystick position
- **Y** (int8_t): Vertical joystick position  
- **Buttons** (uint8_t): Button state bitmap (bit 0 = Button 1)

The game detects button presses (specifically bit 0 of the buttons byte) and translates them into upward movement for the seagull.

## Game Features

- 🐦 Smooth seagull animation with rotation based on velocity
- 🚧 Moving pipes with random heights
- 📊 Score tracking
- 🎮 HID joystick integration via WebHID API
- ⌨️ Keyboard fallback (Spacebar)
- 🎨 Clean, modern UI

## Browser Requirements

- Chrome/Edge 89+ or Opera 75+ (WebHID API support required)
- For other browsers, you can use the keyboard fallback (Spacebar)

## Running the Game

Simply open `index.html` in a compatible web browser. No server required, but you may need to enable local file access depending on your browser settings.

For best results, serve via a local web server:
```bash
# Python 3
python3 -m http.server 8000

# Python 2
python -m SimpleHTTPServer 8000

# Node.js (with http-server installed)
npx http-server
```

Then navigate to `http://localhost:8000` in your browser.