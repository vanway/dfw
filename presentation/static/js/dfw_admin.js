//获取用户列表
function getUserList() {
    $.ajax({
        url: "getUserList",
        type: 'GET', 
        data: {},
        dataType: 'html', 
        timeout: 10000,
        error: function(){alert('Error loading document');},
        success: function(result){ 
            var data = eval("("+result+")");
            $('#user_data').empty();
            for(var i in data) {
                var tr = $('<tr>');
                tr.append($('<td>' + data[i][0] + '</td>')); //用户id
                tr.append($('<td>' + data[i][1] + '</td>')); //用户姓名
                tr.append($('<td>' + data[i][2].toFixed(2) + '</td>')); //累计充值
                tr.append($('<td>' + data[i][3].toFixed(2) + '</td>')); //当前余额
                tr.append($('<td>' + data[i][4].toFixed(2) + '</td>')); //总盈亏
                tr.append($('<td>' + data[i][5] + '</td>')); //电话号码
                if (data[i][6] == '1') {//是否在线
                    tr.append($('<td>是</td>'));
                } else {
                    tr.append($('<td>否</td>'));
                }
                $('#user_data').append(tr);
            }
        }
    });
}

//交易流水tab
$('#userStreamTab').click(function(){
        $('.nav li').removeClass('active');
        $('#userStreamTab').addClass('active');
        $('#div_trade_payback').attr('hidden',true);
        $('#userStreamTab').removeAttr('hidden');
        });
$('#tradeTab').click(function(){
        $('.nav li').removeClass('active');
        $('#tradeTab').addClass('active');
        $('#div_user_steam').attr('hidden', true);
        $('#tradeTab').removeAttr('hidden');
        });


//获取交易流水数据
function getTradeData(user_stream,trade_payback) {
    $('#user_stream_data').empty();
    $('#trade_data').empty();

    for(var i in user_stream) {
        var tr = ('<tr>');
        tr.append($('<td>' + user_stream[i][0] + '</td>')); //时间
        tr.append($('<td>' + user_stream[i][1] + '</td>')); //用户名
        tr.append($('<td>' + user_stream[i][2] + '</td>')); //操作
        tr.append($('<td>' + user_stream[i][3] + '</td>')); //定位
        $('#user_stream_data').append(tr);
    }

    for(var i in trade_payback) {
        var tr = ('<tr>');
        tr.append($('<td>' + user_stream[i][0] + '</td>')); //时间
        tr.append($('<td>' + user_stream[i][1] + '</td>')); //用户名
        tr.append($('<td>' + user_stream[i][2] + '</td>')); //操作
        tr.append($('<td>' + user_stream[i][3] + '</td>')); //定位
        tr.append($('<td>' + user_stream[i][4] + '</td>')); //状态
        $('#trade_data').append(tr);
    }
}
