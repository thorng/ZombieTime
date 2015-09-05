Pebble.addEventListener('ready',
  function(e) {
    // Get location updates
    id = navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
  }
);

var id;
var latitude;
var longitude;

var locationOptions = {
  enableHighAccuracy: true, 
  maximumAge: 0, 
  timeout: 5000
};


function locationSuccess(pos) {
  console.log('Location changed!');
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
	
	latitude = pos.coords.latitude.toString();
	longitude = pos.coords.longitude.toString();
	// Send to Pebble
// define dictionary and sent it to Pebble
	var dictionary = {
		'LOCATION_LATITUDE': latitude,
		'LOCATION_LONGITUDE': longitude
	};
	
	Pebble.sendAppMessage(dictionary,
  function(e) {
    console.log('Weather info sent to Pebble successfully!');
  },
  function(e) {
    console.log('Error sending weather info to Pebble!');
  }
	);
}

function locationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);
}




// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
  }                     
);