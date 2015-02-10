# WiFi-Weather
Wifi enabled Arduino Weather station sending data to Weather Underground

Sparkfun Wifi shield on an Arduino Uno: Weather station that sends data to wunderground.com

Notes:
This project assumes battery power and to save power it will rest for awhile, wake, send data and rest again.
You will need to sign up for a user account at wunderground.com, to get your pass
When you register a station you will get an ID
Wunderground wants UTC Zulu, not local time, if your RTC is local, offset it in code.
Wunderground Upload guidelines: http://wiki.wunderground.com/index.php/PWS_-_Upload_Protocol
This code assumes your wifi credentials are already saved in the CC3000's non-volitile memory
