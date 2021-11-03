var ssid_get;
var time_set = 0;
var clock_switch = 0;

function check_wifi() {
    var input_wifi_check = document.getElementsByName("wifi_add");
    var input_wifi_num;
    var wifi_num = input_wifi_check.length;
    for (var i = 0; i < wifi_num; i++) {
        if (input_wifi_check[i].checked)
            input_wifi_num = i;
    }
    document.getElementById("wifi_ssid").value = ssid_get.info_ssid[i];
}

function set_time(get_switch) {
    if (get_switch == 1)
        clock_switch = 1;
    else if (get_switch == 0)
        clock_switch = 0;
    if (time_set == 0) {
        var tb = document.getElementById("clock_table");
        var newTr = tb.insertRow(2);//表示在第二行后添加一行
        var newTd = newTr.insertCell();//表示在添加的行上添加第一格
        newTd.setAttribute("colspan", "2");
        newTd.innerHTML = "<input type='text' name='shi' id='shi' style='width: 40px;font-size: 25px;font-family: 华文楷体'>" + "时" + "<input type='text' name='fen' id='fen' style='width: 40px;font-size: 25px;font-family: 华文楷体'>" + "分";
        var newTr2 = tb.insertRow(3);//表示在第二行后添加一行
        var newTd2 = newTr2.insertCell();//表示在添加的行上添加第一格
        newTd2.setAttribute("colspan", "2");
        newTd2.innerHTML = "        <input type=\"checkbox\" name=\"week\" value=\"星期一\">星期一\n" +
            "        <input type=\"checkbox\" name=\"week\" value=\"星期二\">星期二\n" +
            "        <input type=\"checkbox\" name=\"week\" value=\"星期三\">星期三\n" +
            "        <input type=\"checkbox\" name=\"week\" value=\"星期四\">星期四\n" + "<br>" +
            "        <input type=\"checkbox\" name=\"week\" value=\"星期五\">星期五\n" +
            "        <input type=\"checkbox\" name=\"week\" value=\"星期六\">星期六\n" +
            "        <input type=\"checkbox\" name=\"week\" value=\"星期日\">星期日\n";

    }
    time_set = 1;
}

function Send_this(button_add) {
    if (button_add == 1) {
        var input_name = document.getElementById("wifi_ssid").value;
        var input_code = document.getElementById("wifi_password").value;
        var xhttp = new XMLHttpRequest();
        xhttp.open("POST", "/wifi_data/sent", true);
        xhttp.onreadystatechange = function () {
            if (xhttp.readyState == 4) {
                if (xhttp.status == 200) {
                    console.log(xhttp.responseText);
                } else if (xhttp.status == 0) {
                    alert("链接断开")
                    location.reload();
                } else {
                    alert(xhttp.status + "Error!\n" + xhttp.responseText);
                    location.reload();
                }
            }
        }
        var data = {
            "wifi_num": input_name,
            "wifi_code": input_code
        }
        xhttp.send(JSON.stringify(data));
    } else if (button_add == 2) {
        var xhttp = new XMLHttpRequest();
        xhttp.open("POST", "/wifi_data/get_ssid", true);
        xhttp.onreadystatechange = function () {
            if (xhttp.readyState == 4) {
                if (xhttp.status == 200) {
                    ssid_get = JSON.parse(xhttp.responseText);
                    var i, new_wifi_num = ssid_get.num;
                    var rssi;
                    for (i = 0; i < new_wifi_num; i++) {
                        var tb = document.getElementById("wifi_table");
                        var newTr = tb.insertRow(i + 1);//表示在第二行后添加一行
                        var newTd = newTr.insertCell();//表示在添加的行上添加第一格
                        newTd.innerHTML = "<input type='radio' name='wifi_add' onclick='check_wifi()' value=i>" + ssid_get.info_ssid[i];
                        var newTd2 = newTr.insertCell();//表示在添加的行上添加第二格
                        rssi = ssid_get.info_rssi[i];
                        newTd2.innerHTML = rssi + 'dBm ';
                    }
                } else if (xhttp.status == 0) {
                    alert("链接断开")
                    location.reload();
                } else {
                    alert(xhttp.status + "Error!\n" + xhttp.responseText);
                    location.reload();
                }
            }
        }
        var data = {
            "get_ssid": "get",
        }
        xhttp.send(JSON.stringify(data));

    } else if (button_add == 3) {
        var xhttp = new XMLHttpRequest();
        xhttp.open("POST", "/clock_data", true);
        xhttp.onreadystatechange = function () {
            if (xhttp.readyState == 4) {
                if (xhttp.status == 200) {
                    console.log(xhttp.responseText);
                } else if (xhttp.status == 0) {
                    alert("链接断开")
                    location.reload();
                } else {
                    alert(xhttp.status + "Error!\n" + xhttp.responseText);
                    location.reload();
                }
            }
        }
        if (clock_switch) {
            var input_alarm_shi = document.getElementById("shi").value;
            var input_alarm_fen = document.getElementById("fen").value;
            var input_alarm_week_group = document.getElementsByName("week");
            var repetition_day = 0;
            for (i = 0; i < 7; i++) {
                if (input_alarm_week_group[i].checked) {
                    repetition_day++;
                }
            }
            if (repetition_day != 0) {
                var data = {
                    "switch": "open",
                    "repetition": "yes",
                    "shi": input_alarm_shi,
                    "fen": input_alarm_fen,
                    "Mon": input_alarm_week_group[0].checked,
                    "Tue": input_alarm_week_group[1].checked,
                    "Wed": input_alarm_week_group[2].checked,
                    "Thu": input_alarm_week_group[3].checked,
                    "Fri": input_alarm_week_group[4].checked,
                    "Sat": input_alarm_week_group[5].checked,
                    "Sun": input_alarm_week_group[6].checked,
                }
            } else {
                var data = {
                    "switch": "open",
                    "repetition": "no",
                    "shi": input_alarm_shi,
                    "fen": input_alarm_fen,
                    "Mon": 0,
                    "Tue": 0,
                    "Wed": 0,
                    "Thu": 0,
                    "Fri": 0,
                    "Sat": 0,
                    "Sun": 0,
                }
            }
        } else {
            var data = {
                "switch": "no",
                "repetition": "no",
                "shi": 0,
                "fen": 0,
                "Mon": 0,
                "Tue": 0,
                "Wed": 0,
                "Thu": 0,
                "Fri": 0,
                "Sat": 0,
                "Sun": 0,
            }
        }
        xhttp.send(JSON.stringify(data));
    }
}

