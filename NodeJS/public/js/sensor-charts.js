(function() {
	//グラフの要素数
	var data = [ ['x'], ['AccX01'],['AccY01'],['AccZ01'],['AccX02'],['AccY02'],['AccZ02']];
	var set = 100;
	//var data2 = [ ['x'],['AccY']];

	$(function() {
		// サーバーに接続
		var socket = io.connect(location.origin);
		var accValueX = new Array();
		var accValueY = new Array();
		var accValueZ = new Array();

		// サーバーからセンサーの値を受信
		socket.on('sensor', function(dataFromServer) {

			accValueX[0] = dataFromServer.accValueX1;
			accValueY[0] = dataFromServer.accValueY1;
			accValueZ[0] = dataFromServer.accValueZ1;
			accValueX[1] = dataFromServer.accValueX2;
			accValueY[1] = dataFromServer.accValueY2;
			accValueZ[1] = dataFromServer.accValueZ2;

			/*
			for(var i = 0;i <= dataFromServer.sensorNum;i++){
				if(i != dataFromServer.num){
					accValueX[i] = accValueX[i];
					accValueY[i] = accValueX[i];
					accValueZ[i] = accValueX[i];
				}
			}
			accValueX[dataFromServer.num] = dataFromServer.accValueX;
			accValueY[dataFromServer.num] = dataFromServer.accValueY;
			accValueZ[dataFromServer.num] = dataFromServer.accValueZ;
			*/
		});

		// 一定間隔でサーバーのセンサーデータを確認してchartを変更

		setInterval(function() {
			data[0].push( new XDate().toString('yyyy-MM-dd hh:mm:ss:fff') );
			data[1].push( accValueX[0] );
			data[2].push( accValueY[0] );
			data[3].push( accValueZ[0] );
			data[4].push( accValueX[1] );
			data[5].push( accValueY[1] );
			data[6].push( accValueZ[1] );
			draw();
		}, 1000);
	});

	// http://c3js.org/samples/timeseries.html
	function draw() {
		var chart1 = c3.generate({
		    data: {
		        x: 'x',
				xFormat: '%Y-%m-%d %H:%M:%S:%f',
		        columns: data
		    },
		    axis: {
		        x: {
		            type: 'timeseries',
		            tick: {
		                format: '%Y-%m-%d %H:%M:%S:%f'
		            }
		        }
		    }
		});
		/*
		var chart2 = c3.generate({
		    data: {
		        x: 'x',
				xFormat: '%Y-%m-%d %H:%M:%S',
		        columns: data1
		    },
		    axis: {
		        x: {
		            type: 'timeseries',
		            tick: {
		                format: '%Y-%m-%d %H:%M:%S'
		            }
		        }
		    }
		});
		*/
	}
})();
