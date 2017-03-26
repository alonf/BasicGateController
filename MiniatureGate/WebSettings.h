#include <String>
#ifndef _WEBSETTINGSTEMPLATE_h
#define _WEBSETTINGSTEMPLATE_h

const static char WebSettingHtmlTemplate[] PROGMEM = R"(
<html xmlns="http://www.w3.org/1999/xhtml"><head> <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/> <title>Cloud Controlled Gate Setup</title><link rel="stylesheet" type="text/css" href="view.css" media="all"><script src="ap_script.js"></script></head><body class="no_guidelines safari" onload="add_ap() "> <div id="form_container"> <h1><a>Cloud Controlled Gate Setup</a></h1> <form id="set-access-point" class="appnitro" method="post" action="setconfiguration"> <div class="form_description"> <p>These values will be saved on the device eeprom. eeprom reset: press the button for 20 seconds.</p></div><ul> <li class="section_break"> <h3>Router Settings</h3> <p> On submit the device will reboot and will try to connect to the chosen network. The green Led light will blink the IP Address. </p></li><li> <label class="dsc">Choose WiFi Network:</label><span id="AP"> </span> </li><li> <label class="dsc">WiFi Password </label> <div> <input name="WFPwd" type="password" maxlength="64" value="<%=WFPwd%>"/> </div></li><li> <h3>Microsoft Azure IoT Hub Configuration</h3> <p> Enter the Azure IoT Hub device connection string. For more information: <a href="https://azure.microsoft.com/en-us/services/iot-hub/">here</a> and <a href="https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-sdk-c-intro/">here.</a> </p></li><li> <label class="dsc">Azure IoT Hub Connection String: </label> <div> <input name="IoTConStr" class="element text medium" type="text" size="160" maxlength="255" value="<%=IoTConStr%>"/> </div></li><li> <label class="dsc">Device Id Name </label> <div> <input name="DeviceId" class="element text medium" type="text" maxlength="64" value="<%=DeviceId%>"/> </div></li><li> <label class="dsc" for="WebOrIoT">Use Azure IoT Hub or Web Server for issuing commands </label> <span> <input name="WebOrIoT" class="element radio" type="radio" value="WebSrv" <%=WebSrv%>/> <label class="choice">ESP8266 Web Server</label> <input name="WebOrIoT" type="radio" class="element radio" value="IoT" <%=IoT%>/> <label class="choice">Azure IoT Hub</label> </span> </li><li class="section_break"> <h3>Push Button settings</h3> </li><li> <label class="dsc">Long press period </label> <div> <input name="PBLng" class="element text medium" type="number" min="1000" value="<%=PBLng%>"/> </div></li><li> <label class="dsc">Very long press period </label> <div> <input name="PBVLng" class="element text medium" type="number" min="1500" value="<%=PBVLng%>"/> </div></li><li class="buttons"> <input type="submit" name="submit" class="button_text" value="Submit"/> </li></ul> </form> </div></body></html>
)";
/* http://www.willpeavy.com/minifier/
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <title>Cloud Controlled Gate Setup</title>
	<link rel="stylesheet" type="text/css" href="view.css" media="all">
	<script src="ap_script.js"></script>
</head>
<body class="no_guidelines safari" onload="add_ap() ">
    <div id="form_container">
        <h1><a>Cloud Controlled Gate Setup</a></h1>
        <form id="set-access-point" class="appnitro" method="post" action="setconfiguration">
            <div class="form_description">
                <p>These values will be saved on the device eeprom.  eeprom reset: press the button for 20 seconds.</p>
            </div>
            <ul>
                <li class="section_break">
                    <h3>Router Settings</h3>
                    <p>
                        On submit the device will reboot and will try to connect to the chosen network. The green Led light will blink the IP Address.
                    </p>
                </li>
                <li>
                    <label class="dsc">Choose WiFi Network:</label>
					<span  id="AP">
                        
                    </span>
                </li>
                <li>
                    <label class="dsc">WiFi Password </label>
                    <div>
                        <input name="WFPwd" type="password" maxlength="64" value="<%=WFPwd%>" />
                    </div>
                </li>
                <li>
                    <h3>Microsoft Azure IoT Hub Configuration</h3>
                    <p>
                        Enter the Azure IoT Hub device connection string.
                        For more information: <a href="https://azure.microsoft.com/en-us/services/iot-hub/">here</a> and <a href="https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-sdk-c-intro/">here.</a>
                    </p>
                </li>
                <li>
                    <label class="dsc">Azure IoT Hub Connection String: </label>
                    <div>
                        <input name="IoTConStr" class="element text medium" type="text" size="160" maxlength="255" value="<%=IoTConStr%>" />
                    </div>
                </li>
                <li>
                    <label class="dsc">Device Id Name </label>
                    <div>
                        <input name="DeviceId" class="element text medium" type="text" maxlength="64" value="<%=DeviceId%>" />
                    </div>
                </li>
                <li>
                    <label class="dsc" for="WebOrIoT">Use Azure IoT Hub or Web Server for issuing commands </label>
                    <span>
                            <input name="WebOrIoT" class="element radio"  type="radio" value="WebSrv" <%=WebSrv%> /> 
                            <label class="choice">ESP8266 Web Server</label>
                            <input name="WebOrIoT" type="radio" class="element radio"  value="IoT" <%=IoT%>/>
                            <label class="choice">Azure IoT Hub</label>
                    </span>
                </li>
                <li class="section_break">
                    <h3>Push Button settings</h3>
                </li>
                <li>
                    <label class="dsc">Long press period </label>
                    <div>
                        <input name="PBLng" class="element text medium" type="number" min="1000" value="<%=PBLng%>" />
                    </div>
                </li>
                <li>
                    <label class="dsc">Very long press period </label>
                    <div>
                        <input name="PBVLng" class="element text medium" type="number" min="1500" value="<%=PBVLng%>" />
                    </div>
                </li>
                <li class="buttons">
                    <input type="submit" name="submit" class="button_text" value="Submit" />
                </li>
            </ul>
        </form>
    </div>
</body>
</html>
*/

const static char APScript[] PROGMEM = R"(
function add_ap()
{
	xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function()
	{
		if (this.readyState == 4 && this.status == 200)
		{
			document.getElementById("AP").innerHTML = this.responseText;
		}
	};
	xhttp.open("GET", "aplist.html", true);
	xhttp.send();
	return;
}
)";

//view.css
const static char ViewCSS[] PROGMEM = 
R"(
body
{
	background:#336699;
	font-family:"Lucida Grande", Tahoma, Arial, Verdana, sans-serif;
	font-size:small;
	margin:8px 0 16px;
	text-align:center;
}

#form_container
{
	background:#fff;
	
	margin:0 auto;
	text-align:left;
	width:640px;
}

#top
{
	display:block;
	height:10px;
	margin:10px auto 0;
	width:650px;
}

#footer
{
	width:640px;
	clear:both;
	color:#999999;
	text-align:center;
	width:640px;
	padding-bottom: 15px;
	font-size: 85%;
}

#footer a{
	color:#999999;
	text-decoration: none;
	border-bottom: 1px dotted #999999;
}

#bottom
{
	display:block;
	height:10px;
	margin:0 auto;
	width:650px;
}

form.appnitro
{
	margin:20px 20px 0;
	padding:0 0 20px;
}

/**** Logo Section  *****/
h1
{
	background-color:#6699CC;
	margin:0;
	min-height:0;
	padding:0;
	text-decoration:none;
	text-indent:-8000px;	
}

h1 a
{
	display:block;
	height:20px;
	min-height:40px;
	overflow:hidden;
}

/**** Form Section ****/
.appnitro
{
	font-family:Lucida Grande, Tahoma, Arial, Verdana, sans-serif;
	font-size:small;
}

.appnitro li
{
	width:61%;
}

form ul
{
	font-size:100%;
	list-style-type:none;
	margin:0;
	padding:0;
	width:100%;
}

form li
{
	display:block;
	margin:0;
	padding:4px 5px 2px 9px;
	position:relative;
}

form li:after
{
	clear:both;
	content:".";
	display:block;
	height:0;
	visibility:hidden;
}

.buttons:after
{
	clear:both;
	content:".";
	display:block;
	height:0;
	visibility:hidden;
}

.buttons
{
	clear:both;
	display:block;
	margin-top:10px;
}

* html form li
{
	height:1%;
}

* html .buttons
{
	height:1%;
}

* html form li div
{
	display:inline-block;
}

form li div
{
	color:#444;
	margin:0 4px 0 0;
	padding:0 0 8px;
}

form li span
{
	color:#444;
	float:left;
	margin:0 4px 0 0;
	padding:0 0 8px;
}

form li div.left
{
	display:inline;
	float:left;
	width:48%;
}

form li div.right
{
	display:inline;
	float:right;
	width:48%;
}

form li div.left .medium
{
	width:100%;
}

form li div.right .medium
{
	width:100%;
}

.clear
{
	clear:both;
}

form li div label
{
	clear:both;
	color:#444;
	display:block;
	font-size:9px;
	line-height:9px;
	margin:0;
	padding-top:3px;
}

form li span label
{
	clear:both;
	color:#444;
	display:block;
	font-size:9px;
	line-height:9px;
	margin:0;
	padding-top:3px;
}

form li .datepicker
{
	cursor:pointer !important;
	float:left;
	height:16px;
	margin:.1em 5px 0 0;
	padding:0;
	width:16px;
}

.form_description
{
	border-bottom:1px dotted #ccc;
	clear:both;
	display:inline-block;
	margin:0 0 1em;
}

.form_description[class]
{
	display:block;
}

.form_description h2
{
	clear:left;
	font-size:160%;
	font-weight:400;
	margin:0 0 3px;
}

.form_description p
{
	font-size:95%;
	line-height:130%;
	margin:0 0 12px;
}

form hr
{
	display:none;
}

form li.section_break
{
	border-top:1px dotted #ccc;
	margin-top:9px;
	padding-bottom:0;
	padding-left:9px;
	padding-top:13px;
	width:97% !important;
}

form ul li.first
{
	border-top:none !important;
	margin-top:0 !important;
	padding-top:0 !important;
}

form .section_break h3
{
	font-size:110%;
	font-weight:400;
	line-height:130%;
	margin:0 0 2px;
}

form .section_break p
{
	font-size:85%;

	margin:0 0 10px;
}

/**** Buttons ****/
input.button_text
{
	overflow:visible;
	padding:0 7px;
	width:auto;
}

.buttons input
{
	font-size:120%;
	margin-right:5px;
}

/**** Inputs and Labels ****/
label.dsc
{
	border:none;
	color:#222;
	display:block;
	font-size:95%;
	font-weight:700;
	line-height:150%;
	padding:0 0 1px;
}

span.symbol
{
	font-size:115%;
	line-height:130%;
}

input.text
{
	background:#fff  top;
	border-bottom:1px solid #ddd;
	border-left:1px solid #c3c3c3;
	border-right:1px solid #c3c3c3;
	border-top:1px solid #7c7c7c;
	color:#333;
	font-size:100%;
	margin:0;
	padding:2px 0;
}

input.file
{
	color:#333;
	font-size:100%;
	margin:0;
	padding:2px 0;
}

textarea.textarea
{
	background:#fff;
	border-bottom:1px solid #ddd;
	border-left:1px solid #c3c3c3;
	border-right:1px solid #c3c3c3;
	border-top:1px solid #7c7c7c;
	color:#333;
	font-family:"Lucida Grande", Tahoma, Arial, Verdana, sans-serif;
	font-size:100%;
	margin:0;
	width:99%;
}

select.select
{
	color:#333;
	font-size:100%;
	margin:1px 0;
	padding:1px 0 0;
	background:#fff;
	border-bottom:1px solid #ddd;
	border-left:1px solid #c3c3c3;
	border-right:1px solid #c3c3c3;
	border-top:1px solid #7c7c7c;
}


input.currency
{
	text-align:right;
}

input.checkbox
{
	display:block;
	height:13px;
	line-height:1.4em;
	margin:6px 0 0 3px;
	width:13px;
}

input.radio
{
	display:block;
	height:13px;
	line-height:1.4em;
	margin:6px 0 0 3px;
	width:13px;
}

label.choice
{
	color:#444;
	display:block;
	font-size:100%;
	line-height:1.4em;
	margin:-1.55em 0 0 25px;
	padding:4px 0 5px;
	width:90%;
}

select.select[class]
{
	margin:0;
	padding:1px 0;
}

*:first-child+html select.select[class]
{
	margin:1px 0;
}

.safari select.select
{
	font-size:120% !important;
	margin-bottom:1px;
}

input.small
{
	width:25%;
}

select.small
{
	width:25%;
}

input.medium
{
	width:50%;
}

select.medium
{
	width:50%;
}

input.large
{
	width:99%;
}

select.large
{
	width:100%;
}

textarea.small
{
	height:5.5em;
}

textarea.medium
{
	height:10em;
}

textarea.large
{
	height:20em;
}

/**** Errors ****/
#error_message
{
	background:#fff;
	border:1px dotted red;
	margin-bottom:1em;
	padding-left:0;
	padding-right:0;
	padding-top:4px;
	text-align:center;
	width:99%;
}

#error_message_title
{
	color:#DF0000;
	font-size:125%;
	margin:7px 0 5px;
	padding:0;
}

#error_message_desc
{
	color:#000;
	font-size:100%;
	margin:0 0 .8em;
}

#error_message_desc strong
{
	background-color:#FFDFDF;
	color:red;
	padding:2px 3px;
}

form li.error
{
	background-color:#FFDFDF !important;
	border-bottom:1px solid #EACBCC;
	border-right:1px solid #EACBCC;
	margin:3px 0;
}

form li.error label
{
	color:#DF0000 !important;
}

form p.error
{
	clear:both;
	color:red;
	font-size:10px;
	font-weight:700;
	margin:0 0 5px;
}

form .required
{
	color:red;
	float:none;
	font-weight:700;
}

/**** Guidelines and Error Highlight ****/
form li.highlighted
{
	background-color:#fff7c0;
}

form .guidelines
{
	background:#f5f5f5;
	border:1px solid #e6e6e6;
	color:#444;
	font-size:80%;
	left:100%;
	line-height:130%;
	margin:0 0 0 8px;
	padding:8px 10px 9px;
	position:absolute;
	top:0;
	visibility:hidden;
	width:42%;
	z-index:1000;
}

form .guidelines small
{
	font-size:105%;
}

form li.highlighted .guidelines
{
	visibility:visible;
}

form li:hover .guidelines
{
	visibility:visible;
}

.no_guidelines .guidelines
{
	display:none !important;
}

.no_guidelines form li
{
	width:97%;
}

.no_guidelines li.section
{
	padding-left:9px;
}

/*** Success Message ****/
.form_success 
{
	clear: both;
	margin: 0;
	padding: 90px 0pt 100px;
	text-align: center
}

.form_success h2 {
    clear:left;
    font-size:160%;
    font-weight:normal;
    margin:0pt 0pt 3px;
}

/*** Password ****/
ul.password{
    margin-top:60px;
    margin-bottom: 60px;
    text-align: center;
}
.password h2{
    color:#DF0000;
    font-weight:bold;
    margin:0pt auto 10px;
}

.password input.text {
   font-size:170% !important;
   width:380px;
   text-align: center;
}
.password label{
   display:block;
   font-size:120% !important;
   padding-top:10px;
   font-weight:bold;
}

#li_captcha{
   padding-left: 5px;
}


#li_captcha span{
	float:none;
}

/** Embedded Form **/

.embed #form_container{
	border: none;
}

.embed #top, .embed #bottom, .embed h1{
	display: none;
}

.embed #form_container{
	width: 100%;
}

.embed #footer{
	text-align: left;
	padding-left: 10px;
	width: 99%;
}

.embed #footer.success{
	text-align: center;
}

.embed form.appnitro
{
	margin:0px 0px 0;
	
}
)";


#endif //_WEBSETTINGSTEMPLATE_h