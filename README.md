# WiFi-Weather
Wifi enabled Arduino Weather station sending data to Weather Underground

WiFi Weather Station that sends data to Weather Underground for storage and viewing


Notes:
This project has options for battery power -- to save power it will rest for awhile, wake, send data and rest again.
You will need to sign up for a user account at wunderground.com, to get your pass
When you register a station you will get an ID

Sign up at http://www.wunderground.com/personal-weather-station/signup.asp

Wunderground wants UTC Zulu, not local time, if your RTC is local, offset it in code.
Wunderground Upload guidelines: http://wiki.wunderground.com/index.php/PWS_-_Upload_Protocol
This code assumes your wifi credentials are already saved in the CC3000's non-volitile memory
