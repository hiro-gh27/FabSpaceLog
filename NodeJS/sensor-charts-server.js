//http://localhost:3000/sensor-charts.html アクセス先

var server = require('./server.js');
var settings = require('./settings.js')
server.init();

console.log(settings);
/*
    Socket.IO
*/
var socket;
// サーバーへのアクセスを監視。アクセスがあったらコールバックが実行
server.io.sockets.on('connection', function (s) {
    socket = s;
});

//setting UDP from ESP 8266
var dgram = require('dgram');
var sender = dgram.createSocket('udp4');
var message = new Buffer('test message');
var str = new String("");
var flag = 1;
var sensorNum = 0; //センサのID管理のためのカウント変数
var set = 100; //setの個数分だけセンサを識別できる
var sensor = new String("");

/*
ESP8266からのセンサの値の受信処理
*/
var NODE_HOST = '172.20.10.15';
var NODE_PORT = 8889
var sensorAddr = new Array();
for(var i = 0;i < set;i++){
  sensorAddr[i] = new Array();
  for(var j = 0;j < 6;j++){
    sensorAddr[i][j] = "dammy";
  }
}

var receiver = dgram.createSocket('udp4');
receiver.on('message', function (message, remote) {
  str = message.toString('utf-8', 0, message.length);
  var flag = 0;

  //センサの紐付け及び出力の処理
  for(var i = 0;i < sensorNum + 1;i++){
    if(str.indexOf(sensorAddr[i][0]) != -1){
      sensorAddr[i] = str.split(",");
      //str = str.replace(sensorAddr[i][0],"No" + i);
      flag = 1;
      if(socket) {
        socket.emit('sensor', { accValueX1: sensorAddr[0][1],accValueY1: sensorAddr[0][2],accValueZ1: sensorAddr[0][3],accValueX2: sensorAddr[1][1],accValueY2: sensorAddr[1][2],accValueZ2: sensorAddr[1][3]});
        /*
        socket.emit('sensor', { accValueX: sensorAddr[i][1],accValueY: sensorAddr[i][2],accValueZ: sensorAddr[i][3],num:i,set:sensorNum});
        */
      }
      break;
    }
  }
  if(flag == 0){
    sensorAddr[sensorNum] = str.split(",");
    console.log(sensorNum);
    sensorNum++;
  }
});
receiver.bind(NODE_PORT, NODE_HOST);
