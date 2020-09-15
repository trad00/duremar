function buttonClick(i, mode) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/switch", true);
  xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  xhttp.onreadystatechange = function() {
    if (this.readyState == XMLHttpRequest.DONE && this.status == 200) {
      update();
    }
  }
  xhttp.send("i="+i+"&m="+mode);
}

function update() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == XMLHttpRequest.DONE) {
      if (this.status == 200) {
        var data = JSON.parse(this.responseText);
        var elem;
        for (let i = 0; i < 4; i++) {
          elem = document.getElementById("t"+i);
          elem.innerHTML = data[i];
        }
        for (let i = 0; i < 4; i++) {
          elem = document.getElementById("r"+i);
          if (data[i+4] == 0)
            elem.classList.remove("RbtnOn");
          else
            elem.classList.add("RbtnOn");
        }
      } else {
        var elem;
        for (let i = 0; i < 4; i++) {
          elem = document.getElementById("t"+i);
          elem.innerHTML = "--";
        }
      }
    }
  };
  xhttp.open("GET", "/data", true);
  xhttp.send();
  setTimeout(update, 2500);
};
