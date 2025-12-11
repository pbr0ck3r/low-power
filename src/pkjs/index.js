// PebbleKit JS - handles settings communication
// Similar to PebbleAI's settings implementation

var Clay = require('./clay');

// Clay configuration - inline like PebbleAI
var clayConfig = [
  {
    type: "section",
    items: [
      {
        type: "heading",
        defaultValue: "Display Settings"
      },
      {
        type: "toggle",
        messageKey: "ShowDateInfo",
        label: "Show Date Info",
        description: "Toggle to show/hide day, month, and date",
        defaultValue: true
      }
    ]
  },
  {
    type: "submit",
    defaultValue: "Save Settings"
  }
];

var clay = new Clay(clayConfig);

// Handle app messages from watch
Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received!');
});

