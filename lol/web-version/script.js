const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

// UI Elements
const uiMainMenu = document.getElementById('mainMenu');
const uiGameOver = document.getElementById('gameOver');
const uiLeaderboard = document.getElementById('leaderboardView');
const uiInGame = document.getElementById('inGameUI');
const elCurrentScore = document.getElementById('currentScore');
const elFinalScore = document.getElementById('finalScore');
const elBestScore = document.getElementById('bestScore');
const elLeaderboardList = document.getElementById('leaderboardList');

// Buttons
const btnLeadMenu = document.getElementById('leadBtnMenu');
const btnLeadGO = document.getElementById('leadBtnGO');
const btnRetry = document.getElementById('retryBtn');
const btnBack = document.getElementById('backBtn');

// Game Constants
const W = canvas.width;
const H = canvas.height;
const GROUND_H = 60;
const GRAVITY = 800;
const JUMP_VELOCITY = -330;
const MAX_VELOCITY = 500;
const PIPE_WIDTH = 55;
const GAP_SIZE = 150;
const PIPE_SPEED = 180;
const SPAWN_INTERVAL = 1.8;

// Game State
let state = 'MENU'; // MENU, PLAYING, GAMEOVER, LEADERBOARD
let score = 0;
let lastTime = 0;
let flashTimer = 0;
let animationFrameId;

// Entities
let bird = { x: 80, y: H / 2, vy: 0, rotation: 0, radius: 15 };
let pipes = [];
let spawnTimer = 0;
let clouds = [];

// High Scores
let highScores = JSON.parse(localStorage.getItem('flappyScores')) || [];

function initClouds() {
    clouds = [];
    for (let i = 0; i < 5; i++) {
        clouds.push({
            x: Math.random() * W,
            y: 30 + Math.random() * 200,
            speed: 15 + Math.random() * 20,
            r: 18 + Math.random() * 12
        });
    }
}

function resetGame() {
    bird.y = H / 2;
    bird.vy = 0;
    bird.rotation = 0;
    pipes = [];
    score = 0;
    spawnTimer = 0;
    updateScoreUI();
}

function spawnPipe() {
    const minY = GAP_SIZE / 2 + 60;
    const maxY = H - GROUND_H - GAP_SIZE / 2 - 60;
    const gapCenterY = minY + Math.random() * (maxY - minY);
    
    pipes.push({
        x: W,
        gapCenterY: gapCenterY,
        scored: false
    });
}

function update(dt) {
    // Clouds
    clouds.forEach(c => {
        c.x += c.speed * dt;
        if (c.x > W + 60) c.x = -60;
    });

    if (state !== 'PLAYING') return;

    // Bird Physics
    bird.vy += GRAVITY * dt;
    if (bird.vy > MAX_VELOCITY) bird.vy = MAX_VELOCITY;
    bird.y += bird.vy * dt;

    // Bird Rotation
    let targetRot = bird.vy * 0.12;
    if (targetRot > 60) targetRot = 60;
    if (targetRot < -30) targetRot = -30;
    bird.rotation += (targetRot - bird.rotation) * 8 * dt;

    // Floor/Ceiling collision
    if (bird.y + bird.radius >= H - GROUND_H || bird.y - bird.radius <= 0) {
        endGame();
        return;
    }

    // Pipes
    spawnTimer += dt;
    if (spawnTimer >= SPAWN_INTERVAL) {
        spawnTimer -= SPAWN_INTERVAL;
        spawnPipe();
    }

    const collisionRadius = bird.radius - 3; // slightly forgiving bound

    for (let i = pipes.length - 1; i >= 0; i--) {
        let p = pipes[i];
        p.x -= PIPE_SPEED * dt;

        // Collision logic
        const topH = p.gapCenterY - GAP_SIZE / 2;
        const botY = p.gapCenterY + GAP_SIZE / 2;
        
        // Rect collisions approximations
        const hitTop = (bird.x + collisionRadius > p.x && bird.x - collisionRadius < p.x + PIPE_WIDTH) && (bird.y - collisionRadius < topH);
        const hitBot = (bird.x + collisionRadius > p.x && bird.x - collisionRadius < p.x + PIPE_WIDTH) && (bird.y + collisionRadius > botY);

        if (hitTop || hitBot) {
            endGame();
            return;
        }

        // Scoring
        if (!p.scored && p.x + PIPE_WIDTH < bird.x) {
            p.scored = true;
            score++;
            updateScoreUI();
        }

        // Remove offscreen
        if (p.x < -PIPE_WIDTH - 20) {
            pipes.splice(i, 1);
        }
    }
}

function drawBackground() {
    // Sky gradient
    const grad = ctx.createLinearGradient(0, 0, 0, H);
    grad.addColorStop(0, '#87CEEB');
    grad.addColorStop(1, '#4682B4');
    ctx.fillStyle = grad;
    ctx.fillRect(0, 0, W, H);

    // Clouds
    ctx.fillStyle = 'rgba(255, 255, 255, 0.7)';
    clouds.forEach(c => {
        ctx.beginPath(); ctx.arc(c.x, c.y, c.r, 0, Math.PI*2); ctx.fill();
        ctx.beginPath(); ctx.arc(c.x + 20, c.y - 5, c.r - 3, 0, Math.PI*2); ctx.fill();
        ctx.beginPath(); ctx.arc(c.x + 38, c.y + 2, c.r - 6, 0, Math.PI*2); ctx.fill();
    });
}

function drawGround() {
    // Soil
    ctx.fillStyle = '#C2A36B';
    ctx.fillRect(0, H - GROUND_H, W, GROUND_H);
    // Grass
    ctx.fillStyle = '#4CAF50';
    ctx.fillRect(0, H - GROUND_H, W, 10);
}

function drawPipes() {
    pipes.forEach(p => {
        const topH = p.gapCenterY - GAP_SIZE / 2;
        const botY = p.gapCenterY + GAP_SIZE / 2;
        const botH = H - GROUND_H - botY;
        const capH = 20;
        const capX = -6;
        
        ctx.fillStyle = '#4CAF50';
        ctx.strokeStyle = '#388E3C';
        ctx.lineWidth = 2;

        // Top Pipe
        ctx.fillRect(p.x, 0, PIPE_WIDTH, topH);
        ctx.strokeRect(p.x, 0, PIPE_WIDTH, topH);
        ctx.fillRect(p.x + capX, topH - capH, PIPE_WIDTH - capX*2, capH);
        ctx.strokeRect(p.x + capX, topH - capH, PIPE_WIDTH - capX*2, capH);

        // Bottom Pipe
        ctx.fillRect(p.x, botY, PIPE_WIDTH, botH);
        ctx.strokeRect(p.x, botY, PIPE_WIDTH, botH);
        ctx.fillRect(p.x + capX, botY, PIPE_WIDTH - capX*2, capH);
        ctx.strokeRect(p.x + capX, botY, PIPE_WIDTH - capX*2, capH);
    });
}

function drawBird() {
    ctx.save();
    ctx.translate(bird.x, bird.y);
    ctx.rotate(bird.rotation * Math.PI / 180);

    // Body
    ctx.fillStyle = '#FFCC00';
    ctx.strokeStyle = '#CC9900';
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.arc(0, 0, bird.radius, 0, Math.PI * 2);
    ctx.fill();
    ctx.stroke();

    // Wing
    ctx.fillStyle = '#FFAA00';
    ctx.beginPath();
    ctx.arc(-6, 2, 8, 0, Math.PI * 2);
    ctx.fill();

    // Eye
    ctx.fillStyle = 'white';
    ctx.beginPath();
    ctx.arc(6, -5, 4.5, 0, Math.PI * 2);
    ctx.fill();
    ctx.fillStyle = 'black';
    ctx.beginPath();
    ctx.arc(7.5, -5, 2, 0, Math.PI * 2);
    ctx.fill();

    // Beak
    ctx.fillStyle = '#FF6432';
    ctx.beginPath();
    ctx.moveTo(bird.radius - 2, -2);
    ctx.lineTo(bird.radius + 9, 3);
    ctx.lineTo(bird.radius - 2, 6);
    ctx.closePath();
    ctx.fill();

    ctx.restore();
}

function render() {
    ctx.clearRect(0, 0, W, H);
    drawBackground();
    
    if (state === 'PLAYING' || state === 'GAMEOVER') {
        drawPipes();
        drawBird();
    }
    
    drawGround();
}

function gameLoop(timestamp) {
    if (!lastTime) lastTime = timestamp;
    let dt = (timestamp - lastTime) / 1000;
    lastTime = timestamp;
    if (dt > 0.05) dt = 0.05; // cap delta time

    update(dt);
    render();

    animationFrameId = requestAnimationFrame(gameLoop);
}

// Interactions & States
function setUIState(newState) {
    state = newState;
    uiMainMenu.classList.add('hidden');
    uiGameOver.classList.add('hidden');
    uiLeaderboard.classList.add('hidden');
    uiInGame.classList.add('hidden');

    if (state === 'MENU') uiMainMenu.classList.remove('hidden');
    if (state === 'PLAYING') uiInGame.classList.remove('hidden');
    if (state === 'GAMEOVER') uiGameOver.classList.remove('hidden');
    if (state === 'LEADERBOARD') {
        updateLeaderboardUI();
        uiLeaderboard.classList.remove('hidden');
    }
}

function updateScoreUI() {
    elCurrentScore.innerText = score;
}

function startGame() {
    resetGame();
    setUIState('PLAYING');
    jump();
}

function endGame() {
    setUIState('GAMEOVER');
    elFinalScore.innerText = score;
    
    // Process High Score
    highScores.push(score);
    highScores.sort((a,b) => b - a);
    highScores = highScores.slice(0, 10);
    localStorage.setItem('flappyScores', JSON.stringify(highScores));
    
    elBestScore.innerText = highScores[0] || 0;
}

function updateLeaderboardUI() {
    elLeaderboardList.innerHTML = '';
    if (highScores.length === 0) {
        elLeaderboardList.innerHTML = '<li><span>-</span><span>No scores yet!</span><span>-</span></li>';
        return;
    }
    highScores.forEach((s, i) => {
        const li = document.createElement('li');
        li.innerHTML = `<span>#${i+1}</span><span>${s}</span>`;
        elLeaderboardList.appendChild(li);
    });
}

function jump() {
    if (state === 'PLAYING') {
        bird.vy = JUMP_VELOCITY;
    } else if (state === 'MENU') {
        startGame();
    }
}

// Event Listeners
window.addEventListener('keydown', (e) => {
    if (e.code === 'Space') {
        e.preventDefault();
        jump();
        if(state === 'GAMEOVER') startGame();
    }
});

canvas.parentElement.addEventListener('mousedown', (e) => {
    if(e.target.tagName !== 'BUTTON') { // Prevent click jump if clicking a button overlay
        jump();
    }
});

btnLeadMenu.addEventListener('click', () => setUIState('LEADERBOARD'));
btnLeadGO.addEventListener('click', () => setUIState('LEADERBOARD'));
btnBack.addEventListener('click', () => setUIState('MENU'));
btnRetry.addEventListener('click', startGame);

// Init
initClouds();
setUIState('MENU');
requestAnimationFrame(gameLoop);
