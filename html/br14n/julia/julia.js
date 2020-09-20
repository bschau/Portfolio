const width = 500;
const height = 500;
const maxIterations = 128;
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext( "2d" );
var running = false;

function redFilter(i) {
    var red = '' + (((i * 248) / maxIterations) | 0);
    var other = '' + (((i * 128) / maxIterations) | 0);
    return 'rgb(' + red + ',' + other + ',' + other + ')';
}

function greenFilter(i) {
    var green = '' + (((i * 248) / maxIterations) | 0);
    var other = '' + (((i * 128) / maxIterations) | 0);
    return 'rgb(' + other + ',' + green + ',' + other + ')';
}

function blueFilter(i) {
    var blue = '' + (((i * 248) / maxIterations) | 0);
    var other = '' + (((i * 128) / maxIterations) | 0);
    return 'rgb(' + other + ',' + other + ',' + blue + ')';
}

function greyFilter(i) {
    var col = '' + (((i * 128) / maxIterations) | 0);
    return 'rgb(' + col + ',' + col + ',' + col + ')';
}

function julia(cRe, cIm, colorFilter) {
    var i;
    for (var y = 0; y < height; y++) {
        for (var x = 0; x < width; x++) {
            var re = (x - width / 2) / (0.5 * width);
            var im = (y - height / 2) / (0.5 * height);
            for (i = 0; i < maxIterations; i++) {
                var oldRe = re;
                var oldIm = im;
                re = oldRe * oldRe - oldIm * oldIm + cRe;
                im = 2 * oldRe * oldIm + cIm;

                if ((re * re + im * im) > 4) {
                    break;
                }
            }

            var distance = (32 * i) / maxIterations;
            ctx.fillStyle = colorFilter(i);
            ctx.fillRect(x, y, 1, -distance);
            if (i < maxIterations) {
                continue;
            }

            ctx.fillStyle = '#fff';
            ctx.fillRect(x, y - distance, 1, 1);
        }
    }
}

function select(cRe, cIm, filter) {
    if (running) {
        return;
    }
    running = true;
    document.getElementById('realPart').value = cRe;
    document.getElementById('imaginaryPart').value = cIm;

    if (filter === 'red') {
        filter = redFilter;
    } else if (filter === 'green') {
        filter = greenFilter;
    } else if (filter === 'blue') {
        filter = blueFilter;
    } else {
        filter = greyFilter;
    }

    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, width, height);
    julia(cRe, cIm, filter);

    running = false;
}

function run(filter) {
    var cRe = parseFloat(document.getElementById('realPart').value);
    if (cRe === NaN) {
        return;
    }

    var cIm = parseFloat(document.getElementById('imaginaryPart').value);
    if (cIm === NaN) {
        return;
    }

    select(cRe, cIm, filter);
}

select(-0.7, 0.27015, "red");
