// Color mapping - names to hex values
var COLOR_MAP = {
  'tomato': 0xFF6347,
  'sandybrown': 0xF4A460,
  'gold': 0xFFD700,
  'aquamarine': 0x7FFFD4,
  'green': 0x4aa951,
  'deepskyblue': 0x00BFFF,
  'plum': 0xDDA0DD,
  'white': 0xFFFFFF,
  'black': 0x000000
};

// Generate the configuration page HTML locally
function getConfigHTML() {
  var showDate = localStorage.getItem('ShowDate');
  var showSeconds = localStorage.getItem('ShowSeconds');
  var textColor = localStorage.getItem('TextColor') || 'white';
  var bgColor = localStorage.getItem('BackgroundColor') || 'black';
  
  var dateChecked = (showDate === null || showDate === 'true') ? 'checked' : '';
  var secondsChecked = (showSeconds === 'true') ? 'checked' : '';
  
  // Check if the watch supports color
  var watchInfo = Pebble.getActiveWatchInfo();
  var supportsColor = watchInfo && (
    watchInfo.platform === 'basalt' || 
    watchInfo.platform === 'chalk' || 
    watchInfo.platform === 'emery'
  );
  
  console.log('Watch platform:', watchInfo ? watchInfo.platform : 'unknown');
  console.log('Supports color:', supportsColor);
  
  return '<!DOCTYPE html>' +
    '<html><head>' +
    '<meta charset="utf-8">' +
    '<meta name="viewport" content="width=device-width, initial-scale=1">' +
    '<title>Low Power Configuration</title>' +
    '<style>' +
    'body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; margin: 0; padding-top: 56px; background-color: #f5f5f5; }' +
    '.nav-bar { position: fixed; top: 0; left: 0; right: 0; z-index: 100; background: #f8f8f8; border-bottom: 1px solid #ddd; height: 48px; display: flex; align-items: center; justify-content: space-between; padding: 0 12px; }' +
    '.nav-title { font-size: 17px; font-weight: 600; color: #333; }' +
    '.nav-actions { display: flex; gap: 8px; align-items: center; }' +
    '.nav-btn { background: none; border: none; font-size: 15px; cursor: pointer; padding: 6px 8px; border-radius: 6px; }' +
    '.nav-btn-cancel { color: #666; }' +
    '.nav-btn-reset { color: #FF6B35; }' +
    '.nav-btn-save { color: #4CAF50; font-weight: 700; }' +
    '.container { max-width: 600px; margin: 12px auto; background-color: white; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); padding: 20px; }' +
    'h1 { margin-top: 0; color: #333; font-size: 24px; }' +
    'h2 { color: #555; font-size: 18px; margin-top: 20px; margin-bottom: 10px; }' +
    '.setting-item { padding: 15px 0; border-bottom: 1px solid #e0e0e0; display: flex; justify-content: space-between; align-items: center; }' +
    '.setting-item:last-child { border-bottom: none; }' +
    '.setting-label { font-size: 16px; color: #333; }' +
    '.setting-description { font-size: 13px; color: #666; margin-top: 4px; }' +
    '.toggle-switch { position: relative; width: 50px; height: 28px; }' +
    '.toggle-switch input { opacity: 0; width: 0; height: 0; }' +
    '.slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 28px; }' +
    '.slider:before { position: absolute; content: ""; height: 20px; width: 20px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }' +
    'input:checked + .slider { background-color: #4CAF50; }' +
    'input:checked + .slider:before { transform: translateX(22px); }' +
    '.color-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin-top: 10px; }' +
    '.color-option { width: 100%; padding-top: 100%; position: relative; border-radius: 8px; cursor: pointer; border: 3px solid transparent; transition: all 0.2s; }' +
    '.color-option.selected { border-color: #4CAF50; box-shadow: 0 0 0 2px #4CAF50; }' +
    '.color-option:active { transform: scale(0.95); }' +
    '</style>' +
    '</head><body>' +
    '<div class="nav-bar">' +
    '  <span class="nav-title">Low Power</span>' +
    '  <div class="nav-actions" id="navActions">' +
    '    <button class="nav-btn nav-btn-cancel" onclick="cancelSettings()">Cancel</button>' +
    '  </div>' +
    '</div>' +
    '<div class="container">' +
    '<h1>Settings</h1>' +

    '<div class="setting-item">' +
    '<div>' +
    '<div class="setting-label">Show Date</div>' +
    '<div class="setting-description">Display weekday, month, and day</div>' +
    '</div>' +
    '<label class="toggle-switch">' +
    '<input type="checkbox" id="showDate" ' + dateChecked + ' onchange="checkForChanges()">' +
    '<span class="slider"></span>' +
    '</label>' +
    '</div>' +
    
    '<div class="setting-item">' +
    '<div>' +
    '<div class="setting-label">Show Seconds</div>' +
    '<div class="setting-description">Display seconds on middle left</div>' +
    '</div>' +
    '<label class="toggle-switch">' +
    '<input type="checkbox" id="showSeconds" ' + secondsChecked + ' onchange="checkForChanges()">' +
    '<span class="slider"></span>' +
    '</label>' +
    '</div>' +
    
    // Only show color pickers on color watches
    (supportsColor ? 
      '<h2>Text Color</h2>' +
      '<div class="color-grid" id="textColorGrid"></div>' +
      '<h2>Background Color</h2>' +
      '<div class="color-grid" id="bgColorGrid"></div>' 
      : 
      '<div style="padding: 20px 0; color: #999; font-size: 14px; text-align: center;">' +
      'Color settings not available on black & white watches' +
      '</div>'
    ) +
    
    '</div>' +
    
    '<script>' +
    'var colors = [' +
    '  { name: "tomato", value: "tomato" },' +
    '  { name: "sandybrown", value: "sandybrown" },' +
    '  { name: "gold", value: "gold" },' +
    '  { name: "aquamarine", value: "aquamarine" },' +
    '  { name: "green", value: "#4aa951" },' +
    '  { name: "deepskyblue", value: "deepskyblue" },' +
    '  { name: "plum", value: "plum" },' +
    '  { name: "white", value: "white" },' +
    '  { name: "black", value: "black" }' +
    '];' +
    
    'var selectedTextColor = "' + textColor + '";' +
    'var selectedBgColor = "' + bgColor + '";' +
    'var supportsColor = ' + supportsColor + ';' +
    'var initialState = { showDate: ' + (dateChecked ? 'true' : 'false') + ', showSeconds: ' + (secondsChecked ? 'true' : 'false') + ', textColor: "' + textColor + '", bgColor: "' + bgColor + '" };' +

    'function checkForChanges() {' +
    '  var hasChanges = ' +
    '    document.getElementById("showDate").checked !== initialState.showDate ||' +
    '    document.getElementById("showSeconds").checked !== initialState.showSeconds ||' +
    '    selectedTextColor !== initialState.textColor ||' +
    '    selectedBgColor !== initialState.bgColor;' +
    '  var el = document.getElementById("navActions");' +
    '  if (hasChanges) {' +
    '    el.innerHTML = \'<button class="nav-btn nav-btn-reset" onclick="resetChanges()">Reset Changes</button><button class="nav-btn nav-btn-save" onclick="saveSettings()">Save</button>\';' +
    '  } else {' +
    '    el.innerHTML = \'<button class="nav-btn nav-btn-cancel" onclick="cancelSettings()">Cancel</button>\';' +
    '  }' +
    '}' +

    'function resetChanges() {' +
    '  document.getElementById("showDate").checked = initialState.showDate;' +
    '  document.getElementById("showSeconds").checked = initialState.showSeconds;' +
    '  selectedTextColor = initialState.textColor;' +
    '  selectedBgColor = initialState.bgColor;' +
    '  if (supportsColor) {' +
    '    document.getElementById("textColorGrid").innerHTML = "";' +
    '    document.getElementById("bgColorGrid").innerHTML = "";' +
    '    createColorGrid("textColorGrid", selectedTextColor, "text");' +
    '    createColorGrid("bgColorGrid", selectedBgColor, "bg");' +
    '  }' +
    '  checkForChanges();' +
    '}' +

    'function cancelSettings() { document.location = "pebblejs://close"; }' +

    'function createColorGrid(gridId, selectedColor, varName) {' +
    '  var grid = document.getElementById(gridId);' +
    '  if (!grid) return;' +
    '  colors.forEach(function(color) {' +
    '    var div = document.createElement("div");' +
    '    div.className = "color-option" + (color.name === selectedColor ? " selected" : "");' +
    '    div.style.backgroundColor = color.value;' +
    '    if (color.name === "white") { div.style.border = "3px solid #ddd"; }' +
    '    div.onclick = function() {' +
    '      var options = grid.querySelectorAll(".color-option");' +
    '      options.forEach(function(opt) { opt.classList.remove("selected"); });' +
    '      div.classList.add("selected");' +
    '      if (varName === "text") { selectedTextColor = color.name; }' +
    '      else { selectedBgColor = color.name; }' +
    '      checkForChanges();' +
    '    };' +
    '    grid.appendChild(div);' +
    '  });' +
    '}' +

    'if (supportsColor) {' +
    '  createColorGrid("textColorGrid", selectedTextColor, "text");' +
    '  createColorGrid("bgColorGrid", selectedBgColor, "bg");' +
    '}' +

    'function saveSettings() {' +
    '  var settings = { ' +
    '    showDate: document.getElementById("showDate").checked.toString(), ' +
    '    showSeconds: document.getElementById("showSeconds").checked.toString()' +
    '  };' +
    '  if (supportsColor) {' +
    '    settings.textColor = selectedTextColor;' +
    '    settings.bgColor = selectedBgColor;' +
    '  }' +
    '  document.location = "pebblejs://close#" + encodeURIComponent(JSON.stringify(settings));' +
    '}' +
    '</script>' +
    '</body></html>';
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received!');
});

// Listen for when the configuration window is opened
Pebble.addEventListener('showConfiguration', function(e) {
  // Generate and show the local config page
  var html = getConfigHTML();
  Pebble.openURL('data:text/html;charset=utf-8,' + encodeURIComponent(html));
});

// Listen for when the configuration window is closed
Pebble.addEventListener('webviewclosed', function(e) {
  if (e.response) {
    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration page returned: ' + JSON.stringify(configData));

    var dict = {};
    
    if (configData.showDate !== undefined) {
      var showDate = configData.showDate === 'true';
      localStorage.setItem('ShowDate', showDate);
      dict.ShowDate = showDate ? 1 : 0;
    }
    
    if (configData.showSeconds !== undefined) {
      var showSeconds = configData.showSeconds === 'true';
      localStorage.setItem('ShowSeconds', showSeconds);
      dict.ShowSeconds = showSeconds ? 1 : 0;
    }
    
    if (configData.textColor !== undefined) {
      var textColor = configData.textColor;
      localStorage.setItem('TextColor', textColor);
      dict.TextColor = COLOR_MAP[textColor] || COLOR_MAP.white;
    }
    
    if (configData.bgColor !== undefined) {
      var bgColor = configData.bgColor;
      localStorage.setItem('BackgroundColor', bgColor);
      dict.BackgroundColor = COLOR_MAP[bgColor] || COLOR_MAP.black;
    }
    
    // Send all settings to watch
    if (Object.keys(dict).length > 0) {
      Pebble.sendAppMessage(dict, function() {
        console.log('Send successful: ' + JSON.stringify(dict));
      }, function() {
        console.log('Send failed!');
      });
    }
  }
});
