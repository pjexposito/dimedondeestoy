var latitude;
var longitude;
var dict;

var locationOptions = { enableHighAccuracy:true, "timeout": 15000, "maximumAge": 7000 };

function HTTPGET(url) {
	var req = new XMLHttpRequest();
	req.open("GET", url, false);
	req.send(null);
	return req.responseText;
}

var obtenDireccion = function() {

  // Usa Google para obtener la información
  var response = HTTPGET("http://maps.googleapis.com/maps/api/geocode/json?latlng="+latitude+","+longitude+"&sensor=true");

  
	//Convierte la respuesta de la web de Google en JSON, para parsearla
	var json = JSON.parse(response);
	

  
  var calle = json.results[0].address_components[1].long_name;
  var numero = json.results[0].address_components[0].long_name;
  var situacion = latitude + ", " + longitude;
	
	//Mando a la consola los datos que he obtenido
  console.log("Estás en:" + latitude + ", " + longitude + ". La calle es: " + calle + " y el número, " + numero);
	
	//Este es el diccionario con los datos que voy a mandar al reloj
  dict = {"KEY_SITUACION" : situacion, "KEY_CALLE": calle, "KEY_NUMERO": numero};
	
	//Mando los datos de dirección al reloj
	Pebble.sendAppMessage(dict);
};

function locationSuccess(pos) {
  var coordinates = pos.coords;
  latitude = coordinates.latitude;
  longitude = coordinates.longitude;
  // Vale, tengo latitud y longitud. Ahora salto a la función de obtener dirección
  console.log("Tengo datos" + coordinates.latitude + " " + coordinates.longitude);
  obtenDireccion();
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  dict = {"KEY_SITUACION" : "Error", "KEY_CALLE": "Error", "KEY_NUMERO": "Error"};
  // Hay un error con el GPS. Se manda el mensaje al reloj
	Pebble.sendAppMessage(dict);

}


Pebble.addEventListener("ready",
  function(e) {
	//Se activa el reloj por primera vez. Se piden los datos por primera vez
  console.log("Evento Ready");

  window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);

  }
);





Pebble.addEventListener("appmessage",
  function(e) {
    //Se refresca la información
    console.log("Evento appmessage");
    window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);

  }
);