// $(document).bind("ready", function () {
//     document.getElementById("dd").addEventListener("change", trigger);
 
//     function trigger() {
//       javascript:change_essay();
//     }
// });
$(document).ready(function(){
  $('#dd').change(function(){
    change_essay();
  });
});