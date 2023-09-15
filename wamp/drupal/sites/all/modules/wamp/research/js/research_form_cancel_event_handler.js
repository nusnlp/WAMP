$(document).bind("ready", function () {
  document.getElementById("research_form_cancel_btn").addEventListener("click", trigger);

  function trigger() {
    location.href = history.back();
  }
});