function change_annotation(){

    var $essay = document.navi_other.dd_other.options[document.navi_other.dd_other.selectedIndex];
    var $dest = Drupal.settings.basePath;
    if (location.href.indexOf("/admin") != -1) $dest += "admin/";
    $dest += "wamp/wamp_essay/";
    $dest += $essay.value;
    
    if ($essay.value != "-1") {
        //location.href=$dest;
        document.navi_other.dd_other.selectedIndex = 0;
        if ($essay.text == "Own Annotation") location.href=$dest;
        else window.open($dest, Math.random(), "menubar=yes,scrollbars=yes");
    }
}