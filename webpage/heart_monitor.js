// Heart Monitor Audio
var audio = new Audio('http://www.dallasjclark.com/wordpress/wp-content/uploads/2016/05/Heart-Rate-Monitor.mp3');
var audioPlayed = false;

// Full Height and Width
var canvasHeight = $(document).height();
var canvasWidth = $(document).width();

// Create Canvas Element
var canvas = document.createElement('canvas');
canvas.width = canvasWidth;
canvas.height = canvasHeight;
document.body.appendChild(canvas);

// Context
var context = canvas.getContext('2d');
context.save();

// Stroke Positions
strokePositionX = 0;
strokePositionY = canvasHeight / 2;

/**
 * Render the Background Screen
 */
function renderBackgroundScreen (alpha) {
    context.beginPath();
    context.fillStyle = 'rgba(20, 20, 20, ' + alpha + ')';
    context.fillRect(0, 0, canvasWidth, canvasHeight);
    context.closePath();

    context.beginPath();

    for (var j = 20; j < canvasHeight; j = j + 20) {
        context.moveTo(0, j);
        context.lineTo(canvasWidth, j);
    }
    
    for (var i = 20; i < canvasWidth; i = i + 20) {
        context.moveTo(i, 0);
        context.lineTo(i, canvasHeight);
    }
    
    context.lineWidth = 1;
    context.strokeStyle = 'rgba(20, 50, 20, ' + alpha + ')';
    context.stroke();
    context.closePath();
}

/**
 * Draws the Heart Monitor Stroke
 */
function drawHeartMonitorStroke () {
    context.restore();
    
    renderBackgroundScreen(0.02);

    context.beginPath();
    context.moveTo(strokePositionX, strokePositionY);
    strokePositionX = strokePositionX + 1;

    if ((strokePositionX >= canvasWidth * 20 / 100 && strokePositionX < canvasWidth * 24 / 100)
    || (strokePositionX >= canvasWidth * 70 / 100 && strokePositionX < canvasWidth * 74 / 100)) {
        // Test audio has not been played
        if (!audioPlayed) {
            audio.play();
            audioPlayed = true;
        }
        strokePositionY = strokePositionY - canvasHeight / 100;
    }

    else if ((strokePositionX >= canvasWidth * 24 / 100 && strokePositionX < canvasWidth * 32 / 100)
    || (strokePositionX >= canvasWidth * 74 / 100 && strokePositionX < canvasWidth * 82 / 100)) {
        strokePositionY = strokePositionY + canvasHeight / 100;

        // Reset Audio Played flag
        audioPlayed = false;
    }

    else if ((strokePositionX >= canvasWidth * 32 / 100 && strokePositionX < canvasWidth * 36 / 100)
    || (strokePositionX >= canvasWidth * 82 / 100 && strokePositionX < canvasWidth * 86 / 100)) {
        strokePositionY = strokePositionY - canvasHeight / 100;
    }

    else if ((strokePositionX >= canvasWidth * 36 / 100 && strokePositionX < canvasWidth * 60 / 100)
    || (strokePositionX >= canvasWidth * 86 / 100 && strokePositionX <= canvasWidth)) {
        strokePositionY = canvasHeight / 2;
    }

    else if (strokePositionX > canvasWidth) {
        strokePositionX = 0;
        context.moveTo(strokePositionX, strokePositionY);
    }

    context.lineTo(strokePositionX, strokePositionY);
    context.lineWidth = 2;
    context.strokeStyle = '#47FF47';
    context.stroke();
    context.closePath();
}

renderBackgroundScreen(1);
setInterval(drawHeartMonitorStroke, 1);