Drupal.behaviors.wamp_essay = function (context) {
    var ui_html = Drupal.settings.wamp_essay.ui_html;
    var firstSlash = cur_loc.indexOf('/',1); var uiURL = cur_loc.substr(0, firstSlash); uiURL += `/wamp_includes/${ui_html}`;
    $(document).bind('ready', function() {
        var ui = new EditUI(uiURL, annotationsURL);
    });
 }