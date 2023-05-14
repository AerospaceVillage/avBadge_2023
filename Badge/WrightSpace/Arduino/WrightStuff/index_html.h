
const char *index_html_code =  "<html>\
	<head>\
		<meta name='viewport' content='width=device-width, initial-scale=1'>\
 	  <title>The Wright Stuff</title>\
    <style>\
      .button { background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; }\
    </style>\
	</head>\
	<body bgcolor='white'>\
		<h1>Aerospace Village</h1>\
    <ul><li>Build</li><li>Inspire</li><li>Promote</li></ul>\
		<p>Color Index: %i</p>\
    <p>Change the satelletile <a href=\"/C\" class=\"button\">COLOR</a></p>\
    <p><a href=\"/P\" class=\"button\">Pulse</a> the satelletile </p>\
    <p><a href=\"/A\" class=\"button\">Toggle</a> the alien</p>\
    <form action='' method='post'><input type='color' id='favcolor' name='favcolor' value='#ff0000'><input type='submit' class='button'></form>\
    <p><a href=\"/\" class=\"button\">HOME</a></p>\
    <form action=\"/\" method=\"post\"><input type='hidden' name='twinkle_value' value='5'></input><input type='submit' value='Twinkle' class='button'></input></form>\
	</body>\
</html>";
