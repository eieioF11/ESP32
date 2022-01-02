function sendCheckbox(e)
{
    var data = new Int8Array(2);
    data[0] = parseInt(e.target.id);
    data[1] = e.target.checked;
    console.log(data);
    connection.send(data);
}

var selbutton = 0;
function sendselbutton(e)
{
    var data = new Int8Array(2);
    data[0] = parseInt(e.target.id);
    if(selbutton==1)
        selbutton=0;
    else
        selbutton=1;
    data[1] = selbutton;
    console.log(data);
    connection.send(data);
}

var embutton = 0;
function sendembutton(e)
{
    var data = new Int8Array(2);
    data[0] = parseInt(e.target.id);
    if(embutton==1)
        embutton=0;
    else
        embutton=1;
    data[1] = embutton;
    console.log(data);
    connection.send(data);
}