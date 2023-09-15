$(document).bind("ready", function () {
    document.getElementById("edit-bad-essay").addEventListener("change", submit);
    document
      .getElementById("edit-needs-editing")
      .addEventListener("change", submit);
 
    function submit() {
      this.form.submit("wamp_essay_flag_form");
    }
});