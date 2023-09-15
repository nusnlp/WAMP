// $(document).bind("ready", function () {
//     document.getElementById("dd_other").addEventListener("change", trigger);
 
//     function trigger() {
//       javascript:change_annotation();
//     }
// });
$(document).ready(function(){
  $('#dd_other').change(function(){
    change_annotation();
  });
});
