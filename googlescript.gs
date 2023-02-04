var SS = SpreadsheetApp.openById('1dsI13z8patw5KM7wljgk4NsespfYh_3Ryt86tuvYLGY');
var str = "";

function doGet(e) { 
Logger.log( JSON.stringify(e) );
var result = 'Ok';
if (e.parameter == 'undefined') {
result = 'No Parameters';
}
else {
var sheet_id = '1dsI13z8patw5KM7wljgk4NsespfYh_3Ryt86tuvYLGY'; // Spreadsheet ID
var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();
var newRow = sheet.getLastRow() + 1; 
var rowData = [];
var Curr_Date = new Date();
rowData[0] = Curr_Date; // Date in column A
var Curr_Time = Utilities.formatDate(Curr_Date, 'GMT+5:30', 'HH:mm:ss');
rowData[1] = Curr_Time; // Time in column B
for (var param in e.parameter) {
Logger.log('In for loop, param=' + param);
var value = stripQuotes(e.parameter[param]);
Logger.log(param + ':' + e.parameter[param]);
switch (param) {
case 'temperaturein':
rowData[2] = value; // Temperature in column C
result = 'Temperaturein Written on column C'; 
break;
case 'humidityin':
rowData[3] = value; // Humidity in column D
result += 'Humidityin Written on column D'; 
break; 
case 'temperatureout':
rowData[4] = value; 
result += 'Temperatureout Written on column E'; 
break; 
case 'humidityout':
rowData[5] = value; // Humidity in column D
result += 'Humidityout Written on column F'; 
break; 
case 'temphot':
rowData[6] = value;
result += 'Temp hot ';
break;
case 'tempcold':
rowData[7] = value;
result += 'Temp cold';
break;
default:
result = "unsupported parameter";
}
}
Logger.log(JSON.stringify(rowData));
var newRange = sheet.getRange(newRow, 1,1,rowData.length);
newRange.setValues([rowData]);
}
return ContentService.createTextOutput(result);
}
function stripQuotes( value ) {
return value.replace(/^["']|['"]$/g, "");
}
