
var canvas = document.getElementById('Stick');
var ctx = canvas.getContext("2d");
var mX=canvas.width/2;
var mY=canvas.height/2;
var STX=0;
var STY=0;

function draw()
{
    const sr=20;
    const br=80;
    var rect = canvas.getBoundingClientRect();
    document.body.addEventListener("mousemove", function(e)
    {
        //座標を取得する
        mX = e.pageX-rect.left;  //X座標
        mY = e.pageY-rect.top;  //Y座標
    });
    if(mX>canvas.width||mY>canvas.height||mX<0||mY<0)
    {
        mX=canvas.width/2;
        mY=canvas.height/2;       
    }
    var xw=mX-(canvas.width/2);
    var yh=mY-(canvas.height/2);
    var D=Math.sqrt(xw*xw + yh*yh);
    var angle=Math.atan2(yh, xw);
    //console.log(D);
    //console.log(angle*(180/Math.PI));
    var sX=Math.cos(angle);
    var sY=Math.sin(angle);
    if(D>br)
    {
        mX=(canvas.width/2)+br*sX;
        mY=(canvas.height/2)+br*sY;
    }
    else 
    {
        var data = new Int8Array(2);
        data[0] = 2;
        data[1] = STX;
        console.log(data);
        connection.send(data);
        data[0] = 3;
        data[1] = STY;
        console.log(data);
        connection.send(data);
    }
    STX=Math.floor((D/br)*sX*128);
    STY=Math.floor((D/br)*sY*128)*-1;
    document.getElementById("txtX").value = STX;
    document.getElementById("txtY").value = STY;
    /*
    else if(mX>canvas.width)
        mX=canvas.width;
    else if(mY>canvas.height)
        mY=canvas.height;*/
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.beginPath();
    ctx.strokeStyle = '#000000';
    ctx.lineWidth = 1;
    ctx.arc(canvas.width/2,canvas.height/2,br,0,Math.PI*2,false);
    ctx.stroke();
    ctx.beginPath();
    ctx.strokeStyle = '#CCCCCC';
    ctx.lineWidth = 10;
    ctx.moveTo(canvas.width/2,canvas.height/2);
    ctx.lineTo(mX,mY);
    ctx.stroke();
    ctx.beginPath();
    ctx.arc(mX,mY, sr, 0, Math.PI*2);
    ctx.fillStyle = "#0095DD";
    ctx.fill();
    ctx.closePath();
}
setInterval(draw, 20);