var cur_loc = location.pathname; var lastSlash = cur_loc.lastIndexOf('/'); var annotationsURL = cur_loc.substr(0, lastSlash); annotationsURL += '/annotation_readonly';
$(document).bind('ready', function() {
    var corrector = new Corrector(annotationsURL);
});