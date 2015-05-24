//global websocket object

var g_ws;
var g_username;
var g_furture_fig_echart;

function getcookie(objname){
    var arrstr = document.cookie.split("; ");
    for(var i = 0;i < arrstr.length;i ++){
        var temp = arrstr[i].split("=");
        if(temp[0] == objname) return unescape(temp[1]);
    }
}


$(document).ready(function() {
    init_furture_fig();

    function showMessage(message) {
        $('#messages').append('<li>' + message)
    }

    if (!window.WebSocket) {
        if (window.MozWebSocket) {
            window.WebSocket = window.MozWebSocket;
        } else {
            alert("Your browser doesn't support WebSockets")
        }
    }
    
    g_username = getcookie('username');
    g_ws = new WebSocket('ws://127.0.0.1:5000/websocket');
    g_ws.onopen = function(evt) {
        // 发送信息让后端能关联 ws.id和username
        var type = "1";
        var obj = "user";
        g_ws.send(type+','+obj+','+g_username);
    }

    g_ws.onmessage = function(evt) {
        evt_dict = eval("("+evt.data+")");
        if (typeof(evt_dict.cur_price) != "undefined") { 
            update_furture_fig(evt_dict.cur_price);
        }
        if (typeof(evt_dict.trade_info) != "undefined") { 
            getTradeData(evt_dict.cur_price);
        }
        if (typeof(evt_dict.single_trade) != "undefined") { 
            getSingleData(evt_dict.single_trade);
        }
        if (typeof(evt_dict.total_trade) != "undefined") { 
            getTotalData(evt_dict.total_trade)
        }
    }

    $('#send-message').submit(function() {
        var message = $('#name').val() + ": " + $('#message').val();
        showMessage(message)
    return false;
    });
});

function click_buy(){
    g_ws.send("2,,");
    document.getElementById('buy-btn').setAttribute("disabled", "disabled");
    document.getElementById('sell-btn').setAttribute("disabled", "disabled");
}

function click_sell(){
    g_ws.send("3,,");
    document.getElementById('buy-btn').setAttribute("disabled", "disabled");
    document.getElementById('sell-btn').setAttribute("disabled", "disabled");
}
function click_pingcan(){
    g_ws.send("4,,");
    document.getElementById('buy-btn').removeAttribute("disabled");
    document.getElementById('sell-btn').removeAttribute("disabled");
    $('#singe-trade').empty();   
}

function update_furture_fig(date_str){
    axisData = (new Date()).toLocaleTimeString().replace(/^\D*/,'');
    g_furture_fig_echart.addData([[0, parseFloat(date_str), false, false, axisData]]);
    //g_furture_fig_echart.setOption({});
}

function init_furture_fig(){
    $.ajax({
        url: "getInitInfo",
        type: 'GET', 
        data: {},
        dataType: 'html', 
        timeout: 10000,
        error: function(){alert('Error loading document');},
        success: function(result){ 
            var data = eval("(" + result + ")");
            init_furture_line(data);
        }
    });
}

function init_furture_line(data){
    var len_data = data.length;
    elementDL = document.getElementById('furture_fig');
    g_furture_fig_echart = echarts.init(document.getElementById('furture_fig'));
    g_furture_fig_echart.setTheme("macarons");
    option = {
        title : {
                    text: '指数走势面盘',
                    subtext: '纯属虚构'
                },
        tooltip : {
                      trigger: 'axis'
                  },
        legend: {
                    data:['IF505']
                },
        toolbox: {
                     show : false,
                     feature : {
                         mark : {show: true},
                         dataView : {show: true, readOnly: false},
                         magicType : {show: true, type: ['line', 'bar']},
                         restore : {show: true},
                         saveAsImage : {show: true}
                     }
                 },
        dataZoom : {
                       show : false,
                       start : 0,
                       end : 100
                   },
        xAxis : [
        {
            type : 'category',
            boundaryGap : true,
            data : (function (){
                var now = new Date();
                var res = [];
                var len = len_data;
                while (len--) {
                    res.unshift(now.toLocaleTimeString().replace(/^\D*/,''));
                    now = new Date(now - 500);
                }
                return res;
            })()
        }
        ],
            yAxis : [
            {
                type : 'value',
                scale: true,
                name : '指数',
                boundaryGap: [0.002, 0.002]
            }
        ],
            series : [
            {
                name:'IF505',
                type:'line',
                data:(function (){
                    return data;
                })(),
                markPoint : {
                                data : [
                                {type : 'max', name: '最大值'},
                                {type : 'min', name: '最小值'}
                                ]
                            },
                markLine : {
                               data : [
                               {type : 'average', name: '平均值'}
                               ]
                           }
            }
        ]
    };
    g_furture_fig_echart.setOption(option);
    elementDL.style.backgroundColor = '#ffffff';
}

function getTradeData(cur_price){
    //var data = [['卖5',51.88,78],['卖4',51.85,40],['卖3',51.84,8],['卖2',51.80,7190],['卖1',51.79, 100],['买1',51.71,5],['买2',51.60,4],['买3',51.50,101],['买4',51.45,59],['买5',51.43,4]];
    var table = $('<table class="table">');

    var thead = $('<thead><tr><th>五挡</th><th>价格</th><th>成交量</th></tr></thead>');
    table.append(thead);

    var tbody = $('<tbody>');
        var tr = $('<tr>');
        var wudang = $('<td>' + "当前指数" + '</td>');
        var price = $('<td><font color="red">' + cur_price.toFixed(2) + '</font></td>');
        var volumn = $('<td>' + "" + '</td>');
        tr.append(wudang);
        tr.append(price);
        tr.append(volumn);
        tbody.append(tr);
    table.append(tbody);
    $('#trader_fig').empty();
    $('#trader_fig').append(table);
}

function getSingleData(data){
    //[买入时间],买入指数，当前指数，账户资产,[盈亏，盈亏比例]
    //var data = [3400,2345,100000];

    var table = $('<table class="table">');
    var thead = $('<thead><tr><th>买入时间</th><th>账户资产</th><th>买入指数</th><th>当前指数</th><th>盈亏</th><th>盈亏比例</th></tr></thead>');
    table.append(thead);
    var tbody = $('<tbody>');

    var tr = $('<tr>');
    tr.append($('<td>' + data[0] + '</td>'));
    var totalAssets = $('<td>' + data[1].toFixed(2) + '</td>');
    var buyPoint = $('<td>' + data[2].toFixed(2) + '</td>');
    var currentPoint = $('<td>' + data[3].toFixed(2) + '</td>');
    tr.append(totalAssets);
    tr.append(buyPoint);
    tr.append(currentPoint);
    var benefit = data[4].toFixed(2);
    if(benefit >= 0) {
        tr.append($('<td><font color="red">' + benefit + '</font></td>'));
    } else {
        tr.append($('<td><font color="green">' + benefit + '</font></td>'));
    }

    var re = /([0-9]+\.[0-9]{2})[0-9]*/;
    tr.append("<td>" + (benefit*100/data[1]).toString().replace(re,"$1") + '%' + "</td>");
    tbody.append(tr);
    table.append(tbody);
    $('#singe-trade').empty();
    $('#singe-trade').append(table);
}

function getTotalData(data){
    //累计充值，当前资产，[盈亏，盈亏比例]
    var table = $('<table class="table">');
    var thead = $('<thead><tr><th>用户名称</th><th>累计充值</th><th>当前资产</th><th>累计盈亏</th><th>盈亏比例</th></tr></thead>');
    table.append(thead);
    var tbody = $('<tbody>');

    var tr = $('<tr>');
    tr.append($('<td>' + "hi, " + data[0]  + '</td>'));
    var recharge = $('<td>' + data[1] + '</td>');
    var currentAssets = $('<td>' + data[2] + '</td>');
    tr.append(recharge);
    tr.append(currentAssets);

    var benefit = data[3].toFixed(2);
    if(benefit >= 0) {
        tr.append($('<td><font color="red">' + benefit + '</font></td>'));
    } else {
        tr.append($('<td><font color="green">' + benefit + '</font></td>'));
    }

    var re = /([0-9]+\.[0-9]{2})[0-9]*/;
    tr.append("<td>" + (benefit*100/data[2]).toString().replace(re,"$1") + '%' + "</td>");
    tbody.append(tr);

    table.append(tbody);
    $('#total-trade').empty();
    $('#total-trade').append(table);
}

