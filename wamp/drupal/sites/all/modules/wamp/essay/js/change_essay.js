function change_essay(){

    var $essay = document.navi.dd.options[document.navi.dd.selectedIndex].value;
    var $dest = Drupal.settings.basePath;
    $dest += "wamp/wamp_essay/";
    $dest += $essay;
    //$dest += "/annotate_essay";
    location.href=$dest;
}