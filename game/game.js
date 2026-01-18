// Game canvas and context
const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

// Game state
let gameState = 'start'; // 'start', 'playing', 'gameOver'
let score = 0;
let gameSpeed = 2;
let pipes = [];
let device = null;

// Images
let seagullImage = new Image();
let cloudImage = new Image();
let pipeTopImage = new Image();
let pipeBottomImage = new Image();

// Seagull properties
const seagull = {
    x: 200,
    y: canvas.height / 2,
    width: 120,
    height: 90,
    fallSpeed: 0.3,
    jumpStrength: -30,
    color: '#ffffff'
};

// Clouds array for background
let clouds = [];

// Bread items
let breadItems = [];
let breadImage = new Image();

// Pipe properties
const pipeWidth = 60;
const pipeGap = 300;  // Bigger holes between pipes
const pipeSpacing = 300;

// Button press handling
let buttonPressed = false;
let lastButtonPress = 0;

// Initialize game
function init() {
    // Load seagull image (better clipart style)
    const seagullSVG = `
        <svg width="100" height="80" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 80">
            <!-- Wing extended -->
            <path d="M 10 25 Q 0 15, 0 30 Q 0 45, 10 50 Q 20 55, 30 45 Q 25 35, 20 30 Q 15 25, 10 25 Z" 
                  fill="#f5f5f5" stroke="#999" stroke-width="2"/>
            <path d="M 5 28 Q 3 30, 5 32 M 12 35 Q 10 37, 12 39" stroke="#ccc" stroke-width="1" fill="none"/>
            
            <!-- Body -->
            <ellipse cx="45" cy="38" rx="25" ry="16" fill="#ffffff" stroke="#888" stroke-width="2"/>
            
            <!-- Head -->
            <circle cx="62" cy="22" r="12" fill="#ffffff" stroke="#888" stroke-width="2"/>
            
            <!-- Eye -->
            <circle cx="65" cy="20" r="4" fill="#000000"/>
            <circle cx="66.5" cy="19" r="1.5" fill="#ffffff"/>
            
            <!-- Beak -->
            <path d="M 70 21 L 82 22 L 82 25 L 70 24 Z" fill="#FFA500" stroke="#FF8C00" stroke-width="2"/>
            <path d="M 72 22 L 76 22.5 L 76 23.5 Z" fill="#FF8C00"/>
            
            <!-- Tail -->
            <path d="M 20 36 Q 5 32, 0 38 Q 5 44, 20 40 Z" fill="#ffffff" stroke="#888" stroke-width="2"/>
            
            <!-- Feet -->
            <circle cx="38" cy="52" r="4" fill="#FFA500"/>
            <circle cx="52" cy="52" r="4" fill="#FFA500"/>
            <line x1="36" y1="52" x2="34" y2="58" stroke="#FFA500" stroke-width="3" stroke-linecap="round"/>
            <line x1="40" y1="52" x2="42" y2="58" stroke="#FFA500" stroke-width="3" stroke-linecap="round"/>
            <line x1="50" y1="52" x2="48" y2="58" stroke="#FFA500" stroke-width="3" stroke-linecap="round"/>
            <line x1="54" y1="52" x2="56" y2="58" stroke="#FFA500" stroke-width="3" stroke-linecap="round"/>
        </svg>
    `;
    const blob1 = new Blob([seagullSVG], { type: 'image/svg+xml' });
    seagullImage.src = URL.createObjectURL(blob1);
    
    // Load cloud image
    const cloudSVG = `
        <svg width="120" height="80" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 120 80">
            <circle cx="25" cy="40" r="18" fill="#ffffff" opacity="0.9"/>
            <circle cx="45" cy="35" r="22" fill="#ffffff" opacity="0.9"/>
            <circle cx="65" cy="40" r="20" fill="#ffffff" opacity="0.9"/>
            <circle cx="85" cy="42" r="15" fill="#ffffff" opacity="0.9"/>
            <circle cx="40" cy="50" r="16" fill="#ffffff" opacity="0.9"/>
            <circle cx="60" cy="52" r="14" fill="#ffffff" opacity="0.9"/>
        </svg>
    `;
    const blob2 = new Blob([cloudSVG], { type: 'image/svg+xml' });
    cloudImage.src = URL.createObjectURL(blob2);
    
    // Load pipe images
    const pipeTopSVG = `
        <svg width="80" height="400" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 400">
            <rect x="0" y="0" width="80" height="400" fill="#2d5016"/>
            <rect x="5" y="0" width="70" height="400" fill="#3a6b20"/>
            <rect x="0" y="390" width="80" height="10" fill="#1a3009"/>
            <rect x="10" y="395" width="60" height="5" fill="#2d5016"/>
        </svg>
    `;
    const blob3 = new Blob([pipeTopSVG], { type: 'image/svg+xml' });
    pipeTopImage.src = URL.createObjectURL(blob3);
    
    const pipeBottomSVG = `
        <svg width="80" height="400" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 80 400">
            <rect x="0" y="0" width="80" height="400" fill="#2d5016"/>
            <rect x="5" y="0" width="70" height="400" fill="#3a6b20"/>
            <rect x="0" y="0" width="80" height="10" fill="#1a3009"/>
            <rect x="10" y="0" width="60" height="5" fill="#2d5016"/>
        </svg>
    `;
    const blob4 = new Blob([pipeBottomSVG], { type: 'image/svg+xml' });
    pipeBottomImage.src = URL.createObjectURL(blob4);
    
    // Load bread image
    const breadSVG = `
        <svg width="30" height="20" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 30 20">
            <!-- Bread slice -->
            <ellipse cx="15" cy="10" rx="14" ry="8" fill="#D4A574" stroke="#B8956A" stroke-width="1"/>
            <ellipse cx="15" cy="10" rx="12" ry="6" fill="#E8C599"/>
            <!-- Crust details -->
            <path d="M 3 10 Q 8 8, 12 10 Q 16 12, 20 10 Q 24 8, 27 10" stroke="#B8956A" stroke-width="1.5" fill="none"/>
            <circle cx="10" cy="9" r="1" fill="#C49D69" opacity="0.6"/>
            <circle cx="20" cy="11" r="1" fill="#C49D69" opacity="0.6"/>
        </svg>
    `;
    const blob5 = new Blob([breadSVG], { type: 'image/svg+xml' });
    breadImage.src = URL.createObjectURL(blob5);
    
    // Connect button event
    document.getElementById('connectBtn').addEventListener('click', connectJoystick);
    document.getElementById('startBtn').addEventListener('click', startGame);
    document.getElementById('restartBtn').addEventListener('click', restartGame);
    
    // Keyboard fallback (spacebar)
    document.addEventListener('keydown', (e) => {
        if (e.code === 'Space') {
            e.preventDefault();
            if (gameState === 'playing') {
                jump();
            }
        }
    });
    
    // Initial pipe
    generatePipe(canvas.width + 100);
    
    // Initialize clouds
    initClouds();
}

// Connect to HID joystick
async function connectJoystick() {
    const statusText = document.getElementById('connectionStatus');
    const connectBtn = document.getElementById('connectBtn');
    
    try {
        // Request access to HID devices
        // usagePage 0x01 = Generic Desktop Controls
        // usage 0x04 = Joystick
        const filters = [
            { usagePage: 0x01, usage: 0x04 }  // Generic Desktop Joystick
        ];
        
        const devices = await navigator.hid.requestDevice({ filters });
        
        if (devices.length === 0) {
            statusText.textContent = 'No device selected';
            return;
        }
        
        device = devices[0];
        
        // Open the device
        if (!device.opened) {
            await device.open();
        }
        
        // Log device info for debugging
        console.log('HID Device connected:', {
            vendorId: '0x' + device.vendorId.toString(16),
            productId: '0x' + device.productId.toString(16),
            productName: device.productName,
            opened: device.opened
        });
        console.log('Collections:', device.collections);
        
        // Check for input reports in collections
        if (device.collections && device.collections.length > 0) {
            device.collections.forEach((collection, idx) => {
                console.log(`Collection ${idx}:`, {
                    usagePage: '0x' + collection.usagePage.toString(16),
                    usage: '0x' + collection.usage.toString(16),
                    inputReports: collection.inputReports,
                    outputReports: collection.outputReports,
                    featureReports: collection.featureReports
                });
            });
        }
        
        // Listen for input reports
        device.addEventListener('inputreport', handleInputReport);
        
        statusText.textContent = '✓ Joystick connected!';
        statusText.classList.add('connected');
        connectBtn.textContent = 'Connected';
        connectBtn.disabled = true;
        
        // Show start button
        document.getElementById('startBtn').classList.remove('hidden');
        
    } catch (error) {
        console.error('Error connecting to joystick:', error);
        statusText.textContent = 'Error: ' + error.message;
        statusText.style.color = '#f44336';
    }
}

// Handle input report from HID device
function handleInputReport(event) {
    if (event.device !== device) return;
    
    const data = new Uint8Array(event.data.buffer);
    
    // Debug: log all received reports
    console.log('HID Input Report received:', {
        reportId: event.reportId,
        dataLength: data.length,
        data: Array.from(data).map(b => '0x' + b.toString(16).padStart(2, '0')).join(' ')
    });
    
    // Parse the joystick report
    // Format: [x (int8), y (int8), buttons (uint8)]
    // From main.c: joy_report_t { int8_t x; int8_t y; uint8_t buttons; }
    if (data.length >= 3) {
        const buttons = data[2];
        
        // Check if button 1 is pressed (bit 0 = 0x01)
        const isButtonPressed = (buttons & 0x01) !== 0;
        
        // Detect button press (edge detection)
        if (isButtonPressed && !buttonPressed) {
            buttonPressed = true;
            const now = Date.now();
            
            // Flash the blink indicator
            const indicator = document.getElementById('blinkIndicator');
            if (indicator) {
                indicator.classList.add('active');
                setTimeout(() => {
                    indicator.classList.remove('active');
                }, 150); // Flash for 150ms
            }
            
            // Debounce: only jump if enough time has passed since last press
            if (now - lastButtonPress > 100) {
                lastButtonPress = now;
                if (gameState === 'playing') {
                    jump();
                }
            }
        } else if (!isButtonPressed) {
            buttonPressed = false;
        }
    }
}

// Start the game
function startGame() {
    gameState = 'playing';
    document.getElementById('startScreen').classList.add('hidden');
    document.getElementById('gameOverScreen').classList.add('hidden');
    
    // Reset game state
    score = 0;
    pipes = [];
    breadItems = [];
    seagull.y = canvas.height / 2;
    gameSpeed = 2;
    
    // Generate initial pipes
    generatePipe(canvas.width + 100);
    
    // Reinitialize clouds
    initClouds();
    
    // Generate initial bread items
    for (let i = 0; i < 3; i++) {
        generateBread(canvas.width + 200 + i * 400);
    }
    
    // Start game loop
    gameLoop();
}

// Restart the game
function restartGame() {
    startGame();
}

// Jump function - move up directly (no velocity accumulation)
function jump() {
    seagull.y += seagull.jumpStrength; // Move up by jump strength
}

// Generate a new pipe
function generatePipe(x) {
    const minHeight = 100;
    const maxHeight = canvas.height - pipeGap - minHeight;
    const topHeight = Math.random() * (maxHeight - minHeight) + minHeight;
    
    pipes.push({
        x: x,
        topHeight: topHeight,
        bottomY: topHeight + pipeGap,
        bottomHeight: canvas.height - (topHeight + pipeGap),
        passed: false
    });
}

// Generate a cloud
function generateCloud(x) {
    const size = Math.random() * 40 + 30; // Random size between 30-70
    const y = Math.random() * (canvas.height * 0.5) + 20; // In upper half of sky
    
    clouds.push({
        x: x,
        y: y,
        size: size,
        speed: Math.random() * 0.5 + 0.3 // Slow drift speed
    });
}

// Generate a bread item
function generateBread(x) {
    const minY = 50;
    const maxY = canvas.height * 0.7 - 50; // Above the ocean
    const y = Math.random() * (maxY - minY) + minY;
    
    breadItems.push({
        x: x,
        y: y,
        width: 30,
        height: 20,
        collected: false
    });
}

// Initialize clouds
function initClouds() {
    clouds = [];
    // Generate initial clouds spread across the canvas
    for (let i = 0; i < 5; i++) {
        generateCloud(Math.random() * canvas.width);
    }
}

// Check collision
function checkCollision() {
    const birdRect = {
        x: seagull.x,
        y: seagull.y,
        width: seagull.width,
        height: seagull.height
    };
    
    // Check boundaries
    if (birdRect.y + birdRect.height > canvas.height || birdRect.y < 0) {
        return true;
    }
    
    // Check pipe collisions
    for (let pipe of pipes) {
        const topRect = {
            x: pipe.x,
            y: 0,
            width: pipeWidth,
            height: pipe.topHeight
        };
        
        const bottomRect = {
            x: pipe.x,
            y: pipe.bottomY,
            width: pipeWidth,
            height: pipe.bottomHeight
        };
        
        if (isColliding(birdRect, topRect) || isColliding(birdRect, bottomRect)) {
            return true;
        }
    }
    
    return false;
}

// Rectangle collision detection
function isColliding(rect1, rect2) {
    return rect1.x < rect2.x + rect2.width &&
           rect1.x + rect1.width > rect2.x &&
           rect1.y < rect2.y + rect2.height &&
           rect1.y + rect1.height > rect2.y;
}

// Draw a cloud using image
function drawCloud(x, y, size) {
    if (cloudImage.complete) {
        ctx.drawImage(cloudImage, x, y - size / 2, size * 1.5, size);
    } else {
        // Fallback while loading
        ctx.fillStyle = '#ffffff';
        ctx.beginPath();
        ctx.arc(x, y, size * 0.6, 0, 2 * Math.PI);
        ctx.fill();
    }
}

// Update game
function update() {
    if (gameState !== 'playing') return;
    
    // Update seagull physics - constant fall speed (no acceleration)
    seagull.y += seagull.fallSpeed;
    
    // Update pipes
    for (let i = pipes.length - 1; i >= 0; i--) {
        pipes[i].x -= gameSpeed;
        
        // Check if pipe was passed
        if (!pipes[i].passed && pipes[i].x + pipeWidth < seagull.x) {
            pipes[i].passed = true;
            score++;
            gameSpeed += 0.1; // Gradually increase speed
            document.getElementById('score').textContent = score;
        }
        
        // Remove pipes that are off screen
        if (pipes[i].x + pipeWidth < 0) {
            pipes.splice(i, 1);
        }
    }
    
    // Generate new pipes
    const lastPipe = pipes[pipes.length - 1];
    if (lastPipe && lastPipe.x < canvas.width - pipeSpacing) {
        generatePipe(canvas.width + 100);
    }
    
    // Update clouds (slow drift)
    for (let i = clouds.length - 1; i >= 0; i--) {
        clouds[i].x -= clouds[i].speed;
        
        // Remove clouds that are off screen
        if (clouds[i].x + clouds[i].size * 2 < 0) {
            clouds.splice(i, 1);
        }
    }
    
    // Generate new clouds occasionally
    if (clouds.length < 5 && Math.random() < 0.02) {
        generateCloud(canvas.width + 100);
    }
    
    // Update bread items
    for (let i = breadItems.length - 1; i >= 0; i--) {
        breadItems[i].x -= gameSpeed;
        
        // Check collision with seagull
        if (!breadItems[i].collected) {
            const breadRect = {
                x: breadItems[i].x,
                y: breadItems[i].y,
                width: breadItems[i].width,
                height: breadItems[i].height
            };
            
            const seagullRect = {
                x: seagull.x,
                y: seagull.y,
                width: seagull.width,
                height: seagull.height
            };
            
            if (isColliding(seagullRect, breadRect)) {
                breadItems[i].collected = true;
                score += 5; // Award 5 points for bread
                document.getElementById('score').textContent = score;
            }
        }
        
        // Remove bread that's off screen or collected
        if (breadItems[i].x + breadItems[i].width < 0 || breadItems[i].collected) {
            breadItems.splice(i, 1);
        }
    }
    
    // Generate new bread items occasionally
    if (breadItems.length < 2 && Math.random() < 0.01) {
        generateBread(canvas.width + 100);
    }
    
    // Check collision
    if (checkCollision()) {
        gameOver();
    }
}

// Draw game
function draw() {
    // Clear canvas
    ctx.fillStyle = '#87CEEB'; // Sky blue
    ctx.fillRect(0, 0, canvas.width, canvas.height * 0.7);
    
    // Draw clouds in background
    ctx.fillStyle = '#ffffff';
    ctx.strokeStyle = '#e0e0e0';
    ctx.lineWidth = 1;
    for (let cloud of clouds) {
        drawCloud(cloud.x, cloud.y, cloud.size);
    }
    
    // Draw ocean with waves and texture
    const oceanY = canvas.height * 0.7;
    const oceanHeight = canvas.height * 0.3;
    
    // Base ocean color (gradient from lighter blue to deeper blue)
    const gradient = ctx.createLinearGradient(0, oceanY, 0, canvas.height);
    gradient.addColorStop(0, '#4A90E2'); // Light blue near surface
    gradient.addColorStop(0.5, '#2E6DA4'); // Medium blue
    gradient.addColorStop(1, '#1E4D72'); // Darker blue at bottom
    
    ctx.fillStyle = gradient;
    ctx.fillRect(0, oceanY, canvas.width, oceanHeight);
    
    // Draw wave patterns
    ctx.strokeStyle = '#3A7CB4';
    ctx.lineWidth = 2;
    const time = Date.now() * 0.001;
    
    for (let i = 0; i < 5; i++) {
        ctx.beginPath();
        const waveY = oceanY + (oceanHeight * 0.2) * (i / 5) + Math.sin(time + i) * 3;
        ctx.moveTo(0, waveY);
        for (let x = 0; x < canvas.width; x += 20) {
            const y = waveY + Math.sin((x / 50) + time + i) * 5;
            ctx.lineTo(x, y);
        }
        ctx.lineTo(canvas.width, waveY);
        ctx.stroke();
    }
    
    // Draw some water texture (bubbles/foam)
    ctx.fillStyle = 'rgba(255, 255, 255, 0.1)';
    for (let i = 0; i < 20; i++) {
        const bubbleX = (i * 40 + Math.sin(time * 0.5 + i) * 30) % canvas.width;
        const bubbleY = oceanY + Math.random() * oceanHeight;
        ctx.beginPath();
        ctx.arc(bubbleX, bubbleY, 3, 0, 2 * Math.PI);
        ctx.fill();
    }
    
    // Draw pipes using images
    for (let pipe of pipes) {
        if (pipeTopImage.complete && pipeBottomImage.complete) {
            // Top pipe (flipped)
            ctx.save();
            ctx.translate(pipe.x, 0);
            ctx.scale(1, -1);
            ctx.drawImage(pipeTopImage, 0, -pipe.topHeight, pipeWidth, pipe.topHeight);
            ctx.restore();
            
            // Bottom pipe
            ctx.drawImage(pipeBottomImage, pipe.x, pipe.bottomY, pipeWidth, pipe.bottomHeight);
        } else {
            // Fallback while loading
            ctx.fillStyle = '#2d5016';
            ctx.fillRect(pipe.x, 0, pipeWidth, pipe.topHeight);
            ctx.fillRect(pipe.x, pipe.bottomY, pipeWidth, pipe.bottomHeight);
        }
    }
    
    // Draw bread items
    for (let bread of breadItems) {
        if (!bread.collected && breadImage.complete) {
            ctx.drawImage(breadImage, bread.x, bread.y, bread.width, bread.height);
        } else if (!bread.collected) {
            // Fallback while loading
            ctx.fillStyle = '#D4A574';
            ctx.beginPath();
            ctx.ellipse(bread.x + bread.width / 2, bread.y + bread.height / 2, bread.width / 2, bread.height / 2, 0, 0, 2 * Math.PI);
            ctx.fill();
        }
    }
    
    // Draw seagull using image
    if (seagullImage.complete) {
        ctx.drawImage(seagullImage, seagull.x, seagull.y, seagull.width, seagull.height);
    } else {
        // Fallback: simple white circle while image loads
        ctx.fillStyle = '#ffffff';
        ctx.beginPath();
        ctx.arc(seagull.x + seagull.width / 2, seagull.y + seagull.height / 2, seagull.width / 2, 0, 2 * Math.PI);
        ctx.fill();
    }
}

// Game over
function gameOver() {
    gameState = 'gameOver';
    document.getElementById('finalScore').textContent = score;
    document.getElementById('gameOverScreen').classList.remove('hidden');
}

// Game loop
function gameLoop() {
    if (gameState === 'playing') {
        update();
        draw();
        requestAnimationFrame(gameLoop);
    }
}

// Initialize when page loads
window.addEventListener('load', init);