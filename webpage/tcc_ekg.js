<button id="stop">Stop</button>
<br>
<canvas id="canvas" width=300 height=300></canvas>

<link rel="stylesheet" href="tcc_ekg.css">

<script>
var canvas = document.getElementById("canvas");
var ctx = canvas.getContext("2d");
//ctx.fillStyle = "#dbbd7a";
ctx.fill();

// capture incoming socket data in an array
var data = new Array(500);

// TESTING: fill data with some test values
for (var i = 0; i < 500; i++) {
    //data.push(Math.sin(i / 10) * 70 + 100);
    data[i] = (Math.sin(i / 10) * 70 + 100);
}


var x = 0;
var panAtX = 250;
var continueAnimation = true;
animate();


function animate() {

    //if (x > data.length - 1) {
        //return;
    //}
    if (x >= data.length*1) {
	x = 0;
    }

    if (continueAnimation) {
        requestAnimationFrame(animate);
    }
    ctx.lineWidth = "2";
    ctx.strokeStyle = 'red';

    if (x++ < panAtX) {

        ctx.fillRect(x, data[x], 1, 1);

    } else {

        ctx.clearRect(0, 0, canvas.width, canvas.height);

        // plot data[] from x-PanAtX to x 

        for (var xx = 0; xx < panAtX; xx++) {
            var y = data[x - panAtX + xx];
            ctx.fillRect(xx, y, 1, 1)
        }
    }
}


$("#stop").click(function () {
    continueAnimation = false;
});
</script>